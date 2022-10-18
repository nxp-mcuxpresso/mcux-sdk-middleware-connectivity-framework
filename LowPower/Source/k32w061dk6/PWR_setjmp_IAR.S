;==========================================================================
;
; SPDX-License-Identifier: BSD-3-Clause
; Copyright (c) 2015, Freescale Semiconductor, Inc.
; Copyright 2016-2017, 2020 NXP
; All rights reserved.
;
; MODULE:      Power Manager
;
; COMPONENT:   pwrm_setjmp.s
;
; DESCRIPTION: Setjmp / longjmp for power manager warm start with memory held
;              for JN518x family
;
; $HeadURL: https://www.collabnet.nxp.com/svn/lprf_sware/Projects/Cortex/Modules/PWRM/Branches/ZBPRO_R20_v2.0/Source/pwrm_setjmp_JN517x.S $
;
; $Revision: 59489 $
;
; $LastChangedBy: nxp29764 $
;
; $LastChangedDate: 2014-03-04 18:04:09 +0000 (Tue, 04 Mar 2014) $
;
; $Id: pwrm_setjmp_JN517x.S 59489 2014-03-04 18:04:09Z nxp29764 $
;
;==========================================================================


                    PUBWEAK PWR_setjmp
                    SECTION .text:CODE:REORDER:NOROOT(2)
                    THUMB

PWR_setjmp   
                    MOV IP, SP
                    STMEA A1!, {V1-V7, FP, IP, LR}
                    MOV A1, #0
                    BX LR

                    PUBWEAK PWR_longjmp
                    SECTION .text:CODE:REORDER:NOROOT(2)
                    THUMB

PWR_longjmp   
                    LDMFD A1!, {V1-V7, FP, IP, LR}
                    MOV SP, IP
                    MOVS A1, A2
                    IT EQ
                    MOVEQ A1, #1
                    BX LR
                    END
