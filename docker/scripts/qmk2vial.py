#!/usr/bin/env python3
from __future__ import annotations
import argparse
import json
from pathlib import Path
import re
import shlex
import subprocess
import sys

# --------------------------------------------------------------------------
# Tunables you may need to adjust for your own userspace
# --------------------------------------------------------------------------

CUSTOM_KEYCODE_BASE = 0x7E40
RESERVED_CUSTOM_KEYCODE_SLOTS = 0
EXTRA_CUSTOM_KEYCODES: dict[str, int] = {}

# --------------------------------------------------------------------------
# Bracket-aware helpers (works for any mix of (), {}, [])
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
# Keycode renaming
# --------------------------------------------------------------------------

KC_ALIASES = {
    "KC_GRV": "KC_GRAVE",
    "KC_GRAVE": "KC_GRAVE",
    "KC_MINS": "KC_MINUS",
    "KC_MINUS": "KC_MINUS",
    "KC_EQL": "KC_EQUAL",
    "KC_EQUAL": "KC_EQUAL",
    "KC_LBRC": "KC_LBRACKET",
    "KC_LEFT_BRACKET": "KC_LBRACKET",
    "KC_RBRC": "KC_RBRACKET",
    "KC_RIGHT_BRACKET": "KC_RBRACKET",
    "KC_BSLS": "KC_BSLASH",
    "KC_BACKSLASH": "KC_BSLASH",
    "KC_SCLN": "KC_SCOLON",
    "KC_SEMICOLON": "KC_SCOLON",
    "KC_QUOT": "KC_QUOTE",
    "KC_QUOTE": "KC_QUOTE",
    "KC_COMM": "KC_COMMA",
    "KC_COMMA": "KC_COMMA",
    "KC_SLSH": "KC_SLASH",
    "KC_SLASH": "KC_SLASH",
    "KC_DOT": "KC_DOT",
    "KC_LCTL": "KC_LCTRL",
    "KC_LEFT_CTRL": "KC_LCTRL",
    "KC_RCTL": "KC_RCTRL",
    "KC_RIGHT_CTRL": "KC_RCTRL",
    "KC_LSFT": "KC_LSHIFT",
    "KC_LEFT_SHIFT": "KC_LSHIFT",
    "KC_RSFT": "KC_RSHIFT",
    "KC_RIGHT_SHIFT": "KC_RSHIFT",
    "KC_LALT": "KC_LALT",
    "KC_LEFT_ALT": "KC_LALT",
    "KC_RALT": "KC_RALT",
    "KC_ALGR": "KC_RALT",
    "KC_RIGHT_ALT": "KC_RALT",
    "KC_LGUI": "KC_LGUI",
    "KC_LEFT_GUI": "KC_LGUI",
    "KC_RGUI": "KC_RGUI",
    "KC_RIGHT_GUI": "KC_RGUI",
    "KC_ESC": "KC_ESCAPE",
    "KC_ESCAPE": "KC_ESCAPE",
    "KC_ENT": "KC_ENTER",
    "KC_ENTER": "KC_ENTER",
    "KC_BSPC": "KC_BSPACE",
    "KC_BSPACE": "KC_BSPACE",
    "KC_DEL": "KC_DELETE",
    "KC_DELETE": "KC_DELETE",
    "KC_INS": "KC_INSERT",
    "KC_INSERT": "KC_INSERT",
    "KC_SPC": "KC_SPACE",
    "KC_SPACE": "KC_SPACE",
    "KC_APP": "KC_APPLICATION",
    "KC_APPLICATION": "KC_APPLICATION",
    "KC_RGHT": "KC_RIGHT",
    "KC_RIGHT": "KC_RIGHT",
    "KC_PGUP": "KC_PGUP",
    "KC_PGDN": "KC_PGDOWN",
    "KC_PGDOWN": "KC_PGDOWN",
    "KC_HOME": "KC_HOME",
    "KC_END": "KC_END",
    "KC_CAPS": "KC_CAPSLOCK",
    "KC_CAPSLOCK": "KC_CAPSLOCK",
    "KC_PSCR": "KC_PSCREEN",
    "KC_PSCREEN": "KC_PSCREEN",
    "KC_SCRL": "KC_SCROLLLOCK",
    "KC_SCROLLLOCK": "KC_SCROLLLOCK",
    "KC_PAUS": "KC_PAUSE",
    "KC_PAUSE": "KC_PAUSE",
    "KC_WWW_FORWARD": "KC_WFWD",
    "KC_WFWD": "KC_WFWD",
    "KC_WWW_BACK": "KC_WBAK",
    "KC_WBAK": "KC_WBAK",
    "MS_BTN1": "KC_BTN1",
    "MS_BTN2": "KC_BTN2",
    "MS_BTN3": "KC_BTN3",
    "MS_BTN4": "KC_BTN4",
    "MS_BTN5": "KC_BTN5",
    "MS_UP": "KC_MS_U",
    "MS_DOWN": "KC_MS_D",
    "MS_LEFT": "KC_MS_L",
    "MS_RGHT": "KC_MS_R",
    "MS_WHLU": "KC_WH_U",
    "MS_WHLD": "KC_WH_D",
    "MS_WHLL": "KC_WH_L",
    "MS_WHLR": "KC_WH_R",
    "CW_TOGG": "QK_CAPS_WORD_TOGGLE",
    "XXXXXXX": "KC_NO",
    "_______": "KC_TRNS",
    "KC_TRNS": "KC_TRNS",
    "KC_NO": "KC_NO",
}

