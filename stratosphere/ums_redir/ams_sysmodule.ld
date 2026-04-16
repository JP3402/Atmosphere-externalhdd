/* 
 * Linker Script Requirements for UMS Redirection Sysmodule
 *
 * 1. Entry Point: Must be _start.
 * 2. Section Placement:
 *    - .text: Code must reside in the Code memory region.
 *    - .rodata: Constant data.
 *    - .data / .bss: Global variables.
 * 3. Heap Mapping:
 *    - The 4MB heap buffer (g_malloc_buffer) must be page-aligned.
 *    - It should be placed in the .bss or a custom section to ensure it's part of the sysmodule's memory footprint.
 * 4. Relocations:
 *    - Must be PIE (Position Independent Executable) to support KASLR.
 */

OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5); /* R E */
    rodata PT_LOAD FLAGS(4); /* R */
    data PT_LOAD FLAGS(6); /* RW */
    dynamic PT_DYNAMIC;
}

SECTIONS
{
    . = 0;

    .text : {
        KEEP(*(.crt0))
        *(.text .text.*)
    } :text

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic
}
