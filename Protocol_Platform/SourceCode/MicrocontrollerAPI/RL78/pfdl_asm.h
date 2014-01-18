/*******************************************************************************
* Library       : Flash Data Library T04 (PicoFDL)
*
* File Name     : $Source: pfdl_asm.h $
* Lib. Version  : $RL78_FDL_LIB_VERSION_T04_GNU: V1.04 $
* Mod. Revision : $Revision: 1.8 $
* Mod. Date     : $Date: 2013/02/19 09:59:38MEZ $
* Device(s)     : RL78
* Description   : Preprocessor defines for library usage from assembler
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

#ifndef __PFDL_ASM_H_INCLUDED
#define __PFDL_ASM_H_INCLUDED


/* PFDL command code set as used for pfdl_command_t */
                                                          /* ---------------------------------------------- */
#define PFDL_CMD_READ_BYTES            (0x00)             /* 0x00, reads data from flash memory             */
#define PFDL_CMD_IVERIFY_BYTES         (0x06)             /* 0x06, verifies data if flash content is stable */
#define PFDL_CMD_BLANKCHECK_BYTES      (0x08)             /* 0x08, checks if flash content is blank         */
#define PFDL_CMD_WRITE_BYTES           (0x04)             /* 0x04, writes data into flash memory            */
#define PFDL_CMD_ERASE_BLOCK           (0x03)             /* 0x03, erases one flash block                   */
                                                          /* ---------------------------------------------- */


/* PFDL error code set as used for pfdl_status_t */

/* operation related status                           */  /* ---------------------------------------------- */
#define PFDL_IDLE                      (0x30)             /* 0x30, PFDL ready to receive requests           */
#define PFDL_OK                        (0x00)             /* 0x00, command finished without problems        */
#define PFDL_BUSY                      (0xFF)             /* 0xFF, command is being processed               */
/* flash related status                               */  /* ---------------------------------------------- */
#define PFDL_ERR_PROTECTION            (0x10)             /* 0x10, protection error (access right conflict) */
#define PFDL_ERR_ERASE                 (0x1A)             /* 0x1A, erase error                              */
#define PFDL_ERR_MARGIN                (0x1B)             /* 0x1B, blankcheck or verify margin violated     */
#define PFDL_ERR_WRITE                 (0x1C)             /* 0x1C, write error                              */
#define PFDL_ERR_PARAMETER             (0x05)             /* 0x05, parameter error                          */
                                                          /* ---------------------------------------------- */


#endif
