/*
 * Copyright (c) 2015 Focalcrest, Ltd. All rights reserved.
 *
 * Author(s):
 *     Phil Han <phil@focalcrest.com>
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <sys/select.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <errno.h>

#include "jennic_core.h"

#if 0
#define JENNIC_SERIAL_DEBUG
#else
#define JENNIC_SERIAL_DEBUG printf
#endif

typedef struct
{
    int baud;
    speed_t speed;
}_st_serial_bdmap_t, *_pst_serial_bdmap_t;

typedef struct
{
    int handle;
    int baudrate;
    struct termios options;

    int init_flag;
}_stuart_t, *_pstuart_t;


static _stuart_t gdc;
static _pstuart_t gpdc = &gdc;

static unsigned char serial_cache[2048];

static _st_serial_bdmap_t _bd_maps[] =
{
    #ifdef B50
    {50, B50},
    #endif
    #ifdef B75
    {75, B75},
    #endif
    #ifdef B110
    {110, B110},
    #endif
    #ifdef B134
    {134, B134},
    #endif
    #ifdef B150
    {150, B150},
    #endif
    #ifdef B200
    {200, B200},
    #endif
    #ifdef B300
    {300, B300},
    #endif
    #ifdef B600
    {600, B600},
    #endif
    #ifdef B1200
    {1200, B1200},
    #endif
    #ifdef B1800
    {1800, B1800},
    #endif
    #ifdef B2400
    {2400, B2400},
    #endif
    #ifdef B4800
    {4800, B4800},
    #endif
    #ifdef B9600
    {9600, B9600},
    #endif
    #ifdef B19200
    {19200, B19200},
    #endif
    #ifdef B38400
    {38400, B38400},
    #endif
    #ifdef B57600
    {57600, B57600},
    #endif
    #ifdef B115200
    {115200, B115200},
    #endif
    #ifdef B230400
    {230400, B230400},
    #endif
    #ifdef B460800
    {460800, B460800},
    #endif
    #ifdef B500000
    {500000, B500000},
    #endif
    #ifdef B576000
    {576000, B576000},
    #endif
    #ifdef B921600
    {921600, B921600},
    #endif
    #ifdef B1000000
    {1000000, B1000000},
    #endif
    #ifdef B1152000
    {1152000, B1152000},
    #endif
    #ifdef B1500000
    {1500000, B1500000},
    #endif
    #ifdef B2000000
    {2000000, B2000000},
    #endif
    #ifdef B2500000
    {2500000, B2500000},
    #endif
    #ifdef B3000000
    {3000000, B3000000},
    #endif
    #ifdef B3500000
    {3500000, B3500000},
    #endif
    #ifdef B4000000
    {4000000, B4000000},
    #endif
};

static int _uart_set_baudrate(int baudrate)
{
    _pstuart_t pdc = gpdc;
    u_int32_t ii = 0;
    int _baudrate = 0;
    int bd_valid = 0;

    JENNIC_SERIAL_DEBUG("[uart]: change baudrate to %d\n", baudrate);

    for(ii=0; ii<sizeof(_bd_maps)/sizeof(_bd_maps[0]); ii++)
    {
        if(baudrate == _bd_maps[ii].baud)
        {
            bd_valid = 1;
            _baudrate = _bd_maps[ii].speed;
        }
    }

    if(0 == bd_valid)
    {
        JENNIC_SERIAL_DEBUG("[uart]: invalid baudrate %d ! \n", baudrate);
        return -1;
    }

    if(-1 == tcflush(pdc->handle, TCIOFLUSH))
    {
        JENNIC_SERIAL_DEBUG("[uart]: flushing uart error! \n");
        return -1;
    }

    if(-1 == cfsetispeed(&pdc->options, _baudrate))
    {
        JENNIC_SERIAL_DEBUG("[uart]: setting input speed error! \n");
        return -1;
    }

    if(-1 == cfsetospeed(&pdc->options, _baudrate))
    {
        JENNIC_SERIAL_DEBUG("[uart]: setting output speed error! \n");
        return -1;
    }

    if(-1 == tcsetattr(pdc->handle, TCSANOW, &pdc->options))
    {
        JENNIC_SERIAL_DEBUG("[uart]: changing port settings error! \n");
        return -1;
    }

    return 0;
}


static int _uart_open(char* dev, int baudrate)
{
    _pstuart_t pdc = gpdc;

    if(1 == pdc->init_flag)
    {
        return 0;
    }

    JENNIC_SERIAL_DEBUG("[uart]: init device %s\n", dev);
    memset(pdc, 0, sizeof(_stuart_t));

    //open the device(com port) to be non-blocking (read will return immediately)
    pdc->handle = open(dev, O_RDWR|O_NOCTTY);// |O_NONBLOCK|O_NDELAY);
    if (pdc->handle < 0)
    {
        JENNIC_SERIAL_DEBUG("[uart]: open device %s failed!\n", dev);
        return -1;
    }

    if (tcgetattr(pdc->handle, &pdc->options) == -1)
    {
        JENNIC_SERIAL_DEBUG("[uart]: get port settings failed!\n");
        return -1;
    }

    pdc->options.c_iflag &= ~(INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXANY|IXOFF);
    pdc->options.c_iflag = IGNBRK|IGNPAR;
    pdc->options.c_oflag &= ~(OPOST|OLCUC|ONLCR|OCRNL|ONOCR|ONLRET);
    pdc->options.c_cflag &= ~(CSIZE|CSTOPB|PARENB|CRTSCTS);
    pdc->options.c_cflag |= CS8|CREAD|HUPCL|CLOCAL;
    pdc->options.c_lflag &= ~(ISIG|ICANON|ECHO|IEXTEN);

    fcntl(pdc->handle, F_SETFL, O_NONBLOCK);

    if(0 != _uart_set_baudrate(baudrate))
    {
        return -1;
    }

    pdc->init_flag = 1;
    return 0;
}


static int _uart_close()
{
    _pstuart_t pdc = gpdc;

    if(1 == pdc->init_flag)
    {
        close(pdc->handle);
        memset(pdc, 0, sizeof(_stuart_t));
    }

    return 0;
}


static int _uart_read_ext(u_int8_t* pbuf, int buflen, int *prnum, int timeout)
{
    _pstuart_t pdc = gpdc;
    int ret = -1;
    struct timeval _timeout;
    fd_set fdset;
    int fdmax = 0;
    char str_error[512] = {0};

    if(0 == pdc->init_flag)
    {
        JENNIC_SERIAL_DEBUG("[uart]: not init, read failed! \n");
        return -1;
    }

    if(pbuf == NULL)
    {
        JENNIC_SERIAL_DEBUG("[uart]: buf invalid, read failed! \n");
        return -1;
    }

    *prnum = 0;

    _timeout.tv_usec = timeout % 1000000;
    _timeout.tv_sec = timeout / 1000000;

    FD_ZERO(&fdset);
    fdmax = pdc->handle;
    FD_SET(pdc->handle, &fdset);

    ret = select(fdmax + 1, &fdset, NULL, NULL, &_timeout);
    if(0 > ret)
    {
        strerror_r(errno, str_error, sizeof(str_error));
        JENNIC_SERIAL_DEBUG("[uart]: reading error: %s ! \n", str_error);
        return -1;
    }
    else if(0 == ret)
    {
        JENNIC_SERIAL_DEBUG("[uart]: reading timeout! \n");
    }
    else
    {
        if(FD_ISSET(pdc->handle, &fdset))
        {
            ret = read(pdc->handle, (u_int8_t*)pbuf, buflen);
            if(ret > 0)
            {
                JENNIC_SERIAL_DEBUG("[uart]: get 0x%02x %c (rnum = %d) \n", *pbuf, *pbuf, ret);
                *prnum = ret;
            }
            else
            {
                ret = 0;
            }
        }
        else
        {
            JENNIC_SERIAL_DEBUG("[uart]: FD_ISSET set failed! \n");
        }
    }

    return 0;
}

static int _uart_write(u_int8_t* pbuf, int len)
{
    _pstuart_t pdc = gpdc;
    int wnum = 0;
    int wfocus = 0;

    if(0 == pdc->init_flag)
    {
        JENNIC_SERIAL_DEBUG("[uart]: not init, write failed! \n");
        return -1;
    }

    do
    {
        wnum = write(pdc->handle, &pbuf[wfocus], len-wfocus);
        if(wnum < 0)
        {
            if(errno != EAGAIN)
            {
                JENNIC_SERIAL_DEBUG("[uart]: could not write!\n");
                return -1;
            }
        }
        else
        {
            wfocus += wnum;
        }
    } while(wfocus < len);

    return 0;
}

static int _serial_msg_write(ezb_ll_msg_t stype, u_int32_t* paddr, u_int16_t mlen,
                             u_int8_t sdatalen, u_int8_t *psdata)
{
    unsigned char msg_crc = 0;
    int msg_idx = 0;
    int msg_len = 3;
    int ii = 0;

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
        printf("serial: talking msg length %d is bigger than 256 bytes", msg_len);
        return -1;
    }

    // msg len
    serial_cache[msg_idx++] = msg_len -1;
    // msg type
    serial_cache[msg_idx++] = stype;
    // msg addr
    if(NULL != paddr)
    {
        serial_cache[msg_idx++] = 0xFF & *paddr;
        serial_cache[msg_idx++] = 0xFF & (*paddr>>8);
        serial_cache[msg_idx++] = 0xFF & (*paddr>>16);
        serial_cache[msg_idx++] = 0xFF & (*paddr>>24);
    }

    // move data offset
    if(0 != mlen)
    {
        serial_cache[msg_idx++] = 0xFF & mlen;
        serial_cache[msg_idx++] = 0xFF & (mlen>>8);
    }

    // msg data payload
    if(0 != sdatalen)
    {
        for(ii=0; ii<sdatalen; ii++)
        {
            serial_cache[msg_idx++] = psdata[ii];
        }
    }

    //crc
    for(ii=0; ii<(msg_idx); ii++)
    {
        msg_crc ^= serial_cache[ii];
    }

    serial_cache[msg_idx++] = msg_crc;

    return _uart_write(serial_cache, msg_idx);
}


static int _serial_msg_read(pezb_ll_msg_t ptype, u_int8_t *prlen, u_int8_t *prbuf, int timeout)
{
    int ii = 0;
    u_int8_t rbuf[256];
    u_int8_t rcsum = 0;
    u_int8_t rlen = 0;
    u_int8_t rfocus = 0;
    int rnum = 0;
    int rtimes = 0;

    /* Get the length byte */
    if((0 != _uart_read_ext(&rlen, 1, &rnum, timeout)) || (1 != rnum))
    {
        JENNIC_SERIAL_DEBUG("serial msg: get length failed! \n");
        return -1;
    }

    /* Add length to checksum */
    rcsum ^= rlen;

    do
    {
        /* Get the rest of the message */
        if(0 != _uart_read_ext(&rbuf[rfocus], (rlen-rfocus), &rnum, timeout))
        {
            JENNIC_SERIAL_DEBUG("serial msg: reading msg failed! \n");
            return -1;
        }

        rfocus += rnum;
        rtimes ++;

    } while((rfocus < rlen) && (rnum > 0 || rtimes < 10));

    if(*ptype != rbuf[0])
    {
        JENNIC_SERIAL_DEBUG("serial msg: request type %02x, answer type 0x%02x \n", *ptype, rbuf[0]);
        return -1;
    }

    if(rfocus != rlen)
    {
        JENNIC_SERIAL_DEBUG("serial msg: got [%d/%d] with %d times trying: \n", rfocus, rlen, rtimes);

        for(ii=0; ii<rfocus; ii++)
        {
            JENNIC_SERIAL_DEBUG("%02x ", rbuf[ii]);
        }

        JENNIC_SERIAL_DEBUG("\n");
        return -1;
    }

    /* add rest of message to checksum */
    for(ii=0; ii<rlen; ii++)
    {
        rcsum ^= rbuf[ii];
    }

    if(rcsum != 0x00)
    {
        JENNIC_SERIAL_DEBUG("serial msg: checksum failed! \n");
        return -1;
    }

    *prlen = rlen - 1;
    memcpy(prbuf, &rbuf[1], *prlen);

    JENNIC_SERIAL_DEBUG("serial msg: read len %d \n", *prlen);

    return 0;
}

int jennic_serial_init(int param1, int param2)
{
    char* dev = (char*) param1;
    return _uart_open(dev, param2);
}


int jennic_serial_prepare(void)
{
    return 0;
}

int jennic_serial_talk(ezb_ll_msg_t stype, pezb_ll_msg_t prtype,
                       u_int32_t* paddr, u_int16_t mlen, u_int8_t sdatalen,
                       u_int8_t *psdata, u_int8_t *prlen, u_int8_t *prbuf)
{
    _pstuart_t pdc = gpdc;

    if(0 == pdc->init_flag)
    {
        printf("serial: not inited \n");
        return -1;
    }

    if(NULL != prtype)
    {
        if(0 == _serial_msg_write(stype, paddr, mlen, sdatalen, psdata))
        {
            _serial_msg_read(prtype, prlen, prbuf, 1000000);
        }
    }
    else
    {
        _serial_msg_write(stype, paddr, mlen, sdatalen, psdata);
    }

    return 0;
}

int jennic_serial_fini(void)
{
    return _uart_close();
}

