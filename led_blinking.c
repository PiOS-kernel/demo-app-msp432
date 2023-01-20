#include "led_blinking.h"

Timing_t timeRed;
Timing_t timeGreen;
Timing_t timeBlue;

extern void busy_wait(uint32_t time);
extern uint32_t TASK_TIME_UNITS;

MCB* mutex;

uint32_t starting_time;

void led_init(){
    // LED - R26 G25 B56
    GPIO_setAsOutputPin(RED_PORT, RED_PIN);
    GPIO_setAsOutputPin(GREEN_PORT, GREEN_PIN);
    GPIO_setAsOutputPin(BLUE_PORT, BLUE_PIN);
}

void setup_led_blinking(){
    starting_time = get_clock();
    mutex = mutex_init();
    create_task((void(*)(void*)) taskBlue, (void*)0, 3, &(timeBlue.task));
}

void exit_led_blinking(){
}

uint32_t check_duration(){
    return get_clock() - starting_time;
}

void taskRed(){
    synch_wait(mutex);
    uint32_t  i = 0;
    for(; i < 100; i++){
        GPIO_toggleOutputOnPin(RED_PORT, RED_PIN);
        busy_wait(WAITING);
    }
    synch_post(mutex);
    task_exit();
}

void taskGreen(){
    uint32_t  i = 0;
    for(; i < 100; i++){
        GPIO_toggleOutputOnPin(GREEN_PORT, GREEN_PIN);
        busy_wait(WAITING);
    }
    task_exit();
}

void taskBlue(){
    synch_wait(mutex);
    uint32_t  i = 0;
    for(; i < 100; i++){
        GPIO_toggleOutputOnPin(BLUE_PORT, BLUE_PIN);
        busy_wait(WAITING);
    }
    synch_post(mutex);
    task_exit();
}
