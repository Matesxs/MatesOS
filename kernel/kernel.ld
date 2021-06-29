OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
  _virtual_base = 0xffffffff80000000;
	. = _virtual_base + 2M;

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