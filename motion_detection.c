#include "motion_detection.h"

EventHandle ADCevent;
EventHandle displayEvent;
PIPE* displayPipe;

TaskHandle orientationHandle;
TaskHandle displayHandle;

Graphics_Context g_sContext;

void drawData(uint16_t x, uint16_t y, uint16_t z);
extern int atoi(const char* str);


// IN TASK_DISPLAY, CAMBIARE EVENTO OCN PIPE

void setup_motion_detection()
{
    // ADCevent = NEW_EVENT(uint16_t[3]);
    //displayEvent = NEW_EVENT(uint8_t);
    displayPipe = NEW_PIPE(1,uint8_t);

//    /* Enabling the interrupt when a conversion on channel 2 (end of sequence)
//     *  is complete and enabling conversions */
//    ADC14_enableInterrupt(ADC_INT2);
//
//    /* Enabling Interrupts */
//    Interrupt_enableInterrupt(INT_ADC14);
//
    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    kcreate_task(taskOrientation, 0, 2, &orientationHandle);
    kcreate_task(taskDisplay, 0, 1, &displayHandle);
}

void exit_motion_detection(){
    /* Halt conversion */
    ADC14_disableConversion();

//    /* Disabling the interrupt when a conversion on channel 2 (end of sequence)
//     *  is complete and enabling conversions */
//    ADC14_disableInterrupt(ADC_INT2);
//
//    /* Disabling Interrupts */
//    Interrupt_disableInterrupt(INT_ADC14);

    kill(orientationHandle);
    kill(displayHandle);

    Graphics_clearDisplay(&g_sContext);

}

// void taskADC()
// {
//     uint16_t ADCresults[3];
//     while(1){
//         /* Store ADC14 conversion results */
//         ADCresults[0] = ADC14_getResult(ADC_MEM0);
//         ADCresults[1] = ADC14_getResult(ADC_MEM1);
//         ADCresults[2] = ADC14_getResult(ADC_MEM2);

//         /* Post ADC results to pipe */
//         event_post(ADCevent, ADCresults);
//     }
        
// }

void taskOrientation(){
    uint16_t ADCresults[3];
    uint8_t Lcd_Orientation = 5; // 5 is an invalid value
    while(1){
        ADCresults[0] = ADC14_getResult(ADC_MEM0);
        ADCresults[1] = ADC14_getResult(ADC_MEM1);
        ADCresults[2] = ADC14_getResult(ADC_MEM2);
        //event_wait(ADCevent);
        //get_event_msg(ADCevent, ADCresult);
        /*
         * Compute orientation
         */
        if (ADCresults[0] < 5600)
        {
            // should be LEFT
            if (Lcd_Orientation != LCD_ORIENTATION_LEFT)
            {
                Lcd_Orientation = LCD_ORIENTATION_LEFT;
                //event_post(displayEvent, &Lcd_Orientation);
                pub_msg(displayPipe, (void*) &Lcd_Orientation);
            }
        }
        else if (ADCresults[0] > 10400)
        {
            // should be RIGHT
            if (Lcd_Orientation != LCD_ORIENTATION_RIGHT)
            {
                Lcd_Orientation = LCD_ORIENTATION_RIGHT;
                //event_post(displayEvent, &Lcd_Orientation);
                pub_msg(displayPipe, (void*) &Lcd_Orientation);
            }
        }
        else if (ADCresults[1] < 5600)
        {
            // should be UP
            if (Lcd_Orientation != LCD_ORIENTATION_UP)
            {
                Lcd_Orientation = LCD_ORIENTATION_UP;
                //event_post(displayEvent, &Lcd_Orientation);
                pub_msg(displayPipe, (void*) &Lcd_Orientation);
            }
        }
        else if (ADCresults[1] > 10400)
        {
            // should be DOWN
            if (Lcd_Orientation != LCD_ORIENTATION_DOWN)
            {
                Lcd_Orientation = LCD_ORIENTATION_DOWN;
                //event_post(displayEvent, &Lcd_Orientation);
                pub_msg(displayPipe, (void*) &Lcd_Orientation);
            }
        }
        else{
            // nothing to do
        }
    }
}

void taskDisplay(){
    uint8_t Lcd_Orientation;
    while(1){
        //event_wait(displayEvent);
        //get_event_msg(displayEvent, &Lcd_Orientation);
        read_msg(displayPipe, (void*) &Lcd_Orientation);
        Crystalfontz128x128_SetOrientation(Lcd_Orientation);
        drawData(1,2,3);
    }
}


/* ------------- Display and initialization utilities ------------- */

void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

}

/*
 * Redraw accelerometer data
 */
void drawAccelData(uint16_t x, uint16_t y, uint16_t z)
{
    char string[10];
    sprintf(string, "X: %5d", x);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 8, 64, 50,
    OPAQUE_TEXT);

    sprintf(string, "Y: %5d", y);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 8, 64, 70,
    OPAQUE_TEXT);

    sprintf(string, "Z: %5d", z);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 8, 64, 90,
    OPAQUE_TEXT);

}

/*
 * Clear display and redraw title + accelerometer data
 */
void drawData(uint16_t x, uint16_t y, uint16_t z)
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Accelerometer:",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);
    drawAccelData(x,y,z);
}

void _accelSensorInit()
{
    /* Configures Pin 4.0, 4.2, and 6.1 as ADC input */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN0 | GPIO_PIN2,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    /* Initializing ADC (ADCOSC/64/8) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 (A11, A13, A14)  with no repeat)
     * with internal 2.5v reference */
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
    ADC14_configureConversionMemory(ADC_MEM0,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(ADC_MEM1,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(ADC_MEM2,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);


    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

}
