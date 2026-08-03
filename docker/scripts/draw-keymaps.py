#!/usr/bin/env python3
import argparse
import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys

# --------------------------------------------------------------------------
# Bracket & Array parsing helpers (from qmk2vial.py)
# --------------------------------------------------------------------------

_OPEN = {"(": ")", "{": "}", "[": "]"}
_CLOSE = set(_OPEN.values())


def find_matching(text: str, open_pos: int) -> int:
    ch = text[open_pos]
    assert ch in _OPEN, f"not an opening bracket: {ch!r}"
    want_close = _OPEN[ch]
    depth = 0
    i = open_pos
    while i < len(text):
        c = text[i]
        if c in _OPEN:
            depth += 1
        elif c in _CLOSE:
            depth -= 1
            if depth == 0:
                if c != want_close:
                    raise ValueError(f"mismatched brackets at {i}")
                return i
        i += 1
    raise ValueError("unterminated bracket")


def split_top_level(text: str, sep: str = ",") -> list[str]:
    depth = 0
    parts = []
    cur = []
    for ch in text:
        if ch in _OPEN:
            depth += 1
        elif ch in _CLOSE:
            depth -= 1
        if ch == sep and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    tail = "".join(cur).strip()
    if tail:
        parts.append(tail)
    return [p.strip() for p in parts if p.strip()]


def extract_array_body(src: str, array_decl_regex: str) -> str:
    m = re.search(array_decl_regex, src)
    if not m:
        raise ValueError(f"pattern not found: {array_decl_regex}")
    brace_start = src.index("{", m.end())
    brace_end = find_matching(src, brace_start)
    return src[brace_start + 1 : brace_end]


def extract_designated_entries(body: str) -> list[tuple[str, str]]:
    out = []
    for entry in split_top_level(body):
        entry = re.sub(r"/\*.*?\*/", "", entry, flags=re.DOTALL)
        entry = re.sub(r"//.*$", "", entry, flags=re.MULTILINE)
        entry = re.sub(r"\n\s*\n+", "\n", entry)
        entry = entry.strip()

        m = re.match(r"^\[\s*([A-Za-z_]\w*)\s*\]\s*=\s*(.*)$", entry, re.S)
        if not m:
            continue
        name, rhs = m.group(1), m.group(2).strip()
        if rhs.startswith("{"):
            inner = rhs[1 : find_matching(rhs, 0)]
        else:
            inner = rhs
        out.append((name, inner))
    return out


# --------------------------------------------------------------------------
# GCC -E Preprocessing Helper (from qmk2vial.py)
# --------------------------------------------------------------------------


def parse_preprocessed(path: Path, keep_files: list[str] | None = None) -> str:
    """Filters preprocessed gcc output to only keep lines originating from specified files/headers."""
    if not keep_files:
        keep_files = ["keymap.c", "keymap.h"]

    with path.open(encoding="utf-8", errors="ignore") as f:
        content = f.readlines()

    keep = False
    out = []

    for line in content:
        if line.startswith("# "):
            parts = line.split('"')
            if len(parts) > 1:
                filename = parts[1]
                keep = any(filename.endswith(fname) for fname in keep_files)
            continue
        if keep:
            out.append(line)

    return "".join(out)


def get_preprocessed_src(
    qmk_tree: str, kb: str, km: str, allowed_headers: list[str]
) -> str | None:
    """Runs qmk compile dry-run and gcc -E to generate fully macro-expanded keymap content."""
    repo = Path(qmk_tree).resolve()
    keymap_file = repo / "keyboards" / kb / "keymaps" / km / "keymap.c"

    if not keymap_file.exists():
        return None

    # Step 1: Run qmk compile to populate cflags
    cmd = ["qmk", "compile", "-kb", kb, "-km", km]
    res = subprocess.run(
        cmd, cwd=repo, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE
    )
    if res.returncode != 0:
        return None

    target = f"{kb.replace('/', '_')}_{km}"
    build_dir = repo / ".build" / f"obj_{target}"
    cflags_file = build_dir / "cflags.txt"

    if not cflags_file.exists():
        return None

    cflags_text = cflags_file.read_text(encoding="utf-8")
    cflags_args = shlex.split(cflags_text)

    # Filter cflags
    filtered_cflags_args = []
    it = iter(cflags_args)
    for arg in it:
        if arg == "-include":
            filtered_cflags_args.extend([arg, next(it)])
        elif arg.startswith(("-I", "-D", "-std")):
            filtered_cflags_args.append(arg)

    preprocessed = build_dir / "keymap.i"

    # Step 2: Run gcc -E
    gcc_cmd = [
        "gcc",
        *filtered_cflags_args,
        "-E",
        "-o",
        str(preprocessed),
        str(keymap_file),
    ]
    res = subprocess.run(
        gcc_cmd, cwd=repo, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE
    )
    if res.returncode != 0 or not preprocessed.exists():
        return None

    return parse_preprocessed(preprocessed, keep_files=allowed_headers)


