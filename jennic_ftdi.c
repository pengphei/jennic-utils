/*
 * Copyright (c) 2015 Focalcrest, Ltd. All rights reserved.
 *
 * Author(s):
 *     Phil Han <phil@focalcrest.com>
 */

#include <stdio.h>
#include <string.h>
#include <ftdi.h>
#include "util_crc.h"
#include "jennic_core.h"

#if 1
#define JENNIC_FTDI_DEBUG
#else
#define JENNIC_FTDI_DEBUG printf
#endif

typedef struct
{
    int vendor;
    int product;
}_stusb_vp_t, *_pstusb_vp_t;

_stusb_vp_t usb_vps[] =
{
    {0x0403, 0x6001}, // FT232 Chip
    {0x0403, 0xcc40}, // Teco usbbridge
    {0x0403, 0x6010}, // FT2232 chips
};

typedef struct
{
    struct ftdi_context context;
    int init_flag;
    int reset_io;
    int spimiso_io;
} _st_ftdi_dc_t, *_pst_ftdi_dc_t;

static _st_ftdi_dc_t gdc;
static _pst_ftdi_dc_t gpdc = &gdc;

static unsigned char ftdi_cache[2048];

int jennic_ftdi_init(int reset_io, int spimiso_io)
{
    _pst_ftdi_dc_t pdc = gpdc;
    struct ftdi_context* fcontext = &pdc->context;
    int ii = 0;
    int dev_num = sizeof(usb_vps)/sizeof(_stusb_vp_t);

    if(1 == pdc->init_flag)
    {
        printf("ftdi: already inited \n");
        return 0;
    }

    pdc->reset_io = reset_io;
    pdc->spimiso_io = spimiso_io;

    ftdi_init(fcontext);

    for(ii=0; ii<dev_num; ii++)
    {
        if(0 == ftdi_usb_open(fcontext, usb_vps[ii].vendor, usb_vps[ii].product))
        {
            printf("ftdi: avaliable dev [0x%04x: 0x%04x]\n", usb_vps[ii].vendor, usb_vps[ii].product);
            break;
        }
    }

    if(ii == dev_num)
    {
        printf("ftdi: non avaliable ftdi devices exists!\n");
        return -1;
    }

    pdc->init_flag = 1;

    return 0;
}

int jennic_ftdi_prepare(void)
{
    _pst_ftdi_dc_t pdc = gpdc;
    struct ftdi_context* fcontext = &pdc->context;
    unsigned char bit_data = 0x00;
    unsigned char bit_mask_0 = (unsigned char)(1 << gpdc->reset_io) | (1 << gpdc->spimiso_io);
    unsigned char bit_mask_1 = (unsigned char)(1 << gpdc->spimiso_io);

    if(0 == pdc->init_flag)
    {
        printf("ftdi: not inited \n");
        return -1;
    }

    // reset and clear buffer
    ftdi_usb_reset(fcontext);
    ftdi_usb_purge_buffers(fcontext);
    ftdi_usb_purge_buffers(fcontext);

    ftdi_read_data(fcontext, ftdi_cache, 1024);

    // reset board, enter programing mode
    ftdi_enable_bitbang(fcontext, bit_mask_0);
    ftdi_write_data(fcontext, &bit_data, 1);
    usleep(200000);
    ftdi_disable_bitbang(fcontext);
    ftdi_enable_bitbang(fcontext, bit_mask_1);
    ftdi_write_data(fcontext, &bit_data, 1);
    usleep(200000);
    ftdi_disable_bitbang(fcontext);

    //clear crap
    ftdi_usb_reset(fcontext);
    ftdi_set_baudrate(fcontext, 38400);
    ftdi_set_line_property(fcontext, BITS_8, STOP_BIT_1,NONE);
    ftdi_setrts(fcontext, 1);
    ftdi_setflowctrl(fcontext, SIO_RTS_CTS_HS);
    ftdi_setrts(fcontext, 0);
    return 0;
}

