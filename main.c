/*
 * Copyright (c) 2015 Focalcrest, Ltd. All rights reserved.
 *
 * Author(s):
 *     Phil Han <phil@focalcrest.com>
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include "jennic_extra.h"
#include "jennic_core.h"
#include "jennic_ftdi.h"
#include "jennic_serial.h"
#include "util_crc.h"

typedef struct
{
    int verbosity;
    int verify_flag;
    int init_baudrate;
    int prog_baudrate;
    int show_flag;
    int reset_io;
    int spimiso_io;
    u_int64_t* pmac_addr;

    char *connection;
    char *firmware;

    u_int8_t* fw_image;
    int fw_fd;
}_stmain_dc_t, *_pstmain_dc_t;

static _stmain_dc_t gdc;
static _pstmain_dc_t gpdc = &gdc;

static u_int8_t prog_cache[0x80];
static const size_t prog_unit_size = 0x80;
static u_int64_t g_mac_addr = 0;

static int _jennic_program(char* image)
{
    _pstmain_dc_t pdc = gpdc;
    stzb_firmware_t firm;
    struct stat fstate;
    int ii = 0;

    int image_block_num = 0;
    int image_block_left = 0;
    u_int32_t flash_addr = 0x00000000;
    u_int8_t* image_addr = NULL;

    pdc->fw_fd = open(image, O_RDONLY);

    if(0 == pdc->fw_fd)
    {
        return -1;
    }

    // get image size
    if(-1 == fstat(pdc->fw_fd, &fstate))
    {
        perror("Could not stat file! \n");
        return -1;
    }

    firm.u32ImageLength = (u_int32_t)fstate.st_size;

    pdc->fw_image = mmap(NULL, fstate.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, pdc->fw_fd, 0);

    if(MAP_FAILED == pdc->fw_image)
    {
        perror("could not map file! \n");
        return -1;
    }

    jennic_pring_fw_info(pdc->fw_image, &firm);

    image_addr = firm.pu8ImageData;
    image_block_num = fstate.st_size/prog_unit_size;
    image_block_left = fstate.st_size%prog_unit_size;

    printf("\n");

    for(ii=0; ii<image_block_num; ii++)
    {
        jennic_write_flash(flash_addr, (u_int8_t)prog_unit_size, image_addr);
        flash_addr += prog_unit_size;
        image_addr += prog_unit_size;
        printf("%c[Aprograming progress %d%% \n",0x1B, ii*100/image_block_num);
    }

    jennic_write_flash(flash_addr, (u_int8_t)image_block_left, image_addr);
    printf("%c[Aprograming progress 100%% \n", 0x1B);

    munmap(pdc->fw_image, firm.u32ImageLength);
    close(pdc->fw_fd);

    return 0;
}

static int _jennic_verify(pstzb_firmware_t pfirm)
{

    return 0;
}


static int _util_serial_init(void)
{
    //serial wrapper init
    stjn_wrapper_t wrapper =
    {
        .init = jennic_serial_init,
        .prepare = jennic_serial_prepare,
        .talk = jennic_serial_talk,
        .fini = jennic_serial_fini,
    };

    jennic_wrapper_init(&wrapper);

    return 0;
}

static int _util_ftdi_init(void)
{
    _pstmain_dc_t pdc = gpdc;
    stjn_wrapper_t wrapper =
    {
        .init = jennic_ftdi_init,
        .prepare = jennic_ftdi_prepare,
        .talk = jennic_ftdi_talk,
        .fini = jennic_ftdi_fini,
    };

    jennic_wrapper_init(&wrapper);

    jennic_init(pdc->reset_io, pdc->spimiso_io);
    // select flash
    jennic_select_flash();
    return 0;
}


static void _util_usage(char *argv[])
{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    fprintf(stderr, "  Arguments:\n");
    fprintf(stderr, "    -c --connection    <type>          connection implementation (ftdi, serial, ipv6), Default: serial. \n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -a --address       <address>       start reading at address, Default at 0x00000000. \n");
    fprintf(stderr, "    -l --len           <length>        number of bytes to read, Default is 192000. \n");
    fprintf(stderr, "    -V --verbosity     <verbosity>     Verbosity level. Increses amount of debug information. Default 0. \n");
    fprintf(stderr, "    -I --initialbaud   <rate>          Set initial baud rate. \n");
    fprintf(stderr, "    -P --programbaud   <rate>          Set programming baud rate. \n");
    fprintf(stderr, "    -f --firmware      <firmware>      Load module flash with the given firmware file. \n");
    fprintf(stderr, "    -v --verify                        Verify image. If specified, verify the image programmed was loaded correctly.\n");
    fprintf(stderr, "    -m --mac           <MAC Address>   Set MAC address of device. If this is not specified, the address is read from flash. \n");
    fprintf(stderr, "    -s --show                          show mac address, chip id. \n");
    fprintf(stderr, "    -R --reset-pin     <PIN Number>    pin number of reset line for ftdi-type connections (default=6). \n");
    fprintf(stderr, "    -S --spimiso-pin   <PIN Number>    pin number of spimiso line for ftdi-type connections (default=7). \n");
    exit(EXIT_FAILURE);
}

static void _util_options_parse(int argc, char *argv[])
{
    _pstmain_dc_t pdc = gpdc;
    signed char opt;
    int option_index;

    static struct option long_options[] =
    {
        {"help",                    no_argument,        NULL,       'h'},
        {"connection",              required_argument,  NULL,       'c'},
        {"address",                 required_argument,  NULL,       'a'},
        {"len",                     required_argument,  NULL,       'l'},
        {"verbosity",               required_argument,  NULL,       'V'},
        {"initialbaud",             required_argument,  NULL,       'I'},
        {"programbaud",             required_argument,  NULL,       'P'},
        {"show",                    no_argument,        NULL,       's'},
        {"firmware",                required_argument,  NULL,       'f'},
        {"verify",                  no_argument,        NULL,       'v'},
        {"mac",                     required_argument,  NULL,       'm'},
        {"reset-pin",               required_argument,  NULL,       'R'},
        {"spimiso-pin",             required_argument,  NULL,       'S'},
        { NULL, 0, NULL, 0}
    };

    memset(pdc, 0, sizeof(_stmain_dc_t));
    pdc->spimiso_io = -1;
    pdc->reset_io = -1;

    while((opt = getopt_long(argc, argv, "hc:al:f:vI:P:m:s", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
            case 0:
            case 'h':
                _util_usage(argv);
                break;
            case 'c':
                pdc->connection = optarg;
                break;
            case 'f':
                pdc->firmware = optarg;
                break;
            case 'V':
                pdc->verbosity = atoi(optarg);
                break;
            case 'v':
                pdc->verify_flag = 1;
                break;
            case 's':
                pdc->show_flag = 1;
                break;
            case 'I':
            {
                char *pcEnd;
                errno = 0;
                pdc->init_baudrate = strtoul(optarg, &pcEnd, 0);
                if(errno)
                {
                    printf("Initial baud rate '%s' cannot be converted to 32 bit integer (%s) \n",
                           optarg, strerror(errno));
                    _util_usage(argv);
                }
                if (*pcEnd != '\0')
                {
                    printf("Initial baud rate '%s' contains invalid characters \n", optarg);
                    _util_usage(argv);
                }
                break;
            }
            case 'P':
            {
                char *pcEnd;
                errno = 0;
                pdc->prog_baudrate = strtoul(optarg, &pcEnd, 0);
                if (errno)
                {
                    printf("Program baud rate '%s' cannot be converted to 32 bit integer (%s)\n",
                           optarg, strerror(errno));
                    _util_usage(argv);
                }
                if (*pcEnd != '\0')
                {
                    printf("Program baud rate '%s' contains invalid characters\n", optarg);
                    _util_usage(argv);
                }
                break;
            }
            case 'm':
                g_mac_addr = strtoll(optarg, (char **) NULL, 16);
                pdc->pmac_addr = &g_mac_addr;
                break;
            case 'R':
                pdc->reset_io = atoi(optarg);
                if(pdc->reset_io < -1 || pdc->reset_io >= 8)
                {
                    printf("Reset io number '%d' is invalid! it varies from 0-7! \n", pdc->reset_io);
                    exit(-1);
                }
                break;
            case 'S':
                pdc->spimiso_io = atoi(optarg);
                if(pdc->spimiso_io < -1 || pdc->spimiso_io >= 8)
                {
                    printf("Spimiso io number '%d' is invalid! it varies from 0-7! \n", pdc->spimiso_io);
                    exit(-1);
                }
                break;

            default: /* '?' */
                _util_usage(argv);
        }
    }

    pdc->reset_io = (-1 == pdc->reset_io) ? 6 : pdc->reset_io;
    pdc->spimiso_io = (-1 == pdc->spimiso_io) ? 7 : pdc->spimiso_io;
}