# --------------------------------------------------------------------------
# Layer Name Extraction
# --------------------------------------------------------------------------


def extract_layer_names(
    local_src: str, qmk_tree: str, kb: str, km: str, allowed_headers: list[str]
) -> list[str]:
    layer_names = []

    # 1. First attempt gcc -E macro expansion
    preprocessed_src = get_preprocessed_src(qmk_tree, kb, km, allowed_headers)
    if preprocessed_src:
        try:
            body = extract_array_body(
                preprocessed_src,
                r"\bkeymaps\s*\[\s*\]\s*\[[^\]]*\]\s*\[[^\]]*\]\s*=",
            )
            entries = extract_designated_entries(body)
            for raw_name, _ in entries:
                clean_name = re.sub(r"^(?:U_|L_|_)", "", raw_name)
                if clean_name:
                    layer_names.append(clean_name)
            if layer_names:
                return layer_names
        except ValueError:
            pass

    # 2. Raw File Fallback (In case compilation or gcc -E failed)
    keymap_dir = os.path.join(local_src, "keyboards", kb, "keymaps", km)
    if not os.path.isdir(keymap_dir):
        return layer_names

    keymap_c = os.path.join(keymap_dir, "keymap.c")
    if os.path.isfile(keymap_c):
        try:
            with open(keymap_c, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()

            try:
                body = extract_array_body(
                    content, r"\bkeymaps\s*\[\s*\]\s*\[[^\]]*\]\s*\[[^\]]*\]\s*="
                )
                entries = extract_designated_entries(body)
                for raw_name, _ in entries:
                    clean_name = re.sub(r"^(?:U_|L_|_)", "", raw_name)
                    if clean_name:
                        layer_names.append(clean_name)
                if layer_names:
                    return layer_names
            except ValueError:
                pass
        except Exception:
            pass

    # Fallback search for enum/X-macros directly
    for fname in allowed_headers:
        fpath = os.path.join(keymap_dir, fname)
        if os.path.isfile(fpath):
            try:
                with open(fpath, "r", encoding="utf-8", errors="ignore") as f:
                    content = f.read()

                # Enum layer definitions
                enum_matches = re.findall(
                    r"enum\s+(?:\w+\s+)?\{\s*([^}]+)\}", content, re.DOTALL
                )
                for enum_body in enum_matches:
                    raw_tokens = [
                        t.strip() for t in re.split(r"[,=]", enum_body) if t.strip()
                    ]
                    valid_tokens = []
                    for token in raw_tokens:
                        token_clean = token.split()[0] if token.split() else ""
                        if token_clean.startswith("//") or token_clean.startswith("/*"):
                            continue
                        if re.match(
                            r"^(?:U_|L_|_)?([A-Za-z0-9_]+)$", token_clean
                        ) and not token_clean.startswith("KC_"):
                            name = re.sub(r"^(?:U_|L_|_)", "", token_clean)
                            if name:
                                valid_tokens.append(name)
                    if len(valid_tokens) >= 2:
                        return valid_tokens

                # Generic macro matching: e.g. MIRYOKU_X(LAYER) or MACRO_X(LAYER)
                macro_matches = re.findall(
                    r"\b[A-Za-z0-9_]+_X\s*\(\s*([A-Za-z0-9_]+)\s*\)", content
                )
                if macro_matches:
                    seen = set()
                    for m in macro_matches:
                        clean = re.sub(r"^(?:U_|L_|_)", "", m)
                        if clean and clean not in seen:
                            seen.add(clean)
                            layer_names.append(clean)
                    if layer_names:
                        return layer_names
            except Exception:
                pass

    return layer_names


def main():
    parser = argparse.ArgumentParser(
        description="Generate SVG keymap drawings from QMK layouts"
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        default=None,
        help="Directory where output SVG files should be saved",
    )
    parser.add_argument(
        "--allowed-headers",
        action="append",
        default=[],
        help="Additional header filenames to keep during preprocessing parsing",
    )
    args = parser.parse_args()

    # Consolidate allowed headers
    allowed_headers = ["keymap.c", "keymap.h"]
    for entry in args.allowed_headers:
        for item in entry.split(","):
            item = item.strip()
            if item and item not in allowed_headers:
                allowed_headers.append(item)

    local_src = os.environ.get("LOCAL_QMK_OVERLAY", "/local-qmk")
    if not os.path.isdir(local_src):
        local_src = "."

    config_path = os.path.join(local_src, "keymaps.json")
    if not os.path.exists(config_path):
        print(f"Error: Config file not found at {config_path}", file=sys.stderr)
        sys.exit(1)

    with open(config_path, "r", encoding="utf-8") as f:
        keymaps = json.load(f)

    # Sync QMK overlay
    script_dir = os.path.dirname(os.path.abspath(__file__))
    sync_bin = (
        "/root/.local/bin/qmk-sync"
        if os.path.exists("/root/.local/bin/qmk-sync")
        else os.path.join(script_dir, "qmk-sync")
    )
    print("Syncing QMK overlay before drawing...")
    subprocess.run([sync_bin], check=True)

    qmk_tree = os.environ.get("QMK_TREE", "/keeb/qmk_firmware")

    # Configure qmk CLI home
    subprocess.run(["qmk", "config", f"user.qmk_home={qmk_tree}"], check=False)

    for item in keymaps:
        if not item.get("draw"):
            continue

        kb = item["keyboard"]
        km = item["keymap"]
        svg_rel = item.get("svg")
        if not svg_rel:
            print(
                f"Warning: No svg target defined for {kb}:{km}, skipping",
                file=sys.stderr,
            )
            continue

        # Target output file path determination
        if args.output:
            svg_out = os.path.join(args.output, os.path.basename(svg_rel))
        else:
            svg_out = svg_rel

        os.makedirs(os.path.dirname(svg_out) or ".", exist_ok=True)

        # Determine layer names (using preprocessed gcc -E parsing)
        layer_names = item.get("layer_names")
        if not layer_names:
            layer_names = extract_layer_names(
                local_src, qmk_tree, kb, km, allowed_headers
            )

        if layer_names:
            print(f"Using layer names for {kb}:{km}: {', '.join(layer_names)}")
        else:
            print(f"No explicit/parsed layer names for {kb}:{km}")

        print(f"Drawing SVG keymap for {kb}:{km} -> {svg_out}...")

        # Step 1: qmk c2json
        c2json = subprocess.run(
            ["qmk", "c2json", "-kb", kb, "-km", km, "--no-cpp"],
            cwd=qmk_tree,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        if c2json.returncode != 0:
            print(
                f"qmk c2json failed for {kb}:{km}:\n{c2json.stderr}",
                file=sys.stderr,
            )
            sys.exit(c2json.returncode)

        # Step 2: keymap parse -q - [--layer-names ...]
        parse_cmd = ["keymap", "parse", "-q", "-"]
        if layer_names:
            parse_cmd.extend(["--layer-names"] + layer_names)

        parse = subprocess.run(
            parse_cmd,
            input=c2json.stdout,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        if parse.returncode != 0:
            print(
                f"keymap parse failed for {kb}:{km}:\n{parse.stderr}",
                file=sys.stderr,
            )
            sys.exit(parse.returncode)

        # Step 3: keymap draw
        draw = subprocess.run(
            ["keymap", "draw", "-"],
            input=parse.stdout,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        if draw.returncode != 0:
            print(
                f"keymap draw failed for {kb}:{km}:\n{draw.stderr}",
                file=sys.stderr,
            )
            sys.exit(draw.returncode)

        with open(svg_out, "w", encoding="utf-8") as f_out:
            f_out.write(draw.stdout)

        print(f"Successfully generated {svg_out}")

    print("All keymap SVG drawings complete.")


if __name__ == "__main__":
    main()
