#include "util_gpio.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#if 0
#define UTIL_GPIO_DEBUG
#else
#define UTIL_GPIO_DEBUG printf
#endif

#define SYS_GPIO_EXPORT_PATH    "/sys/class/gpio/export"
#define SYS_GPIO_UNEXPORT_PATH  "/sys/class/gpio/unexport"
#define SYS_GPIO_PATH           "/sys/class/gpio"

typedef struct
{
    int fd_export;
    int fd_unexport;

    char path_buf[2048];
    int init_flag;

}_stGPIOS_T, *_pstGPIOS_T;

static _stGPIOS_T _gdc;
static _pstGPIOS_T _gpdc = &_gdc;

int gpio_ctrl_init()
{
    _pstGPIOS_T pdc = _gpdc;
    pdc->fd_export = open(SYS_GPIO_EXPORT_PATH, O_WRONLY);
    pdc->fd_unexport = open(SYS_GPIO_UNEXPORT_PATH, O_WRONLY);

    if(pdc->fd_export && pdc->fd_unexport)
    {
        pdc->init_flag = 1;
        UTIL_GPIO_DEBUG("sunxi gpio init success! \n");
        return 0;
    }

    close(pdc->fd_export);
    close(pdc->fd_unexport);

    UTIL_GPIO_DEBUG("sunxi gpio init failed! \n");

    return 0;
}

int gpio_ctrl_export(int id)
{
    char id_str[8];
    _pstGPIOS_T pdc = _gpdc;
    if(0 == pdc->init_flag)
    {
        UTIL_GPIO_DEBUG("sunxi gpio export failed! not inited! \n");
        return -1;
    }
    UTIL_GPIO_DEBUG("sunxi gpio export %d \n", id);

    snprintf(id_str, 8, "%d", id);
    write(pdc->fd_export, id_str, strlen(id_str));

    return 0;
}

int gpio_ctrl_unexport(int id)
{
    char id_str[8];
    _pstGPIOS_T pdc = _gpdc;
    if(0 == pdc->init_flag)
    {
        UTIL_GPIO_DEBUG("sunxi gpio export failed! not inited! \n");
        return -1;
    }
    UTIL_GPIO_DEBUG("sunxi gpio export %d \n", id);

    snprintf(id_str, 8, "%d", id);

    write(pdc->fd_unexport, id_str, strlen(id_str));

    return 0;
}

int gpio_ctrl_enable_out(int id)
{
    int fd = 0;

    _pstGPIOS_T pdc = _gpdc;
    if(0 == pdc->init_flag)
    {
        return -1;
    }

    UTIL_GPIO_DEBUG("sunxi gpio enable out %d \n", id);

    snprintf(pdc->path_buf, sizeof(pdc->path_buf), "%s/gpio%d/direction", SYS_GPIO_PATH, id);

    fd = open(pdc->path_buf, O_WRONLY);

    if(fd)
    {
        write(fd, "out", strlen("out"));
        close(fd);
        UTIL_GPIO_DEBUG("sunxi gpio enable out %s success! \n", pdc->path_buf);
        return 0;
    }

    UTIL_GPIO_DEBUG("sunxi gpio enable out %s failed! \n", pdc->path_buf);

    return -1;
}

int gpio_ctrl_enable_in(int id)
{
    int fd = 0;

    _pstGPIOS_T pdc = _gpdc;
    if(0 == pdc->init_flag)
    {
        return -1;
    }

    UTIL_GPIO_DEBUG("sunxi gpio enable in %d \n", id);

    snprintf(pdc->path_buf, sizeof(pdc->path_buf), "%s/gpio%d/direction", SYS_GPIO_PATH, id);

    fd = open(pdc->path_buf, O_WRONLY);

    if(fd)
    {
        write(fd, "in", strlen("in"));
        close(fd);
        UTIL_GPIO_DEBUG("sunxi gpio enable in %s success! \n", pdc->path_buf);
        return 0;
    }

    UTIL_GPIO_DEBUG("sunxi gpio enable in %s failed! \n", pdc->path_buf);


    return -1;
}

int gpio_ctrl_set_out(int id, int bon)
{
    int fd = 0;

    _pstGPIOS_T pdc = _gpdc;
    if(0 == pdc->init_flag)
    {
        return -1;
    }

    UTIL_GPIO_DEBUG("sunxi gpio set out %d to %d \n", id, bon);

    snprintf(pdc->path_buf, sizeof(pdc->path_buf), "%s/gpio%d/value", SYS_GPIO_PATH, id);

    fd = open(pdc->path_buf, O_WRONLY);

    if(fd)
    {
        write(fd, (bon ? "1" : "0"), 1);
        close(fd);
        UTIL_GPIO_DEBUG("sunxi gpio set out %s success! \n", pdc->path_buf);
        return 0;
    }

    UTIL_GPIO_DEBUG("sunxi gpio set out %s failed! \n", pdc->path_buf);

    return -1;
}

int gpio_ctrl_get_in(int id)
{
    int fd = 0;
    char buf[10];

    _pstGPIOS_T pdc = _gpdc;
    if(0 == pdc->init_flag)
    {
        return -1;
    }

    UTIL_GPIO_DEBUG("sunxi gpio get in from %d \n", id);

    snprintf(pdc->path_buf, sizeof(pdc->path_buf), "%s/gpio%d/value", SYS_GPIO_PATH, id);

    fd = open(pdc->path_buf, O_RDONLY);

    if(fd)
    {
        read(fd, buf, 10);
        close(fd);
        UTIL_GPIO_DEBUG("sunxi gpio get in %s success! \n", pdc->path_buf);
        return atoi(buf);
    }

    UTIL_GPIO_DEBUG("sunxi gpio get in %s failed! \n", pdc->path_buf);


    return -1;
}

int gpio_ctrl_fini()
{
    _pstGPIOS_T pdc = _gpdc;
    if(pdc->init_flag)
    {
        close(pdc->fd_export);
        close(pdc->fd_unexport);
        memset(pdc, 0, sizeof(*pdc));
    }
    return 0;
}
