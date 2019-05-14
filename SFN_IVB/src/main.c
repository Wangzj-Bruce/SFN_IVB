/*
 * main.c
 *
 * Created on:  05/13/2019
 *
 * Author: Alex Vanzella (alex.vanzella@traxxautomation.com)
 *
 */

#include "common.h"

void platform_init(void);
void tests_init(void);

#define IRQ_PRIOR_PIO                0  // IRQ priority for PIO
volatile uint32_t system_clock_hz  = 0;

void platform_init(void)
{
	sysclk_init();
	system_clock_hz = sysclk_get_cpu_hz();
	board_init();
	scheduler_init();
    leds_init();

	scheduler_start();
}

void tests_init(void)
{  
    led_blink(LED1, 200);
    led_blink(LED2, 500);
    led_blink(LED3, 1000);
}

int main(void)
{
    platform_init();

    tests_init();

	while (1) {
        execute_tasks();
	}
}