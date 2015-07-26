#include "util_globot.h"
#include "util_gpio.h"
#include <stdlib.h>
#include <unistd.h>

static char reset_io[] = "PB6";
static char spimiso_io[] = "PB2";
static int reset_idx = -1;
static int spimiso_idx = -1;

// gpio mapping for A1x, A20, A23/A33
static int _gpio_ctrl_map(char zone, int idx)
{
    if( zone >= 'a' && zone <= 'z')
    {
        return (zone-'a')*32 + idx;
    }

    if(zone >= 'A' && zone <= 'Z')
    {
        return (zone-'A')*32 + idx;
    }

    return -1;
}

int util_globot_prepare()
{
    reset_idx = _gpio_ctrl_map(reset_io[1], atoi(&reset_io[2]));
    spimiso_idx = _gpio_ctrl_map(spimiso_io[1], atoi(&spimiso_io[2]));

    gpio_ctrl_init();

    // export gpios
    gpio_ctrl_export(reset_idx);
    gpio_ctrl_export(spimiso_idx);
    // enable output
    gpio_ctrl_enable_out(reset_idx);
    gpio_ctrl_enable_out(spimiso_idx);

    // processing
    gpio_ctrl_set_out(reset_idx, 0);
    gpio_ctrl_set_out(spimiso_idx, 0);
    usleep(500000);

    gpio_ctrl_set_out(reset_idx, 1);
    usleep(300000);
    gpio_ctrl_set_out(spimiso_idx, 1);

    gpio_ctrl_fini();

    return 0;
}

int util_globot_finalize()
{
    gpio_ctrl_init();
    gpio_ctrl_export(reset_idx);
    gpio_ctrl_enable_out(reset_idx);
    gpio_ctrl_set_out(reset_idx, 0);
    gpio_ctrl_fini();
    return 0;
}