int jennic_ftdi_talk(ezb_ll_msg_t stype, pezb_ll_msg_t prtype, u_int32_t* paddr, u_int16_t mlen, u_int8_t sdatalen, u_int8_t *psdata,
                   u_int8_t *prlen, u_int8_t *prbuf)
{
    _pst_ftdi_dc_t pdc = gpdc;
    struct ftdi_context* fcontext = &pdc->context;
    unsigned char msg_crc = 0;
    int msg_idx = 0;
    int msg_len = 3;
    int ii = 0;

    if(0 == pdc->init_flag)
    {
        printf("ftdi: not inited \n");
        return -1;
    }

    if( NULL != paddr)
    {
        msg_len += 4;
    }

    if(0 != mlen)
    {
        msg_len += 2;
    }

    if(0 != sdatalen)
    {
        msg_len += sdatalen;
    }

    if(msg_len > 0xFF)
    {
        printf("ftdi: talking msg length %d is bigger than 256 bytes", msg_len);
        return -1;
    }

    // msg len
    ftdi_cache[msg_idx++] = msg_len -1;
    // msg type
    ftdi_cache[msg_idx++] = stype;
    // msg addr
    if(NULL != paddr)
    {
        ftdi_cache[msg_idx++] = 0xFF & *paddr;
        ftdi_cache[msg_idx++] = 0xFF & (*paddr>>8);
        ftdi_cache[msg_idx++] = 0xFF & (*paddr>>16);
        ftdi_cache[msg_idx++] = 0xFF & (*paddr>>24);
    }

    // move data offset
    if(0 != mlen)
    {
        ftdi_cache[msg_idx++] = 0xFF & mlen;
        ftdi_cache[msg_idx++] = 0xFF & (mlen>>8);
    }

    // msg data payload
    if(0 != sdatalen)
    {
        for(ii=0; ii<sdatalen; ii++)
        {
            ftdi_cache[msg_idx++] = psdata[ii];
        }
    }

    //crc
    for(ii=0; ii<(msg_idx); ii++)
    {
        msg_crc ^= ftdi_cache[ii];
    }

    ftdi_cache[msg_idx++] = msg_crc;

    //util_debug_buf("sendbuf", ftdi_cache, msg_idx);

    if(NULL != prtype)
    {
        unsigned char rlen = 0;
        unsigned char ans_len = 0;
        unsigned char waited = 0;
        while(0 == ans_len)
        {
            waited = 0;
            ftdi_write_data(fcontext, ftdi_cache, msg_idx);
            while(waited < 150 && 0 == ans_len)
            {
                ans_len = ftdi_read_data(fcontext, &rlen, 1);
            }
        }

        ans_len = rlen;
        rlen = ftdi_read_data(fcontext, ftdi_cache, ans_len);

        if(rlen != ans_len)
        {
            printf("ftdi: data reading lengh %d != answer length %d \n", rlen, ans_len);
            return -1;
        }

        if(ftdi_cache[0] != *prtype)
        {
            printf("ftdi: read type 0x%02x, require type 0x%02x \n", ftdi_cache[0], *prtype);
            return -1;
        }

        ans_len = ans_len - 1;

        if(*prlen >= ans_len)
        {
            memcpy(prbuf, &ftdi_cache[1], ans_len);
            *prlen = ans_len;
        }
        else
        {
            memcpy(prbuf, &ftdi_cache[1], *prlen);
            printf("ftdi: msg 0x%x, rbuf len: %d, msg len: %d, buffer overflow \n", stype, *prlen, ans_len);
        }
        //util_debug_buf("readbuf", prbuf, *prlen);
    }
    else
    {
        ftdi_write_data(fcontext, ftdi_cache, msg_idx);
    }

    return 0;
}

int jennic_ftdi_fini()
{
    _pst_ftdi_dc_t pdc = gpdc;
    struct ftdi_context* fcontext = &pdc->context;
    unsigned char bit_mask_reset = (unsigned char)(1 << gpdc->reset_io);
    unsigned char msg_byte = 0x00;

    if(0 == pdc->init_flag)
    {
        printf("ftdi: not inited \n");
        return -1;
    }

    ftdi_enable_bitbang(fcontext, bit_mask_reset);
    ftdi_write_data(fcontext, &msg_byte, 1);
    usleep(1000000);
    ftdi_disable_bitbang(fcontext);
    ftdi_usb_close(fcontext);

    memset(pdc, 0, sizeof(_st_ftdi_dc_t));

    return 0;
}


