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
    // TASK_TIME_UNITS = 100;
    starting_time = get_clock();
    serial_print_int(starting_time);
    serial_print("\n");
    mutex = mutex_init();
    create_task((void(*)(void*)) taskBlue, (void*)0, 3, &(timeBlue.task));
}

void exit_led_blinking(){
    // TASK_TIME_UNITS = 10;
}

uint32_t check_duration(){
    // if(get_my_taskHandle() == timeRed.task){
    //     timeRed.time++;
    //     if(timeRed.time >= DURATION){
    //         timeRed.exit = 1;
    //     }
    // } else if(get_my_taskHandle() == timeGreen.task){
    //     timeGreen.time++;
    //     if(timeGreen.time >= DURATION){
    //         timeGreen.exit = 1;
    //     }
    // } else if(get_my_taskHandle() == timeBlue.task){
    //     timeBlue.time++;
    //     if(timeBlue.time >= DURATION){
    //         timeBlue.exit = 1;
    //     }
    // }
    return get_clock() - starting_time;
}


// void taskRed(){
//     timeRed.exit = 0;
//     synch_wait(mutex);
//     while(1){
//         if(timeRed.exit == 1){
//             GPIO_setOutputLowOnPin(RED_PORT, RED_PIN);
//             synch_post(mutex);
//             task_exit();
//         }
//         GPIO_toggleOutputOnPin(RED_PORT, RED_PIN);
//         busy_wait(WAITING);
//     }
// }

// void taskGreen(){
//     timeGreen.exit = 0;
//     while(1){
//         if(timeGreen.exit == 1){
//             GPIO_setOutputLowOnPin(GREEN_PORT, GREEN_PIN);
//             task_exit();
//         }
//         GPIO_toggleOutputOnPin(GREEN_PORT, GREEN_PIN);
//         busy_wait(WAITING);
//     }
// }

// void taskBlue(){
//     timeBlue.exit = 0;
//     synch_wait(mutex);
//     while(1){
//         if(timeBlue.exit == 1){
//             GPIO_setOutputLowOnPin(BLUE_PORT, BLUE_PIN);
//             synch_post(mutex);
//             task_exit();
//         }
//         GPIO_toggleOutputOnPin(BLUE_PORT, BLUE_PIN);
//         busy_wait(WAITING);
//     }
// }

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