MOD_TAP_PREFIX = {
    "MOD_LCTL": "LCTL_T",
    "MOD_LSFT": "LSFT_T",
    "MOD_LALT": "LALT_T",
    "MOD_LGUI": "LGUI_T",
    "MOD_RCTL": "RCTL_T",
    "MOD_RSFT": "RSFT_T",
    "MOD_RALT": "RALT_T",
    "MOD_RGUI": "RGUI_T",
}

MOD_BIT_NAMES = {
    0x0100: "LCTL",
    0x0200: "LSFT",
    0x0400: "LALT",
    0x0800: "LGUI",
    0x1100: "RCTL",
    0x1200: "RSFT",
    0x1400: "RALT",
    0x1800: "RGUI",
}
MOD_COMBO_NAMES = {
    0x0300: "C_S",
    0x0500: "LCA",
    0x0700: "MEH",
    0x0900: "SGUI",
    0x0A00: "SGUI",
    0x0D00: "LCAG",
    0x0F00: "HYPR",
}


def rename_kc(name: str) -> str:
    return KC_ALIASES.get(name, name)


class Converter:
    def __init__(self, src: str):
        self.src = src
        self.layer_names = self._find_layer_names()
        self.layer_index = {n: i for i, n in enumerate(self.layer_names)}
        self.td_names = self._find_td_names()
        self.td_index = {n: i for i, n in enumerate(self.td_names)}
        self.local_custom_names = self._find_custom_keycode_enum(src)
        self.custom_map: dict[str, int] = {}
        self._build_custom_keycode_map()

    def _parse_config_defines(
        self, config_paths: list[Path]
    ) -> dict[str, str | int | bool]:
        """Reads config.h files and extracts #define settings."""
        defines: dict[str, str | int | bool] = {}

        # Regex for #define NAME value OR #define NAME
        define_re = re.compile(
            r"^\s*#\s*define\s+([A-Za-z_]\w*)(?:\s+(.*?))?\s*$", re.MULTILINE
        )

        for path in config_paths:
            if not path.exists():
                continue

            content = path.read_text()
            for name, val in define_re.findall(content):
                val = val.strip() if val else None

                # Clean up inline C comments (e.g., "0 // default" -> "0")
                if val and "//" in val:
                    val = val.split("//")[0].strip()
                if val and "/*" in val:
                    val = val.split("/*")[0].strip()

                if val is None:
                    # Parameterless flags like `#define PERMISSIVE_HOLD`
                    defines[name] = True
                else:
                    # Value defines like `#define TAPPING_TERM 160`
                    try:
                        defines[name] = int(val, 0)
                    except ValueError:
                        defines[name] = val

        return defines

    def _find_layer_names(self) -> list[str]:
        """Infer layer names dynamically based on the keymaps array entries."""
        try:
            body = extract_array_body(
                self.src, r"\bkeymaps\s*\[\s*\]\s*\[[^\]]*\]\s*\[[^\]]*\]\s*="
            )
            entries = extract_designated_entries(body)
            return [name for name, _ in entries]
        except ValueError:
            return []

    def _find_td_names(self) -> list[str]:
        """Infer tap dance enum names dynamically based on tap_dance_actions."""
        try:
            body = extract_array_body(self.src, r"\btap_dance_actions\s*\[\s*\]\s*=")
            entries = extract_designated_entries(body)
            return [name for name, _ in entries]
        except ValueError:
            return []

    def _find_custom_keycode_enum(self, src: str) -> list[str]:
        """Look for any enum containing SAFE_RANGE or custom_keycodes."""
        names: list[str] = []
        for m in re.finditer(r"enum\s+(?:\w+\s*)?\{", src):
            brace_start = m.end() - 1
            brace_end = find_matching(src, brace_start)
            body = src[brace_start + 1 : brace_end]
            if "SAFE_RANGE" in body or "custom_keycode" in body.lower():
                for item in split_top_level(body):
                    name = item.split("=")[0].strip()
                    if name and name != "SAFE_RANGE":
                        names.append(name)
        return names

    def _find_unknown_identifiers_in_arrays(self) -> list[str]:
        bodies = []
        try:
            bodies.append(
                extract_array_body(
                    self.src, r"\bkeymaps\s*\[\s*\]\s*\[[^\]]*\]\s*\[[^\]]*\]\s*="
                )
            )
        except ValueError:
            pass
        try:
            bodies.append(
                extract_array_body(
                    self.src,
                    r"\bencoder_map\s*\[\s*\]\s*\[.*?\]\s*\[[^\]]*\]\s*=",
                )
            )
        except ValueError:
            pass

        known = set(KC_ALIASES) | set(KC_ALIASES.values())
        known |= set(self.layer_names) | set(self.td_names)
        known |= set(self.local_custom_names)
        known |= {
            "QK_MOD_TAP",
            "QK_LAYER_TAP",
            "QK_DEF_LAYER",
            "QK_TAP_DANCE",
        } | set(MOD_TAP_PREFIX)

        SAFE_PREFIXES = ("KC_", "MS_", "RM_", "QK_", "MOD_", "DYN_", "OSM", "OSL")

        found = []
        seen = set()
        ident_re = re.compile(r"\b[A-Z][A-Z0-9_]*\b")
        for body in bodies:
            for tok in ident_re.findall(body):
                if tok in known or tok in seen:
                    continue
                if tok.startswith("0X") or re.fullmatch(r"0X[0-9A-F]+", tok):
                    continue
                if tok.startswith(SAFE_PREFIXES):
                    continue
                seen.add(tok)
                found.append(tok)
        return found

    def _build_custom_keycode_map(self):
        code = CUSTOM_KEYCODE_BASE
        for name, val in EXTRA_CUSTOM_KEYCODES.items():
            self.custom_map[name] = val

        for name in self._find_unknown_identifiers_in_arrays():
            if name in self.custom_map:
                continue
            self.custom_map[name] = code
            code += 1

        code += RESERVED_CUSTOM_KEYCODE_SLOTS

        for name in self.local_custom_names:
            if name in self.custom_map:
                continue
            self.custom_map[name] = code
            code += 1

    _MOD_TAP_RE = re.compile(
        r"^\(?\s*QK_MOD_TAP\s*\|\s*\(+\s*(MOD_\w+)\s*\)+\s*&\s*0x1F\s*\)*\s*<<\s*8\s*\)+\s*\|\s*\(+\s*(\w+)\s*\)+\s*&\s*0xFF\s*\)+\s*$"
    )
    _LAYER_TAP_RE = re.compile(
        r"^\(?\s*QK_LAYER_TAP\s*\|\s*\(+\s*(\w+)\s*\)+\s*&\s*0xF\s*\)*\s*<<\s*8\s*\)+\s*\|\s*\(+\s*(\w+)\s*\)+\s*&\s*0xFF\s*\)+\s*$"
    )
    _DEF_LAYER_RE = re.compile(
        r"^\(?\s*QK_DEF_LAYER\s*\|\s*\(+\s*(\w+)\s*\)+\s*&\s*0x1F\s*\)+\s*$"
    )
    _TAP_DANCE_RE = re.compile(
        r"^\(?\s*QK_TAP_DANCE\s*\|\s*\(+\s*(\w+)\s*\)+\s*&\s*0xFF\s*\)+\s*$"
    )
    _MOD_BITS_RE = re.compile(r"^\(?\s*(0[xX][0-9A-Fa-f]+)\s*\|\s*(.+)$", re.S)

    def _layer_num(self, name: str) -> int:
        if name in self.layer_index:
            return self.layer_index[name]
        try:
            return int(name, 0)
        except ValueError:
            raise ValueError(f"unknown layer name: {name}")

    def _td_num(self, name: str) -> int:
        if name in self.td_index:
            return self.td_index[name]
        try:
            return int(name, 0)
        except ValueError:
            raise ValueError(f"unknown tap-dance name: {name}")

    def convert_mod_bits(self, expr: str) -> str:
        bits = 0
        cur = expr.strip()
        while True:
            cur = cur.strip()
            while cur.startswith("(") and cur.endswith(")"):
                if find_matching(cur, 0) == len(cur) - 1:
                    cur = cur[1:-1].strip()
                else:
                    break
            m = self._MOD_BITS_RE.match(cur)
            if not m:
                break
            bits |= int(m.group(1), 16)
            cur = m.group(2).strip()
            while (
                cur.startswith("(")
                and cur.endswith(")")
                and find_matching(cur, 0) == len(cur) - 1
            ):
                cur = cur[1:-1].strip()
        terminal = rename_kc(cur)
        if bits in MOD_COMBO_NAMES:
            return f"{MOD_COMBO_NAMES[bits]}({terminal})"
        names = []
        for bit, nm in sorted(MOD_BIT_NAMES.items()):
            if bits & bit:
                names.append(nm)
                bits &= ~bit
        result = terminal
        for nm in reversed(names):
            result = f"{nm}({result})"
        return result

    def convert(self, raw: str) -> str:
        expr = " ".join(raw.split())

        if re.fullmatch(r"\(\s*[\w]+\s*\)", expr):
            expr = expr[1:-1].strip()

        m = self._MOD_TAP_RE.match(expr)
        if m:
            mod, kc = m.groups()
            prefix = MOD_TAP_PREFIX.get(mod)
            if prefix:
                return f"{prefix}({rename_kc(kc)})"

        m = self._LAYER_TAP_RE.match(expr)
        if m:
            layer, kc = m.groups()
            return f"LT{self._layer_num(layer)}({rename_kc(kc)})"

        m = self._DEF_LAYER_RE.match(expr)
        if m:
            (layer,) = m.groups()
            return f"DF({self._layer_num(layer)})"

        m = self._TAP_DANCE_RE.match(expr)
        if m:
            (idx,) = m.groups()
            return f"TD({self._td_num(idx)})"

        if re.match(r"^\(?\s*0[xX][0-9A-Fa-f]+\s*\|", expr):
            return self.convert_mod_bits(expr)

        ident = expr.strip()
        if re.fullmatch(r"[A-Za-z_]\w*", ident):
            if ident in self.custom_map:
                return f"0x{self.custom_map[ident]:02x}"
            return rename_kc(ident)

        return expr

    def get_settings(self, config_paths: list[Path] | None = None) -> dict[str, int]:
        """Dynamically populates Vial settings by parsing config.h defines with fallback defaults."""
        defines = self._parse_config_defines(config_paths or [])

        # --- Tap-Hold Settings ---
        quick_tap = defines.get("QUICK_TAP_TERM", 0)
        tapping_toggle = defines.get("TAPPING_TOGGLE", 2)
        tapping_term = defines.get("TAPPING_TERM", 160)
        retro_tapping = 1 if "RETRO_TAPPING" in defines else 0
        hold_on_other = 1 if "HOLD_ON_OTHER_KEY_PRESS" in defines else 0
        permissive_hold = 1 if "PERMISSIVE_HOLD" in defines else 0
        chordal_hold = 1 if "CHORDAL_HOLD" in defines else 0

        # Fallback to get_global_tapping_term if TAPPING_TERM macro is absent
        if "TAPPING_TERM" not in defines:
            tapping_term = self.get_global_tapping_term(default_val=160)

        # --- Mouse Key Settings ---
        mouse_delay = defines.get("MOUSEKEY_DELAY", 10)
        mouse_interval = defines.get("MOUSEKEY_INTERVAL", 16)
        mouse_move_delta = defines.get("MOUSEKEY_MOVE_DELTA", 8)
        mouse_max_speed = defines.get("MOUSEKEY_MAX_SPEED", 6)
        mouse_time_to_max = defines.get("MOUSEKEY_TIME_TO_MAX", 64)
        mouse_wheel_delay = defines.get("MOUSEKEY_WHEEL_DELAY", 80)

        return {
            "1": int(quick_tap),
            "2": 50,
            "3": 0,
            "4": int(tapping_toggle),
            "5": 5,
            "6": 5000,
            "7": int(tapping_term),
            "9": 0,
            "10": int(mouse_interval),
            "11": int(mouse_delay) // 10 if isinstance(mouse_delay, int) else 1,
            "12": int(mouse_max_speed),
            "13": int(mouse_time_to_max),
            "14": retro_tapping,
            "15": int(mouse_wheel_delay),
            "16": int(mouse_move_delta),
            "17": 40,
            "18": 0,
            "19": 80,
            "20": hold_on_other,
            "21": 0,
            "22": permissive_hold,
            "23": 0,
            "24": 0,
            "25": 160,
            "26": chordal_hold,
            "27": 0,
        }

    def get_keymaps(self) -> dict[str, list[list[str]]]:
        body = extract_array_body(
            self.src, r"\bkeymaps\s*\[\s*\]\s*\[[^\]]*\]\s*\[[^\]]*\]\s*="
        )
        layers = extract_designated_entries(body)
        out = {}
        for name, rows_body in layers:
            rows = []
            for row in split_top_level(rows_body):
                if row.startswith("{"):
                    row = row[1 : find_matching(row, 0)]
                cells = split_top_level(row)
                rows.append([self.convert(c) for c in cells])
            out[name] = rows
        return out

    def get_encoder_map(self) -> dict[str, list[list[str]]]:
        body = extract_array_body(
            self.src, r"\bencoder_map\s*\[\s*\]\s*\[.*?\]\s*\[[^\]]*\]\s*="
        )
        layers = extract_designated_entries(body)
        out = {}
        for name, pairs_body in layers:
            pairs = []
            for pair in split_top_level(pairs_body):
                if pair.startswith("{"):
                    pair = pair[1 : find_matching(pair, 0)]
                cells = split_top_level(pair)
                pairs.append([self.convert(c) for c in cells])
            out[name] = pairs
        return out

    def get_tap_dance(
        self, total_slots: int = 32, default_term: int = 160
    ) -> list[list]:
        try:
            body = extract_array_body(self.src, r"\btap_dance_actions\s*\[\s*\]\s*=")
            entries = extract_designated_entries(body)
        except ValueError:
            return [
                ["KC_NO", "KC_NO", "KC_NO", "KC_NO", default_term]
                for _ in range(total_slots)
            ]

        fn_re = re.compile(r"\.fn\s*=\s*\{\s*[^,]*,\s*([A-Za-z_]\w*)\s*,", re.S)
        results: dict[int, list] = {}
        for name, inner in entries:
            idx = self._td_num(name)
            m = fn_re.search(inner)
            action = "KC_NO"
            if m:
                fn_name = m.group(1)
                action = self._tap_dance_action_from_fn(fn_name)
            results[idx] = ["KC_NO", "KC_NO", action, "KC_NO", default_term]

        out = []
        for i in range(total_slots):
            out.append(
                results.get(i, ["KC_NO", "KC_NO", "KC_NO", "KC_NO", default_term])
            )
        return out

    def get_global_tapping_term(self, default_val: int = 160) -> int:
        """Parses get_tapping_term in C code to find the baseline/default return value."""
        # Search for the function body of get_tapping_term
        m = re.search(r"\bget_tapping_term\s*\([^)]*\)\s*\{", self.src)
        if not m:
            return default_val

        try:
            brace_start = m.end() - 1
            brace_end = find_matching(self.src, brace_start)
            body = self.src[brace_start + 1 : brace_end]

            # Match `default: return 160;` or a top-level `return TAPPING_TERM;` / `return 160;`
            # 1. First look for an explicit `default:` branch
            default_match = re.search(
                r"\bdefault\s*:\s*return\s+([A-Za-z0-9_]+)\s*;", body
            )
            if default_match:
                val_str = default_match.group(1)
                return (
                    int(val_str, 0)
                    if val_str.isdigit() or val_str.startswith("0x")
                    else default_val
                )

            # 2. Otherwise look for the last `return <number>;` in the function
            returns = re.findall(r"\breturn\s+(0x[0-9A-Fa-f]+|\d+)\s*;", body)
            if returns:
                return int(returns[-1], 0)

        except (ValueError, IndexError):
            pass

        return default_val

    def _tap_dance_action_from_fn(self, fn_name: str) -> str:
        m = re.search(
            re.escape(fn_name) + r"\s*\([^)]*\)\s*\{(.*?)\n\}",
            self.src,
            re.S,
        )
        if not m:
            return "KC_NO"
        body = m.group(1)
        if "reset_keyboard" in body:
            return "QK_BOOT"
        dm = re.search(r"default_layer_set\s*\([^)]*\)\s*1\s*<<\s*(\w+)\s*\)", body)
        if dm:
            return f"DF({self._layer_num(dm.group(1))})"
        return "KC_NO"

    def get_macro(self, count: int = 16) -> list[list]:
        return [[] for _ in range(count)]

    def get_combos(self, count: int = 32) -> list[list[str]]:
        """Generates default empty Vial combo array."""
        return [["KC_NO"] * 5 for _ in range(count)]

    def get_key_overrides(self, count: int = 32) -> list[dict]:
        """Generates default empty Vial key_override array."""
        return [
            {
                "trigger": "KC_NO",
                "replacement": "KC_NO",
                "layers": 65535,
                "trigger_mods": 0,
                "negative_mod_mask": 0,
                "suppressed_mods": 0,
                "options": 7,
            }
            for _ in range(count)
        ]

    def get_alt_repeat_keys(self, count: int = 32) -> list[dict]:
        """Generates default empty Vial alt_repeat_key array."""
        return [
            {
                "keycode": "KC_NO",
                "alt_keycode": "KC_NO",
                "allowed_mods": 0,
                "options": 0,
            }
            for _ in range(count)
        ]

    def build(self, uid: int = 0, config_paths: list[Path] | None = None) -> dict:
        keymaps = self.get_keymaps()
        try:
            encoders = self.get_encoder_map()
        except ValueError:
            encoders = {}

        layout = [keymaps[name] for name in self.layer_names if name in keymaps]
        encoder_layout = [
            encoders[name] for name in self.layer_names if name in encoders
        ]

        return {
            "version": 1,
            "uid": uid,  # Keyboard UID
            "layout": layout,
            "encoder_layout": encoder_layout,
            "layout_options": -1,
            "macro": self.get_macro(),
            "vial_protocol": 6,
            "via_protocol": 9,
            "tap_dance": self.get_tap_dance(),
            "combo": self.get_combos(),
            "key_override": self.get_key_overrides(),
            "alt_repeat_key": self.get_alt_repeat_keys(),
            "settings": self.get_settings(config_paths),
        }


