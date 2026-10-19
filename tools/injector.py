#!/usr/bin/env python3
"""内核二进制静态注入器（占位版）。"""
import argparse
import sys


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--kernel", required=True)
    ap.add_argument("--payload", required=True)
    ap.add_argument("--target", required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    print(f"[injector] kernel  = {args.kernel}")
    print(f"[injector] payload = {args.payload}")
    print(f"[injector] target  = {args.target}")
    print(f"[injector] out     = {args.out}")
    print("[injector] TODO: kallsyms / code cave / B patch")

    with open(args.kernel, "rb") as f:
        data = f.read()
    with open(args.out, "wb") as f:
        f.write(data)
    print("[injector] stub: copied kernel unchanged")
    return 0


if __name__ == "__main__":
    sys.exit(main())
