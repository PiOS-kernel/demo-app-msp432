#include "motion_detection.h"

EventHandle ADCevent;
PIPE* displayPipe;

TaskHandle orientationHandle;
TaskHandle displayHandle;

Graphics_Context g_sContext;

void drawData(uint16_t x, uint16_t y, uint16_t z);


void setup_motion_detection()
{
    displayPipe = NEW_PIPE(1,motion_data_msg);

    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    kcreate_task(taskOrientation, 0, 2, &orientationHandle);
    kcreate_task(taskDisplay, 0, 1, &displayHandle);
}

void exit_motion_detection(){
    /* Halt conversion */
    ADC14_disableConversion();

    kill(orientationHandle);
    kill(displayHandle);

    Graphics_clearDisplay(&g_sContext);

}

void taskOrientation(){
    uint16_t ADCresults[3];
    uint8_t Lcd_Orientation = 5; // 5 is an invalid value
    motion_data_msg msg;
    while(1){
        ADCresults[0] = ADC14_getResult(ADC_MEM0);
        ADCresults[1] = ADC14_getResult(ADC_MEM1);
        ADCresults[2] = ADC14_getResult(ADC_MEM2);
        
        /*
         * Compute orientation
         */
        if (ADCresults[0] < 5600)
        {
            // should be LEFT
            if (Lcd_Orientation != LCD_ORIENTATION_LEFT)
            {
                Lcd_Orientation = LCD_ORIENTATION_LEFT;
                msg.Lcd_Orientation = Lcd_Orientation;
                msg.x = ADCresults[0];
                msg.y = ADCresults[1];
                msg.z = ADCresults[2];
                pub_msg(displayPipe, (void*) &msg);
            }
        }
        else if (ADCresults[0] > 10400)
        {
            // should be RIGHT
            if (Lcd_Orientation != LCD_ORIENTATION_RIGHT)
            {
                Lcd_Orientation = LCD_ORIENTATION_RIGHT;
                msg.Lcd_Orientation = Lcd_Orientation;
                msg.x = ADCresults[0];
                msg.y = ADCresults[1];
                msg.z = ADCresults[2];
                pub_msg(displayPipe, (void*) &msg);
            }
        }
        else if (ADCresults[1] < 5600)
        {
            // should be UP
            if (Lcd_Orientation != LCD_ORIENTATION_UP)
            {
                Lcd_Orientation = LCD_ORIENTATION_UP;
                msg.Lcd_Orientation = Lcd_Orientation;
                msg.x = ADCresults[0];
                msg.y = ADCresults[1];
                msg.z = ADCresults[2];
                pub_msg(displayPipe, (void*) &msg);
            }
        }
        else if (ADCresults[1] > 10400)
        {
            // should be DOWN
            if (Lcd_Orientation != LCD_ORIENTATION_DOWN)
            {
                Lcd_Orientation = LCD_ORIENTATION_DOWN;
                msg.Lcd_Orientation = Lcd_Orientation;
                msg.x = ADCresults[0];
                msg.y = ADCresults[1];
                msg.z = ADCresults[2];
                pub_msg(displayPipe, (void*) &msg);
            }
        }
        else{
            // nothing to do
        }
    }
}

void taskDisplay(){
    motion_data_msg msg;
    while(1){
        read_msg(displayPipe, (void*) &msg);
        Crystalfontz128x128_SetOrientation(msg.Lcd_Orientation);
        drawData(msg.x, msg.y, msg.z);
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
    string[0] = 'X'; string[1] = ':'; string[2]=' ';
    char number[10];
    itoa(x, number);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 3, 50, 50,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) number, 5, 75, 50,
                                OPAQUE_TEXT);

    string[0] = 'Y';
    itoa(y, number);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 3, 50, 70,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) number, 5, 75, 70,
                                OPAQUE_TEXT);

    string[0] = 'Z';
    itoa(z, number);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 3, 50, 90,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) number, 5, 75, 90,
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
