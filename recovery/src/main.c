/**
 * @file main.c
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <zephyr/kernel.h>

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	return 0;
}
