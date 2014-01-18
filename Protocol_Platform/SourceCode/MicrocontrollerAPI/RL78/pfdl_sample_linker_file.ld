/*******************************************************************************
* Library       : Flash Data Library T04 (PicoFDL)
*
* File Name     : $Source: pfdl_sample_linker_file.ld $
* Lib. Version  : $RL78_FDL_LIB_VERSION_T04_GNU: V1.04 $
* Mod. Revision : $Revision: 1.5 $
* Mod. Date     : $Date: 2013/02/21 09:13:09MEZ $
* Device(s)     : RL78/G13 (R5F100LE)
* Description   : Linker sample file, please modify according to your device
*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

OUTPUT_ARCH(rl78)
ENTRY(_start)

SECTIONS
{
    .vec 0x0 : AT (0x0)
    {
        KEEP(*(.vec))
    }

    .vects 0x4 : AT (0x4)
    {
        KEEP(*(.vects))
    }

    .option_bytes 0xC0 : AT (0xC0)
    {
        KEEP(*(.option_bytes))
    }

    .security_id 0xC4 : AT (0xC4)
    {
        KEEP(*(.security_id))
    }

    PFDL_COD 0x1000 : AT (0x1000)
    {
        *(PFDL_COD)
    }

    .text :
    {
        *(.text)
        *(.text.*)
        etext = .;
    }

    .init :
    {
        *(.init)
    }

    .fini :
    {
        *(.fini)
    }

    .got :
    {
        *(.got)
        *(.got.plt)
    }

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
        _erodata = .;
    }

    .eh_frame_hdr :
    {
        *(.eh_frame_hdr)
    }

    .eh_frame :
    {
        *(.eh_frame)
    }

    .jcr :
    {
        *(.jcr)
    }

    .tors :
    {
        . = ALIGN(2);
        __CTOR_LIST__ = .;
        ___ctors = .;
        *(.ctors)
        ___ctors_end = .;
        __CTOR_END__ = .;
        __DTOR_LIST__ = .;
        ___dtors = .;
        *(.dtors)
        ___dtors_end = .;
        __DTOR_END__ = .;
        _mdata = .;
    }

    .lowtext :
    {
        *(.plt)
        *(.lowtext)
        _elowtext = .;
    }

    .data 0x0FF700 : AT (_elowtext)
    {
        . = ALIGN(2);
        _data = .;
        *(.data)
        *(.data.*)
        _edata = .;
    }

    .bss :
    {
        . = ALIGN(2);
        _bss = .;
        *(.bss)
        *(.bss.**)
        *(COMMON)
        _ebss = .;
        _end = .;
    }

    .stack 0xFFE00 (NOLOAD)  : AT (0xFFE00)
    {
        _stack = .;
    }
}
