/*
 * common.h
 *
 * Created on:  05/13/2019
 *
 * Author: Alex Vanzella (alex.vanzella@traxxautomation.com)
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "../leds/leds.h"
#include "../scheduler/scheduler.h"

#define UNKNOWN_BOARD_ID     0
#define ARDUINO_DUE_BOARD_ID 1
#define IVB_BOARD_ID         2

#define BOARD_ID ARDUINO_DUE_BOARD_ID
//#define BOARD_ID IVB_BOARD_ID

#ifndef BOARD_ID
#define BOARD_ID UNKNOWN_BOARD_ID
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef NULL_CHAR
#define NULL_CHAR '\0'
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif // COMMON_H_
