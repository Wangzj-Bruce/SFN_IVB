/*
 * scheduler.c
 *
 * Created on:  05/13/2019
 *
 * Author: Alex Vanzella (alex.vanzella@traxxautomation.com)
 *
 */

#include "common.h"

#define SCHEDULER_MAX_TASKS       10       // maximum supported number of tasks
#define SCHEDULER_FREQ_HZ         40       // scheduler interrupt interval in Hz
#define SCHEDULER_TICK_MS         25       // scheduler interrupt interval in msec = 1000 ms / SCHEDULER_FREQ_HZ

// Tasks
typedef struct _task {
	uint8_t  active;
	uint8_t  timed_out;
	uint32_t interval_ticks;
	uint32_t timed_out_tick;
	void (*task)(void);
} task_t;

static volatile task_t task_list[SCHEDULER_MAX_TASKS];
static volatile task_t *tp = NULL;
static volatile uint8_t tasks_ready = FALSE;

// Tick Counter
static volatile uint32_t  timer_tick_count  = 0;

static void task_init(int16_t index) {
    if (index < 0 || index >= SCHEDULER_MAX_TASKS ) {
        return;
    }

    tp = &(task_list[index]);

    tp->task           = NULL;
    tp->active         = FALSE;
    tp->timed_out      = FALSE;
    tp->interval_ticks = 0;
    tp->timed_out_tick = 0;
}

void scheduler_init(void) {
    for (int i=0; i<SCHEDULER_MAX_TASKS; i++) {
        task_init(i);
    }
    
	tp = NULL;
	tasks_ready = FALSE;
	timer_tick_count = 0;
	
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	pmc_enable_periph_clk(ID_TC0);         // configure PMC, use Timer Counter TC0 for scheduler

	// configure TC0 for defined scheduler frequency in Hz and trigger on RC compare
	tc_find_mck_divisor(SCHEDULER_FREQ_HZ, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC0, 0, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC0, 0, (ul_sysclk / ul_div) / SCHEDULER_FREQ_HZ);
}

void scheduler_start(void) {
	NVIC_EnableIRQ((IRQn_Type) ID_TC0);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
	tc_start(TC0, 0);
}

void scheduler_stop(void) {
    tc_disable_interrupt(TC0, 0, TC_IER_CPCS);
    tc_stop(TC0,0);
}

void execute_tasks(void) {

     if ( FALSE == tasks_ready ) {
         return;
     } else {
         tasks_ready = FALSE;
     }

     tp = &(task_list[0]);

     for (int i=0; i<SCHEDULER_MAX_TASKS; i++,tp++) {
	     if ( NULL == tp->task ) {
		     continue;
	     }

	     if ( TRUE == tp->timed_out ) {
             tp->task();
		     tp->timed_out = FALSE;
	     }
		 
		 //watchdog_reset();
     }
}

int8_t schedule_task(void(*task)(void), uint16_t msec) {
     int16_t  index  = -1;
     tp = &(task_list[0]);

     for (int i=0; i<SCHEDULER_MAX_TASKS; i++,tp++) {
         if (NULL == tp->task) {
             index = (int16_t)i;
             break;
         }
     }

     if ( -1 == index ) {
         return -1;     
     }

     if ( msec < SCHEDULER_TICK_MS ) {
         msec = SCHEDULER_TICK_MS;
     }

     tp->active         = FALSE;
     tp->timed_out      = FALSE;
     tp->interval_ticks = msec/SCHEDULER_TICK_MS;
     tp->timed_out_tick = timer_tick_count + tp->interval_ticks;
     tp->task           = task;
   
     tp->active         = TRUE;

     return index;
}

int8_t reschedule_task(void(*task)(void), int8_t handle, uint16_t msec) {
     if (handle < 0 || handle >= SCHEDULER_MAX_TASKS ) {
	     return -1;
     }

     tp = &(task_list[handle]);

     if ( NULL == tp->task || task != tp->task ) {
	     return -2;
     }

     tp->active         = FALSE;

     tp->timed_out      = FALSE;
     tp->interval_ticks = msec/SCHEDULER_TICK_MS;
     tp->timed_out_tick = timer_tick_count + tp->interval_ticks;
     tp->task           = task;
     
     tp->active         = TRUE;

     return handle;
}

int8_t unschedule_task(void(*task)(void), int8_t handle) {
     if (handle < 0 || handle >= SCHEDULER_MAX_TASKS ) {
	     return -1;
     } 

     tp = &task_list[handle];
     
     if ( NULL == tp->task || task != tp->task ) {
         return -2;
     }

     task_init(handle);

     return handle;
}

void unschedule_all_tasks(void) {
    for (int i=0; i<SCHEDULER_MAX_TASKS; i++) {
	    task_init(i);
    }
}

static void update_tasks(void) {
     tp = &task_list[0];
     
     for (int i=0; i<SCHEDULER_MAX_TASKS; i++,tp++) {
         if ( NULL == task_list[i].task ) {
             continue;
         }

	     if ( (TRUE == task_list[i].active) && (timer_tick_count == task_list[i].timed_out_tick) ) {
             task_list[i].timed_out = TRUE;
             task_list[i].timed_out_tick = timer_tick_count + task_list[i].interval_ticks;
             tasks_ready = TRUE;
         } 
     }
}

void TC0_Handler(void)
{
    volatile uint32_t ul_dummy;
    ul_dummy = tc_get_status(TC0, 0); // clear status to ack interrupt
    UNUSED(ul_dummy);                 // avoid compiler warning

	timer_tick_count++;   // count ticks
	update_tasks();
}