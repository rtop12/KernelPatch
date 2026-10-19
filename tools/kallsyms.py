#!/usr/bin/env python3
"""从 raw kernel Image 恢复符号表（占位，待实现）。"""


def find_kallsyms(image: bytes) -> dict:
    """返回 {name: vaddr}。实现见后续迭代。"""
    raise NotImplementedError("kallsyms recovery not implemented yet")
