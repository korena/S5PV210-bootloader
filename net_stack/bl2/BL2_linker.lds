OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)
SECTIONS
{
 . = 0xD0020FB0;
 . = ALIGN(4);
        .text : {
	*/bl2_asm.o
        *(.text)
        }
 . = ALIGN(4);
        rodata = .;
        .rodata : { *(.rodata) }

. = ALIGN(4);
        data_start = .;
        .data : { *(.data) }

. = ALIGN(4);
        __bss_start__ = .;
        .bss : { *(.bss) }
        __bss_end__ = .;    
	end = .;
}

