/* Entry Point */
ENTRY(main)

MEMORY
{
  RAM    (xrw)    : ORIGIN = 0x00000000,   LENGTH = 1024K
  FLASH    (rx)    : ORIGIN = 0x10000000,   LENGTH = 1024K
}

/* Sections */
SECTIONS
{
  .text :
  {
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
  } >FLASH

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    _sdata = .;        /* create a global symbol at data start */
    *(.nav_dev_tree)
  } >RAM AT> FLASH
}
