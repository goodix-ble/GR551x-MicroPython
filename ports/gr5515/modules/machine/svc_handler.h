#ifndef _SVC_HANDLER_H__
#define _SVC_HANDLER_H__

#undef NULL  /* others (e.g. <stdio.h>) also define */
#define NULL 0

#include<stdint.h>
#define USER_SVC_OFFSET 0XF0

typedef struct
{
    uint8_t svc_num;
    void * func;
} svc_table_t;

uint8_t svc_func_table_register(svc_table_t svc_table[], uint8_t table_size);

#endif

