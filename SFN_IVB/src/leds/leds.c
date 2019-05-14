/*
 * leds.c
 *
 * Created on:  05/13/2019
 *
 * Author: Alex Vanzella (alex.vanzella@traxxautomation.com)
 *
 */

#include "common.h"

typedef enum _led_state {
    off   = 0,
	on    = 1,
	blink = 2,	
} led_state_t;

typedef struct _led_info {
    ioport_port_t port;
	uint8_t       invert;
	led_state_t   state;
    led_state_t   save_state;
	uint16_t      blink_ticks;
	uint16_t      blink_countdown;
} led_info_t;

static void led_on_priv(led_info_t *lp);
static void led_off_priv(led_info_t *lp);
static void led_toggle_priv(led_info_t *lp);
static void led_blink_priv(led_info_t *lp);

#define LED_TASK_TICK_MS (uint16_t)100
static volatile int8_t   leds_task_handle = -1;

#if (BOARD_ID == ARDUINO_DUE_BOARD_ID)
#define NUM_LEDS           3

static led_info_t leds_list[NUM_LEDS] = {
    {   // led 1, Orange
        LED0_GPIO,            // port
        FALSE,                // invert output
        off,                  // state
        off,                  // save state 
        0,                    // blink ticks
        0,                    // blink countdown
    },
    {   // led 2, Green
        LED1_GPIO,            // port
        TRUE,                 // invert output
        off,                  // state
        off,                  // save state
        0,                    // blink ticks
        0,                    // blink countdown
    },
    {   // led 3, Orange
        LED2_GPIO,            // port
        TRUE,                 // invert output
        off,                  // state
        off,                  // save state
        0,                    // blink ticks
        0,                    // blink countdown
    },
};
#endif // (BOARD_ID == ARDUINO_DUE_BOARD_ID)

#if (BOARD_ID == IVB_BOARD_ID)

#define NUM_LEDS           unknown;

#endif // (BOARD_ID == IVB_BOARD_ID)

void leds_init(void) {
    // turn off all leds
	for (int i=0; i<NUM_LEDS; i++) {
        led_off(i);
	}

    // schedule the leds task
	leds_task_handle = schedule_task(leds_task, LED_TASK_TICK_MS);
}

void led_on(uint8_t led_num) {
    if ( led_num < 0 || led_num >= NUM_LEDS ) {
	    return;
    }
     
    led_info_t *lp = &(leds_list[led_num]);
	lp->state = on;
	
	led_on_priv(lp); 
}

void led_off(uint8_t led_num) {
    if ( led_num < 0 || led_num >= NUM_LEDS ) {
		return;
	}
	
    led_info_t *lp = &(leds_list[led_num]);
	lp->state = off;
	
    led_off_priv(lp);
}

void led_toggle(uint8_t led_num) {
    if ( led_num < 0 || led_num >= NUM_LEDS ) {
		return;
	}

    led_info_t *lp = &(leds_list[led_num]);
	
	if ( blink == lp->state ) {
	    return;
	}
	
	if ( on == lp->state ) {
		lp->state = off;
	} else {
		lp->state = on;
	}
	
    led_toggle_priv(lp);
    
}

void led_blink(uint8_t led_num, uint16_t blink_ms) {
    if ( led_num < 0 || led_num >= NUM_LEDS ) {
		return;
	}

    if ( blink_ms < LED_TASK_TICK_MS ) {
		blink_ms = LED_TASK_TICK_MS;
	}
    led_info_t *lp = &(leds_list[led_num]);
	lp->state           = blink;
	lp->blink_ticks     = blink_ms / LED_TASK_TICK_MS;
	lp->blink_countdown = lp->blink_ticks;
	
	led_on_priv(lp); // start with led on
}

void led_test_start(uint8_t led_num) {
    if ( led_num < 0 || led_num >= NUM_LEDS ) {
		return;
	}
	
	led_info_t *lp = &(leds_list[led_num]);
	lp->save_state = lp->state;
    switch ( lp->state ) {
	    case on :
		case blink:
            lp->state = off;
		    led_off_priv(lp);
		    break;
		default :
            lp->state = on;
	        led_on_priv(lp);
			break;
    }
}

void led_test_end(uint8_t led_num) {
	if ( led_num < 0 || led_num >= NUM_LEDS ) {
		return;
	}
	
	led_info_t *lp = &(leds_list[led_num]);
	lp->state = lp->save_state;

	switch ( lp->state ) {
		case on :
			led_on_priv(lp);
			break;
		case off :
			led_off_priv(lp);
			break;
		case blink :
			led_blink_priv(lp);
			break;
	}
}

static void led_on_priv(led_info_t *lp) {
    bool level = (TRUE == lp->invert ? 0 : 1);
    ioport_set_pin_level(lp->port, level );
}

static void led_off_priv(led_info_t *lp) {
    bool level = (TRUE == lp->invert ? 1 : 0);
    ioport_set_pin_level(lp->port, level );
}

static void led_toggle_priv(led_info_t *lp) {
    ioport_toggle_pin_level(lp->port);
}

static void led_blink_priv(led_info_t *lp) {
	lp->blink_countdown = lp->blink_ticks;
	
	led_on_priv(lp); // start with led on
}

void leds_task(void) {
	led_info_t *lp = &(leds_list[0]);
    for (int i = 0; i<NUM_LEDS; i++,lp++) {
		if ( blink == lp->state) {
			lp->blink_countdown--;
			if ( 0 == lp->blink_countdown ) {
				lp->blink_countdown = lp->blink_ticks;
				led_toggle_priv(lp);
			}
		}
	}
}