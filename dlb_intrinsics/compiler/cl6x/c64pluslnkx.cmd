
/* 
    Memory regions.
    This command file uses the memory layout of C6424 by default.
    If executing on hardware(non C6424) and not simulator, please
    adjust the below definitions to match your target
    memory map.
    See the manual of your target chip for memory mapping details.
*/

--args 1024
-c
-stack 0x60000
-heap 0x80000
-m dlb_ti_c6x.map


MEMORY
{
    L2RAM:      o = 0x10800000  l = 0x00010000
    DDR2:       o = 0x80000000  l = 0x10000000
}

SECTIONS
{
    .args       >   DDR2
    .bss        >   DDR2 
    .cinit      >   DDR2
    .cio        >   DDR2
    .const      >   DDR2
    .data       >   DDR2
    .far        >   DDR2
    .stack      >   DDR2
    .switch     >   DDR2
    .sysmem     >   DDR2
    .text       >   DDR2
    
    .ddr2       >   DDR2
}

