#include "util_loftq.h"
#include "util_gpio.h"
#include <unistd.h>

static char reset_io[] = "PC3";
static char spimiso_io[] = "PG16";

int util_loftq_prepare()
{
    sunxi_gpio_init();

    // export gpios
    sunxi_gpio_export(reset_io);
    sunxi_gpio_export(spimiso_io);
    // enable output
    sunxi_gpio_enable_out(reset_io);
    sunxi_gpio_enable_out(spimiso_io);

    // processing
    sunxi_gpio_set_out(reset_io, 1);
    sunxi_gpio_set_out(spimiso_io, 0);
    usleep(500000);

    sunxi_gpio_set_out(reset_io, 0);
    usleep(300000);

    sunxi_gpio_set_out(spimiso_io, 1);

    sunxi_gpio_fini();
    return 0;
}

int util_loftq_finalize()
{
    sunxi_gpio_init();
    sunxi_gpio_export(reset_io);
    sunxi_gpio_enable_out(reset_io);
    sunxi_gpio_set_out(reset_io, 0);
    sunxi_gpio_fini();
    return 0;
}
