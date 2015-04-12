/*
 * Copyright (c) 2015 Focalcrest, Ltd. All rights reserved.
 *
 * Author(s):
 *     Phil Han <phil@focalcrest.com>
 */

#ifndef JENNIC_SERIAL_H
#define JENNIC_SERIAL_H

#include <sys/types.h>

int jennic_serial_init(int param1, int param2);
int jennic_serial_prepare(void);
int jennic_serial_talk(ezb_ll_msg_t stype, pezb_ll_msg_t prtype,
                       u_int32_t* paddr, u_int16_t mlen, u_int8_t sdatalen,
                       u_int8_t *psdata, u_int8_t *prlen, u_int8_t *prbuf);
int jennic_serial_fini(void);

#endif // JENNIC_SERIAL_H

