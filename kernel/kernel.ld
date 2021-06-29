OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
  . = 0xffffffff80200000;

  _KernelStart = .;
  .stivale2hdr : {
    KEEP(*(.stivale2hdr))
  }
  
  .text : ALIGN(0x1000)
  {
    *(.text)
  }

  .data : ALIGN(0x1000)
  {
    *(.data)
  }

  .rodata : ALIGN(0x1000)
  {
    *(.rodata)
  }

  _WritableStart = .;
  .bss : ALIGN(0x1000)
  {
    *(COMMON)
    *(.bss)
  }
  _WritableEnd = .;
  _KernelEnd = .;
}