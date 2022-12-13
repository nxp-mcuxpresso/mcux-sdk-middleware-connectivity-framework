/*
** ###################################################################
**     Processors:          QN9030
**
**     Compiler:            GNU C Compiler
**     Reference manual:    QN9030 Series Reference Manual
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright  2019 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/


/******************* Map of QN9030 FLASH ***********************************


             0x000a_0000    m_flash_end
    - - - +---------------+ - - - - - - - -
          |   _ _ _ _ _   |
          | Flash config  |
   8.5k   |   - - - - -   |
          |               |
          |  RESERVED     |  0x9de00
    - - - +---------------+ - - - - - - - -
          |               |
          |     NOT       |
          |  AVAILABLE    |
          |               | 0x50000
    - - - +---------------+ - - - - - - - -
          |               | NV_STORAGE_START_ADDRESS
   31.5k  |               |
          |   NVM_region  |
          |               |
          |               | NV_STORAGE_END_ADDRESS
    - - - +---------------+ - - - - - - - -
          |               | INT_STORAGE_START
          |               |
          |               |
   MAX    | OTAP Internal |
   144k   |   Storage*    |
          |               |
          |               |
          |               |
          |               | INT_STORAGE_END (Must be aligned on a 8K to support the OTA remap mechanism)
    - - - +---------------+ - - - - - - - -
          |               | m_text_end
          |               |
          |               |
    MAX   |  Application  |
    144k  |               |
          |               |
          |               | m_text_start
    - - - +---------------+ - - - - - - - -
             0x0000_0000

 * - This section is optional for application that does not support OTA (gUseInternalStorageLink_d not defined or equal to 0).
In this case the application (m_text_end) could be up to NV_STORAGE_END_ADDRESS-1 and its MAX size will be 600k
 *****************************************************************************/

/* Entry Point */
ENTRY(ResetISR)

/*** flash memory characteristics ***/
m_flash_start   = 0x00000000;
m_flash_end     = 0x0004FFFF;
m_flash_size    = 0x00050000;
m_sector_size   = 512;

/****************************************************
 *******         User Defines                 *******
 ****************************************************/
UseNVMLink              = DEFINED(gUseNVMLink_d)            ? gUseNVMLink_d             : 0;
NVMSectorCountLink      = DEFINED(gNVMSectorCountLink_d)    ? gNVMSectorCountLink_d     : ( DEFINED(gUseNVMLink_d) ? gUseNVMLink_d*63 : 0 ); 
ram_vector_table        = DEFINED(__ram_vector_table__)     ? __ram_vector_table__      : 0;
UseInternalStorageLink  = DEFINED(gUseInternalStorageLink_d)? gUseInternalStorageLink_d : 0;

m_app_size  = DEFINED(__app_stated_size__) ? __app_stated_size__ : 0x48000;
m_app_start = DEFINED(__app_load_address__) ? __app_load_address__ : 0x0;

m_app_end                 = m_app_start + m_app_size - 1;


HEAP_SIZE               = DEFINED(__heap_size__)            ? __heap_size__             : 0;
STACK_SIZE              = DEFINED(__stack_size__)           ? __stack_size__            : ( DEFINED(__special_stack_size__) ? __special_stack_size__ : (0x1000 - 32));
vector_table_size       = 0x120;

/*
 * stack size for the boot rom during warm boot and application
 * 256 is sufficient (pwrm_test) but keep it large to 1024
 */
BOOT_RESUME_STACK_SIZE = 1024;


/****************************************************
 ******          Flash regions sizes           ******
 ****************************************************/
NV_STORAGE_SIZE         = NVMSectorCountLink * m_sector_size;
NV_STORAGE_MAX_SECTORS  = NVMSectorCountLink;
NV_STORAGE_SECTOR_SIZE = m_sector_size;

m_int_start_addr        = m_app_end + 1;

/****************************************************
 ******        Flash Regions, low to high      ******
 ****************************************************/
m_text_start            = m_app_start;



/****************************************************
 *****        Flash Regions, high to low       ******
 ****************************************************/
NV_STORAGE_START_ADDRESS = m_flash_size - 1;
NV_STORAGE_END_ADDRESS  = NV_STORAGE_START_ADDRESS - NV_STORAGE_SIZE + 1;
INT_STORAGE_START       = NV_STORAGE_END_ADDRESS - 1;
INT_STORAGE_END         = (UseInternalStorageLink) ? m_int_start_addr : INT_STORAGE_START;
INT_STORAGE_SIZE        = INT_STORAGE_START - INT_STORAGE_END;
m_text_end              = INT_STORAGE_END - 1;

/* Other Defines */
m_text_size             = m_text_end + 1 - m_text_start ;

INT_STORAGE_SECTOR_SIZE = m_sector_size;
M_VECTOR_RAM_SIZE       = DEFINED(__ram_vector_table__) ? vector_table_size : 0x0;