static int _util_options_process(char *argv[])
{
    _pstmain_dc_t pdc = gpdc;

    if(NULL == pdc->connection)
    {
        _util_usage(argv);
    }
    else
    {
        if(0 == strcmp(pdc->connection, "serial"))
        {
            _util_serial_init();
        }
        else if(0 == strcmp(pdc->connection, "ftdi"))
        {
            _util_ftdi_init();
        }
        else if(0 == strcmp(pdc->connection, "ipv6"))
        {
            printf("ipv6 has not been implemented! \n");
            exit(0);
        }
        else
        {
            _util_usage(argv);
        }
    }

    if(1 == pdc->show_flag)
    {
        u_int8_t mac[8];
        u_int32_t chipid;
        // get chip id
        jennic_get_chip_id(&chipid);
        printf("Chip ID: %08x \n", chipid);

        // read mac
        jennic_read_mac(mac, JN_CHIP_JN516X, 0);
        util_debug_buf("default mac", mac, sizeof(mac));
        jennic_read_mac(mac, JN_CHIP_JN516X, 1);
        util_debug_buf("user mac", mac, sizeof(mac));
        exit(0);
    }

    if(NULL != pdc->firmware)
    {
        if(-1 != access(pdc->firmware,F_OK))
        {
            // erase flash
            jennic_erase_flash();
            // program flash
            _jennic_program(pdc->firmware);
        }
        else
        {
            printf("Firmware %s doesn't exists! \n", pdc->firmware);
            exit(-1);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    _util_options_parse(argc, argv);
    return _util_options_process(argv);
}
