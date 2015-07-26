#ifndef UTIL_GPIO_H
#define UTIL_GPIO_H

int gpio_ctrl_init();
int gpio_ctrl_export(int io);
int gpio_ctrl_unexport(int io);
int gpio_ctrl_enable_out(int io);
int gpio_ctrl_enable_in(int io);
int gpio_ctrl_set_out(int io, int bon);
int gpio_ctrl_get_in(int io);
int gpio_ctrl_fini();

#endif // UTIL_GPIO_H