/* Specify the memory areas */
MEMORY
{
  /* Define each memory region */
  Flash320        (RX) : ORIGIN = m_app_start,     LENGTH = m_text_size

  SCRATCH_RAM(rwx)   : ORIGIN = 0x4000000, LENGTH = 0x400   /* 1K bytes (alias SCRATCH_RAM) */
  RAM0 (rwx)    : ORIGIN = 0x4000400, LENGTH = 0x15c00 /* 87K bytes (alias RAM) */
}

  /* Define a symbol for the top of each memory region */
  __base_Flash320 = 0x0  ; /* Flash320 */
  __base_Flash = 0x0 ; /* Flash */
  __top_Flash320 = 0x0 + 0x50000 ; /* 320K bytes */
  __top_Flash = 0x0 + 0x50000 ; /* 320K bytes */
  __base_unretainedRAM0 = 0x4000000  ; /* RAM0 bank : trampled at Warm boot, can be used as scratch area */
  __top_unretainedRAM0 =  0x4000400  ; /* start of normal use RAM0 */
  __base_RAM0 = 0x4000400  ; /* RAM0 */
  __base_RAM = 0x4000400 ; /* RAM */
  __top_RAM0 = 0x4000400 + 0x15c00 ; /* 87K bytes */
  __top_RAM = 0x4000400 + 0x15c00 ; /* 87K bytes */



SECTIONS
{
    /* MAIN TEXT SECTION */
    .m_interrupts : ALIGN(4)
    {
        _flash_start = ABSOLUTE(.);
        _flash_beg = ABSOLUTE(.);

        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        __VECTOR_TABLE = .;
        KEEP(*(.isr_vector))
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320

    /* MAIN TEXT SECTION */
    .data_bss : ALIGN(10)
    {
        /* Global Section Table */
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320

    .ro_nonce : ALIGN(0x10)
    {
        _FlsNonceStart = ABSOLUTE(.);
        *(.ro_nonce) /* nonce value is 16 bytes.*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320

    .ro_ota_header : ALIGN(0x10)
    {
        _enc_start = ABSOLUTE(.);
        _enc_offset = (_enc_start & 0x0000000F);
        _FlsOtaHeader = ABSOLUTE(.);
        *(.ro_ota_header) /* Ota Header 69 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320

    .ro_se_lnkKey (ALIGN((. - _enc_offset), 16) + _enc_offset):
    {
        _FlsLinkKey = ABSOLUTE(.);
        *(.ro_se_lnkKey)  /* Link Key 16 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320

    .filler :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(0x40);
    } > Flash320

    .text : ALIGN(0x40)
    {
        FILL(0xff)

       *(.after_vectors*)
       *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > Flash320

    .NVM_TABLE :
    {
      . = ALIGN(4);
      PROVIDE(__start_NVM_TABLE = .);
      KEEP(*(.NVM_TABLE));
      PROVIDE(__stop_NVM_TABLE = .);
      . = ALIGN(4);
    } > Flash320

    .VERSION_TAGS :
    {
        . = ALIGN(4);
        PROVIDE(__start_VERSION_TAGS = .);
        KEEP(*(.VERSION_TAGS));
        PROVIDE(__stop_VERSION_TAGS = .);
        . = ALIGN(4);
    } > Flash320

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > Flash320
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > Flash320
    __exidx_end = .;

    _etext = .;

    .interrupts_ram :
    {
        . = ALIGN(4);
        __VECTOR_RAM__ = .;
        __interrupts_ram_start__ = .;   /* Create a global symbol at data start */
        *(.m_interrupts_ram)            /* This is a user defined section */
        . += M_VECTOR_RAM_SIZE;
        . = ALIGN(4);
        __interrupts_ram_end__ = .;     /* Define a global symbol at data end */
    } > RAM0
    .scratch_area (NOLOAD): ALIGN(4)
    {
       __scratch_area_start__ = .;
       . = ALIGN(4) ;
       . += 0x400;
       __scratch_area_top__ = .;

    } > SCRATCH_RAM

    /* Main DATA section (RAM0) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       *(.ll_exchange_mem)   /* TO fix : create a specific section for *(.ll_exchange_mem) before the . data section */
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > RAM0 AT>Flash320

    __VECTOR_RAM = __VECTOR_RAM__;
    __RAM_VECTOR_TABLE_SIZE_BYTES = DEFINED(__ram_vector_table__) ? (__interrupts_ram_end__ - __interrupts_ram_start__) : 0x0;

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;

        PROVIDE(end = .);
    } > RAM0

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*)
        . = ALIGN(4) ;
        _end_noinit = .;
    } > RAM0

    /* end of firmware RAM to be retained in power down mode */
    _end_fw_retention = .;

    /* non retained RAM section */
    .s_start_non_ret (NOLOAD) : ALIGN(4)
    {
    	*(.s_start_non_ret*)
    	. = ALIGN(4) ;
    } > RAM0

    /* For now, Heap does not need to ne retained as configuration is baremetal
       and no allocator is implemented. Used only ponctually by stdio (sprintf) */
    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_pvHeapLimit = _pvHeapStart + (HEAP_SIZE));
    PROVIDE(_scratch_buf_start = __scratch_area_start__);
    PROVIDE(_scratch_buf_end = __scratch_area_top__);
    /* stack for rom boot during warm resume */
    .boot_resume_stack (NOLOAD): ALIGN(4)
    {
        _boot_resume_stack = .;
        *(.boot_resume_stack*)
        . += BOOT_RESUME_STACK_SIZE;
        . = ALIGN(4) ;
        _end_boot_resume_stack = .;
    } > RAM0

    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RAM0 - 32);
    __StackLimit = _vStackTop - STACK_SIZE;
    /* note : in power down, the usage of the stack is reduced, so BOOT_RESUME_STACK_SIZE is not considered
       (Bare metal only)*/
    ASSERT(__StackLimit >= _pvHeapLimit, "Possible stack corruption with data/bss")

}
ASSERT(m_app_size % 8192 == 0, "Stated size must be a multiple of 8k")

GROUP(libcr_nohost.a libcr_c.a libcr_eabihelpers.a)
