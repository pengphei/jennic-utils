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

#ifndef _JENNIC_CORE_H_
#define _JENNIC_CORE_H_

#include <sys/types.h>

typedef enum
{
    JN_CHIP_JN5139,
    JN_CHIP_JN5142,
    JN_CHIP_JN5148,
    JN_CHIP_JN516X
} ejennic_chip_t;

typedef enum
{
    /* cmds referring to firmware updating */
    E_ZB_CMD_FLASH_ERASE_REQUEST                = 0x07,
    E_ZB_CMD_FLASH_ERASE_RESPONSE               = 0x08,
    E_ZB_CMD_FLASH_PROGRAM_REQUEST              = 0x09,
    E_ZB_CMD_FLASH_PROGRAM_RESPONSE             = 0x0a,
    E_ZB_CMD_FLASH_READ_REQUEST                 = 0x0b,
    E_ZB_CMD_FLASH_READ_RESPONSE                = 0x0c,
    E_ZB_CMD_FLASH_SECTOR_ERASE_REQUEST         = 0x0d,
    E_ZB_CMD_FLASH_SECTOR_ERASE_RESPONSE        = 0x0e,
    E_ZB_CMD_FLASH_WRITE_STATUS_REGISTER_REQUEST= 0x0f,
    E_ZB_CMD_FLASH_WRITE_STATUS_REGISTER_RESPONSE= 0x10,
    E_ZB_CMD_RAM_WRITE_REQUEST                  = 0x1d,
    E_ZB_CMD_RAM_WRITE_RESPONSE                 = 0x1e,
    E_ZB_CMD_RAM_READ_REQUEST                   = 0x1f,
    E_ZB_CMD_RAM_READ_RESPONSE                  = 0x20,
    E_ZB_CMD_RAM_RUN_REQUEST                    = 0x21,
    E_ZB_CMD_RAM_RUN_RESPONSE                   = 0x22,
    E_ZB_CMD_FLASH_READ_ID_REQUEST              = 0x25,
    E_ZB_CMD_FLASH_READ_ID_RESPONSE             = 0x26,
    E_ZB_CMD_SET_BAUD_REQUEST                   = 0x27,
    E_ZB_CMD_SET_BAUD_RESPONSE                  = 0x28,
    E_ZB_CMD_FLASH_SELECT_TYPE_REQUEST          = 0x2c,
    E_ZB_CMD_FLASH_SELECT_TYPE_RESPONSE         = 0x2d,
    E_ZB_CMD_GET_CHIPID_REQUEST                 = 0x32,
    E_ZB_CMD_GET_CHIPID_RESPONSE                = 0x33,
} ezb_ll_msg_t, *pezb_ll_msg_t;


typedef int (*func_jnwrap_init_t)(int para0, int para1);
typedef int (*func_jnwrap_pre_t)(void);
typedef int (*func_jnwrap_talk_t)(ezb_ll_msg_t stype, pezb_ll_msg_t prtype,
                                  u_int32_t* paddr, u_int16_t mlen, u_int8_t sdatalen,
                                  u_int8_t *psdata, u_int8_t *prlen, u_int8_t *prbuf);
typedef int (*func_jnwrap_fini_t)();

typedef struct
{
    func_jnwrap_init_t init;
    func_jnwrap_pre_t prepare;
    func_jnwrap_talk_t talk;
    func_jnwrap_fini_t fini;
} stjn_wrapper_t, *pstjn_wrapper_t;

void jennic_wrapper_init(pstjn_wrapper_t pwrapper);

int jennic_init(int para0, int para1);
int jennic_fini(void);
int jennic_select_flash(void);
int jennic_change_baudrate(int baudrate);
int jennic_write_ram(u_int32_t addr, u_int8_t wlen, u_int8_t* pwbuf);
int jennic_read_ram(u_int32_t addr, u_int16_t len, u_int8_t* prlen, u_int8_t* prbuf);
int jennic_run_ram(u_int32_t addr);
int jennic_write_flash(u_int32_t addr, u_int8_t wlen, u_int8_t* pwbuf);
int jennic_read_flash(u_int32_t addr, u_int16_t len, u_int8_t* prlen, u_int8_t* prbuf);
int jennic_erase_flash(void);
int jennic_erase_flash_sector(u_int8_t sector);
int jennic_set_flash_register(u_int8_t status);
int jennic_get_chip_id(u_int32_t* pid);
int jennic_read_mac(u_int8_t pmac[8], ejennic_chip_t chip, int busermac);
int jennic_write_mac(u_int8_t pmac[8], ejennic_chip_t chip);
#endif