def parse_preprocessed(path: Path, keep_files: list[str] | None = None) -> str:
    """Filters preprocessed gcc output to only keep lines originating from specified files/headers."""
    if not keep_files:
        keep_files = ["keymap.c", "keymap.h"]

    with path.open() as f:
        content = f.readlines()

    keep = False
    out = []

    for line in content:
        if line.startswith("# "):
            parts = line.split('"')
            if len(parts) > 1:
                filename = parts[1]
                # Keep lines originating from any allowed header/file name
                keep = any(filename.endswith(fname) for fname in keep_files)
            continue
        if keep:
            out.append(line)

    return "".join(out)


def strip_vial_incompatible(src: str) -> str:
    """Removes C structure/array blocks and functions from keymap.c that cause issues for Vial."""
    TARGET_PATTERNS = [
        r"\btap_dance_actions\s*\[\s*\]\s*=",
        r"\bget_tapping_term\s*\([^)]*\)\s*\{",
    ]

    for pattern in TARGET_PATTERNS:
        while True:
            m = re.search(pattern, src)
            if not m:
                break

            try:
                # Find the opening brace '{'
                if src[m.end() - 1] == "{":
                    brace_start = m.end() - 1
                else:
                    brace_start = src.index("{", m.end())

                brace_end = find_matching(src, brace_start)

                # Functions end at '}'; Arrays end at ';'
                if pattern.rstrip().endswith("{"):
                    block_end = brace_end
                    block_name = "get_tapping_term function"
                else:
                    block_end = src.index(";", brace_end)
                    block_name = m.group(0).split("[")[0].strip()

                line_start = src.rfind("\n", 0, m.start())
                if line_start == -1:
                    line_start = 0

                comment = (
                    f"\n// [qmk2vial] Removed {block_name} for Vial compatibility\n"
                )

                src = src[:line_start] + comment + src[block_end + 1 :]

            except ValueError:
                break

    return src


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Compile and export QMK keymap to Vial JSON format"
    )
    parser.add_argument(
        "qmk_repo",
        help="QMK directory",
    )
    parser.add_argument(
        "keyboard",
        help="Keyboard name (e.g. mechboards/sofle/pro)",
    )
    parser.add_argument(
        "keymap",
        help="Keymap name (e.g. default)",
    )
    parser.add_argument(
        "--allowed-headers",
        action="append",
        default=[],
        help="Additional header filenames to keep during preprocessing parsing",
    )
    parser.add_argument(
        "--out-vil",
        type=Path,
        default=None,
        help="Path to output the resulting *.vil file",
    )
    parser.add_argument(
        "--out-keymap",
        type=Path,
        default=None,
        help="Optional path to output a modified keymap.c with Vial-incompatible elements stripped",
    )
    parser.add_argument("--pretty", action="store_true")
    parser.add_argument(
        "--custom-keycode",
        action="append",
        default=[],
        metavar="NAME=HEX",
        help="Manually pin a custom keycode's vial hex value, e.g. CK_OLTG=0x7e40",
    )

    args = parser.parse_args()

    for pair in args.custom_keycode:
        name, val = pair.split("=", 1)
        EXTRA_CUSTOM_KEYCODES[name.strip()] = int(val, 0)

    repo = Path(args.qmk_repo).resolve()
    keymap_file = (
        repo
        / Path("keyboards")
        / Path(args.keyboard)
        / Path("keymaps")
        / Path(args.keymap)
        / Path("keymap.c")
    )
    keyboard_dir = repo / "keyboards" / Path(args.keyboard)
    keymap_dir = keyboard_dir / "keymaps" / Path(args.keymap)
    config_paths = [
        keyboard_dir / "config.h",
        keymap_dir / "config.h",
    ]

    # Configure qmk_home
    cmd = ["qmk", "config", f'user.qmk_home="{args.qmk_repo}"']
    subprocess.run(cmd, cwd=repo, check=True, stdout=subprocess.DEVNULL)

    # Compile once to get cflags
    cmd = ["qmk", "compile", "-kb", args.keyboard, "-km", args.keymap]
    subprocess.run(cmd, cwd=repo, check=True, stdout=subprocess.DEVNULL)

    target = f"{args.keyboard.replace('/', '_')}_{args.keymap}"
    build_dir = repo / Path(".build") / f"obj_{target}"
    cflags = build_dir / "cflags.txt"

    if not cflags.exists():
        print(f"error: {cflags} not found", file=sys.stderr)
        return 1

    # Split cflags
    with open(cflags, "r") as f:
        cflags_text = f.read()
    cflags_args = shlex.split(cflags_text)

    # Filter cflags
    filtered_cflags_args = []
    it = iter(cflags_args)
    for arg in it:
        if arg == "-include":
            filtered_cflags_args.extend([arg, next(it)])
        elif arg.startswith(("-I", "-D")):
            filtered_cflags_args.append(arg)
        elif arg.startswith("-std"):
            filtered_cflags_args.append(arg)

    preprocessed = build_dir / "keymap.i"

    cmd = [
        "gcc",
        *filtered_cflags_args,
        "-E",
        "-o",
        preprocessed,
        keymap_file,
    ]
    subprocess.run(cmd, cwd=repo, check=True, stdout=subprocess.DEVNULL)

    vial_json_path = keyboard_dir / "vial.json"
    uid = 0
    if vial_json_path.exists():
        try:
            vial_data = json.loads(vial_json_path.read_text())
            uid = vial_data.get("uid", 0)
        except Exception:
            pass

    allowed_headers = ["keymap.c", "keymap.h"]
    for entry in args.allowed_headers:
        for item in entry.split(","):
            item = item.strip()
            if item and item not in allowed_headers:
                allowed_headers.append(item)

    src_text = parse_preprocessed(preprocessed)

    conv = Converter(src_text)
    result = conv.build(uid=uid, config_paths=config_paths)

    text = json.dumps(result, indent=2 if args.pretty else None)
    if args.out_vil:
        args.out_vil.write_text(text)
        print(f"Converted vil written to: {args.out_vil}", file=sys.stderr)
    else:
        print(text)

    # Strip and generate modified keymap.c if requested
    if args.out_keymap:
        original_src = keymap_file.read_text()
        cleaned_src = strip_vial_incompatible(original_src)
        args.out_keymap.write_text(cleaned_src)
        print(f"Cleaned keymap written to: {args.out_keymap}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
