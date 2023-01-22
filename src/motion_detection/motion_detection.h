#ifndef __MOTION_DETECTION_H__
#define __MOTION_DETECTION_H__

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "../utils/uart.h"
#include "Kernel/kernel/kernel.h"

typedef struct {
    uint8_t Lcd_Orientation;
    uint16_t x;
    uint16_t y;
    uint16_t z;
} motion_data_msg;

void setup_motion_detection();
void exit_motion_detection();

void taskOrientation();
void taskDisplay();

void _graphicsInit();
void _accelSensorInit();

#endif
