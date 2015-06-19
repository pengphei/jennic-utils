/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JENNIC_FTDI_H
#define JENNIC_FTDI_H

#ifdef JENNIC_FTDI_ENABLE

#include <sys/types.h>

int jennic_ftdi_init(int reset_io, int spimiso_io);
int jennic_ftdi_prepare(void);
int jennic_ftdi_talk(ezb_ll_msg_t stype, pezb_ll_msg_t prtype,
                     u_int32_t* paddr, u_int16_t mlen,
                     u_int8_t sdatalen, u_int8_t *psdata,
                     u_int8_t *prlen, u_int8_t *prbuf);
int jennic_ftdi_fini();
#endif

#endif // JENNIC_FTDI_H

