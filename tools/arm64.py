#!/usr/bin/env python3
"""ARM64 指令编码辅助。"""


def encode_b(pc: int, target: int) -> bytes:
    """编码 B 指令。pc 是这条指令自身的虚拟地址。"""
    offset = target - pc
    if offset % 4 != 0:
        raise ValueError(f"target not 4-byte aligned: {offset:#x}")
    if not (-0x8000000 <= offset < 0x8000000):
        raise ValueError(f"offset out of B range: {offset:#x}")
    imm26 = (offset >> 2) & 0x03FFFFFF
    insn = 0x14000000 | imm26
    return insn.to_bytes(4, "little")
