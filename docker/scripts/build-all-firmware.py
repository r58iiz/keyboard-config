#!/usr/bin/env python3
import json
import os
import subprocess
import sys


def main():
    local_src = os.environ.get("LOCAL_QMK_OVERLAY", "/local-qmk")
    if not os.path.isdir(local_src):
        local_src = "."

    config_path = os.path.join(local_src, "keymaps.json")
    if not os.path.exists(config_path):
        print(f"Error: Config file not found at {config_path}", file=sys.stderr)
        sys.exit(1)

    with open(config_path, "r", encoding="utf-8") as f:
        keymaps = json.load(f)

    # 1. Handle vial_convert steps
    for item in keymaps:
        vial_cfg = item.get("vial_convert")
        if vial_cfg:
            kb = item["keyboard"]
            km = item["keymap"]
            target_km = vial_cfg["target_keymap"]
            allowed_headers = vial_cfg.get("allowed_headers", [])

            qmk_tree = os.environ.get("QMK_TREE", "/keeb/qmk_firmware")
            out_vil = os.path.join(
                local_src,
                "keyboards",
                kb,
                "keymaps",
                target_km,
                f"{kb.replace('/', '_')}-{target_km}.vil",
            )
            out_keymap = os.path.join(
                local_src, "keyboards", kb, "keymaps", target_km, "keymap.c"
            )

            script_dir = os.path.dirname(os.path.abspath(__file__))
            qmk2vial_bin = (
                "/root/.local/bin/qmk2vial.py"
                if os.path.exists("/root/.local/bin/qmk2vial.py")
                else os.path.join(script_dir, "qmk2vial.py")
            )

            cmd = [
                sys.executable,
                qmk2vial_bin,
                qmk_tree,
                kb,
                km,
                "--out-vil",
                out_vil,
                "--out-keymap",
                out_keymap,
            ]
            if allowed_headers:
                cmd.extend(["--allowed-headers"] + allowed_headers)

            print(f"Running Vial conversion for {kb}:{km} -> {target_km}...")
            res = subprocess.run(cmd)
            if res.returncode != 0:
                print(f"Vial conversion failed for {kb}:{km}", file=sys.stderr)
                sys.exit(res.returncode)

    # 2. Sync QMK overlay
    print("Syncing QMK trees...")
    script_dir = os.path.dirname(os.path.abspath(__file__))
    sync_bin = (
        "/root/.local/bin/qmk-sync"
        if os.path.exists("/root/.local/bin/qmk-sync")
        else os.path.join(script_dir, "qmk-sync")
    )
    res = subprocess.run([sync_bin])
    if res.returncode != 0:
        print("qmk-sync failed", file=sys.stderr)
        sys.exit(res.returncode)

    # 3. Build firmwares
    qmk_c_bin = (
        "/root/.local/bin/qmk-c"
        if os.path.exists("/root/.local/bin/qmk-c")
        else os.path.join(script_dir, "qmk-c")
    )
    for item in keymaps:
        kb = item["keyboard"]
        km = item["keymap"]
        tree_type = item.get("type", "qmk")
        print(f"Compiling firmware for {tree_type} {kb}:{km}...")
        res = subprocess.run([qmk_c_bin, tree_type, kb, km])
        if res.returncode != 0:
            print(
                f"Firmware compilation failed for {tree_type} {kb}:{km}",
                file=sys.stderr,
            )
            sys.exit(res.returncode)

    print("All firmwares compiled successfully.")


if __name__ == "__main__":
    main()
