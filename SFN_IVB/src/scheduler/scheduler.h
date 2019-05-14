/*
 * scheduler.h
 *
 * Created on:  05/13/2019
 *
 * Author: Alex Vanzella (alex.vanzella@traxxautomation.com)
 *
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

void    scheduler_init(void);
void    scheduler_start(void);
void    scheduler_stop(void);

int8_t  schedule_task(void(*task)(void), uint16_t msec);
int8_t  reschedule_task(void(*task)(void), int8_t handle, uint16_t msec);
int8_t  unschedule_task(void(*task)(void), int8_t handle);
void    unschedule_all_tasks(void);

void    execute_tasks(void);

#endif // SCHEDULER_H_
