OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_bl2_entry)
SECTIONS
{
 . = 0xD0020FB0;
 . = ALIGN(4);
        .text : {
        */BL2.bin
        *(.text)
        }
 . = ALIGN(4);
        rodata = .;
        .rodata : { *(.rodata) }

. = ALIGN(4);
        data_start = .;
        .data : { *(.data) }

. = ALIGN(4);
        bss_start = .;
        .bss : { *(.bss) }
        bss_end = .;    
}

