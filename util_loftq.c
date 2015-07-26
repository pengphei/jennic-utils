#include "util_loftq.h"
#include "util_gpio.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

typedef struct
{
    char base;
    u_int16_t digi_base;
}_stGPIO_MAP_T, *_pstGPIO_MAP_T;

static char reset_io[] = "PC3";
static char spimiso_io[] = "PG16";
static int reset_idx = -1;
static int spimiso_idx = -1;

static _stGPIO_MAP_T a31_maps[] =
{
    {'A', 0},
    {'B', 30},
    {'C', 40},
    {'D', 70},
    {'E', 100},
    {'F', 119},
    {'G', 127},
    {'H', 148},
    {'L', 181},
    {'M', 192},
};

// gpio maping for A31
static int _gpio_ctrl_map_a31(char zone, int idx)
{
    if( zone >= 'a' && zone <= 'z')
    {
        zone = 'A' + zone - 'a';
    }

    if(zone >= 'A' && zone <= 'Z')
    {
        int ii = 0;
        for(ii=0; ii<sizeof(a31_maps)/sizeof(a31_maps[0]); ii++)
        {
            if(a31_maps[ii].base == zone)
            {
                return a31_maps[ii].digi_base + idx;
            }
        }
    }

    return -1;
}

int util_loftq_prepare()
{
    reset_idx = _gpio_ctrl_map_a31(reset_io[1], atoi(&reset_io[2]));
    spimiso_idx = _gpio_ctrl_map_a31(spimiso_io[1], atoi(&spimiso_io[2]));
    gpio_ctrl_init();

    // export gpios
    gpio_ctrl_export(reset_idx);
    gpio_ctrl_export(spimiso_idx);
    // enable output
    gpio_ctrl_enable_out(reset_idx);
    gpio_ctrl_enable_out(spimiso_idx);

    // processing
    gpio_ctrl_set_out(reset_idx, 1);
    gpio_ctrl_set_out(spimiso_idx, 0);
    usleep(500000);

    gpio_ctrl_set_out(reset_idx, 0);
    usleep(300000);

    gpio_ctrl_set_out(spimiso_idx, 1);

    gpio_ctrl_fini();
    return 0;
}

int util_loftq_finalize()
{
    gpio_ctrl_init();
    gpio_ctrl_export(reset_idx);
    gpio_ctrl_enable_out(reset_idx);
    gpio_ctrl_set_out(reset_idx, 0);
    gpio_ctrl_fini();
    return 0;
}
