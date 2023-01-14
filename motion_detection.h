#ifndef __MOTION_DETECTION_H__
#define __MOTION_DETECTION_H__

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "stdio.h"
#include "Kernel/kernel/kernel.h"
#include "stdint.h"

void setup_motion_detection();
void exit_motion_detection();

void taskOrientation();
void taskDisplay();

void _graphicsInit();
void _accelSensorInit();

#endif
