#ifndef __LED_BLINKIMNG_H__
#define __LED_BLINKIMNG_H__

#include "Kernel/kernel/kernel.h"
#include "stdint.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define RED_PORT GPIO_PORT_P2
#define RED_PIN GPIO_PIN6
#define GREEN_PORT GPIO_PORT_P2
#define GREEN_PIN GPIO_PIN4
#define BLUE_PORT GPIO_PORT_P5
#define BLUE_PIN GPIO_PIN6

#define WAITING 0xFFFF

#define DURATION 7000 / TASK_TIME_UNITS

typedef struct{
	TaskHandle task;
	uint32_t time;
    uint8_t exit;
}Timing_t;

void taskRed();
void taskGreen();
void taskBlue();

void led_init();
void setup_led_blinking();
void exit_led_blinking();
uint32_t check_duration();

#endif
