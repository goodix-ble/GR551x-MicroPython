/**
 ****************************************************************************************
 *
 * @file svc_handler.h
 *
 * Copyright(C) 2016-2017, Shenzhen Huiding Technology Co., Ltd
 * All Rights Reserved
 *
 ****************************************************************************************
 */
#include "svc_handler.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern uint32_t get_patch_rep_addr(uint32_t ori_func);
#define MAX_SVC_COUNT 256

void * SVC_Table[MAX_SVC_COUNT] __attribute__((section("SVC_TABLE")))={
    NULL,        //SVC 0 Function Entry
    NULL,         //SVC 1 Function Entry
    NULL,
    NULL,
};

uint8_t svc_func_table_register(svc_table_t svc_table[],uint8_t table_size)
{
    uint8_t i = 0;
    uint8_t status = 0;
    if (svc_table == NULL || table_size == 0)
    {
        status = 0;
    }
    else
    {
        for (i=0; i<table_size; i++)
        {

                SVC_Table[svc_table[i].svc_num] = svc_table[i].func;

        }
    }
    return status;
}
uint8_t svc_func_register(uint8_t svc_num,void * func)
{
    if(func!=NULL)
    {
        SVC_Table[svc_num] = func;
    }
		return 0;
}

/**
 ****************************************************************************************
 * @brief SVC_Handler. Handles h/w patching mechanism IRQ
 *
 * @return void 
 ****************************************************************************************
 */
uint32_t  SVC_Handler_C(unsigned int * svc_args)
{
    // Stack frame contains:
    // r0, r1, r2, r3, r12, r14, the return address and xPSR
    // - Stacked R0 = svc_args[0]
    // - Stacked R1 = svc_args[1]
    // - Stacked R2 = svc_args[2]
    // - Stacked R3 = svc_args[3]
    // - Stacked R12 = svc_args[4]
    // - Stacked LR = svc_args[5]
    // - Stacked PC = svc_args[6]
    // - Stacked xPSR= svc_args[7]

    uint16_t svc_cmd;
    uint32_t svc_func_addr;
    uint32_t func_addr=0;
    svc_func_addr =svc_args[6];
    svc_cmd = *((uint16_t*)svc_func_addr-1);
    //__BKPT(12);
    if((svc_cmd<=0xDFFF)&&(svc_cmd>=0xDF00))
    {
        func_addr =(uint32_t)SVC_Table[svc_cmd&(0xFF)];
        return func_addr ;
    }
    else
    {
        func_addr=get_patch_rep_addr(svc_func_addr);
        svc_args[6]=func_addr;
        return 0;
    }
    
    //return func_addr ;
}

//__asm void SVC_Handler (void) {
//                PRESERVE8
//                .extern    SVC_Handler_C
//                
//                TST     LR,$4                   ; Called from Handler Mode?
//                MRSNE   R12,PSP                 ; Yes, use PSP
//                MOVEQ   R12,SP                  ; No, use MSP
//                PUSH    {R0-R3,LR}
//                MOV  R0, R12
//                BL  SVC_Handler_C
//                MOV  R12, R0
//                POP {R0-R3}
//                CMP R12,$0                      //make sure current point isn't null
//                BLXNE     R12
//                POP {LR}
//                //BKPT    #12
//                TST     LR,$4
//                MRSNE   R12,PSP
//                MOVEQ   R12,SP
//                STM     R12,{R0-R3}           ; Function return values
//                BX      LR                      ; RETI

//SVC_Dead
//                B       SVC_Dead                ; None Existing SVC


//                ALIGN
//}
void __attribute__((naked))SVC_Handler (void)
{
    __asm("TST R14,$4\n");
    __asm("IT NE\n");
    __asm("MRSNE   R12,PSP\n");
    __asm("IT EQ\n");
    __asm("MOVEQ   R12,SP\n");
    __asm("PUSH    {R0-R3,LR}\n");
    __asm("MOV  R0, R12\n");
    __asm("BL  SVC_Handler_C\n");
    __asm("MOV  R12, R0\n");
    __asm("POP {R0-R3}\n");
    __asm("CMP R12,$0\n");
    __asm("IT NE\n");
    __asm("BLXNE     R12\n");
    __asm("POP {R14}\n");
    __asm("TST     R14,$4\n");
    __asm("IT NE\n");
    __asm("MRSNE   R12,PSP\n");
    __asm("IT EQ\n");
    __asm("MOVEQ   R12,SP\n");
    __asm("STM     R12,{R0-R3}\n");
    __asm("BX      LR\n");
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
