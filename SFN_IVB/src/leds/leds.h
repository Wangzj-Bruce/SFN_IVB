/*
 * leds.h
 *
 * Created on:  05/13/2019
 *
 * Author: Alex Vanzella (alex.vanzella@traxxautomation.com)
 *
 */

#ifndef LEDS_H_
#define LEDS_H_

#define LED1  0
#define LED2  1
#define LED3  2
#define LED4  3


void leds_init(void);
void leds_set(void);
void leds_task(void);

void led_on(uint8_t led_num);
void led_off(uint8_t led_num);
void led_toggle(uint8_t led_num);
void led_blink(uint8_t led_num, uint16_t blink_ms);
void led_test_start(uint8_t led_num);
void led_test_end(uint8_t led_num);
void leds_all_on(void);
void leds_all_off(void);

#endif // LEDS_H_
