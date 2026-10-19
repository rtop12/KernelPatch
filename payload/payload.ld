ENTRY(payload_main)
SECTIONS
{
    . = 0;
    .text    : { *(.text*) }
    .rodata  : { *(.rodata*) }
    .patchme : { *(.patchme) }
    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
