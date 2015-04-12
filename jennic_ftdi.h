/*
 * Copyright (c) 2015 Focalcrest, Ltd. All rights reserved.
 *
 * Author(s):
 *     Phil Han <phil@focalcrest.com>
 */

#ifndef JENNIC_FTDI_H
#define JENNIC_FTDI_H

#include <sys/types.h>

int jennic_ftdi_init(int reset_io, int spimiso_io);
int jennic_ftdi_prepare(void);
int jennic_ftdi_talk(ezb_ll_msg_t stype, pezb_ll_msg_t prtype,
                     u_int32_t* paddr, u_int16_t mlen,
                     u_int8_t sdatalen, u_int8_t *psdata,
                     u_int8_t *prlen, u_int8_t *prbuf);
int jennic_ftdi_fini();

#endif // JENNIC_FTDI_H

