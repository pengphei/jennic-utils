/*
 * Copyright (c) 2015 Focalcrest, Ltd. All rights reserved.
 *
 * Author(s):
 *     Phil Han <phil@focalcrest.com>
 */

#include <stdio.h>
#include <string.h>
#include "jennic_core.h"

#if 0
#define JENNIC_SERIAL_DEBUG
#else
#define JENNIC_SERIAL_DEBUG printf
#endif

int jennic_serial_init(int param1, int param2)
{
    return 0;
}


int jennic_serial_prepare(void)
{
    return 0;
}

int jennic_serial_talk(ezb_ll_msg_t stype, pezb_ll_msg_t prtype,
                       u_int32_t* paddr, u_int16_t mlen, u_int8_t sdatalen,
                       u_int8_t *psdata, u_int8_t *prlen, u_int8_t *prbuf)
{
    return 0;
}

int jennic_serial_fini(void)
{
    return 0;
}

