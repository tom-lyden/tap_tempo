MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rw) : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS
{
    _word_align = 4;
    
    .text : 
    {
        . = ALIGN(_word_align);
        
        KEEP(*(.isr_vector))
        *(.text*)
        
        . = ALIGN(_word_align);
        
        _etext = .;
    } > FLASH
    .rodata :
    {
        _rodata = .;
        
        *(.rodata*)
        
        . = ALIGN(_word_align);
        
        _erodata = .;
    } > FLASH
    .data : 
    {
        _data = .;
        
        *(.data*)
        
        . = ALIGN(_word_align);
        
        _edata = .;
    } > SRAM AT > FLASH
    .bss :
    {
        _bss = .;
        
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(_word_align);
        
        _ebss = .;
    } > SRAM
    
    _static_data_end = .;
    _data_loadaddr = LOADADDR(.data);
    
    PROVIDE(_stack_size = 0x1000);
    _stack_top = ORIGIN(SRAM) + LENGTH(SRAM);
    _stack_bottom = _stack_top - _stack_size;
    
    ASSERT(_stack_bottom >= _static_data_end, "No room left for the stack")
}
