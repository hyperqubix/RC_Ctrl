ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}

_estack = 0x20000000+0x2000; /* RAM + 8K(0x2000) = End of RAM */

SECTIONS
{
    .text : 
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        KEEP(*(.init*))
        KEEP(*(.fini*))
        . = ALIGN(4);
    } >FLASH

.ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } >FLASH
    
    _sidata = .;
          
    .data : 
    {
    	. = ALIGN(4);
    	_sdata = .;
        *(.data*)
        _edata = .;
    } >RAM AT>FLASH

    .bss :
    {
    	. = ALIGN(4);
    	_sbss = .;
        *(.bss)       /* Zero-filled run time allocate data memory */
    	_ebss = .;
    } >RAM
}  
