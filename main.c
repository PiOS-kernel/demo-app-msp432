#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Kernel/kernel/kernel.h"
#include "uart.h"

#include "led_blinking.h"
#include "motion_detection.h"


extern void _graphicsInit();
extern void draw();

uint8_t test_number = 0;

extern Timing_t timeGreen;
extern Timing_t timeRed;

EventHandle testEvent;

void busy_wait(uint32_t time){
    uint32_t i = 0;
    for(; i < time; i++){}
}

void manager(void);


/**
 * main.c
 */
void setup(void)
{
    uart_setup();
    _graphicsInit();
	_accelSensorInit();
	led_init();
	GPIO_setOutputLowOnPin(RED_PORT, RED_PIN);
	GPIO_setOutputLowOnPin(GREEN_PORT, GREEN_PIN);
	GPIO_setOutputLowOnPin(BLUE_PORT, BLUE_PIN);
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	testEvent = NEW_EVENT(uint32_t);

	/* Buttons setup to let the user choose the test to run */

	/* P3.5 as input for button  */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);

    /* P5.1 as input for button */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);

    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);

    /* Enable interrupts on Port 3 and 5 */
    Interrupt_enableInterrupt(INT_PORT3);
	Interrupt_enableInterrupt(INT_PORT5);
    /* activate interrupt notification */
    Interrupt_enableMaster();

	/* print logo */
	serial_println("\nWELCOME IN THIS DEMO - powered by");
	serial_println(" ____      _     _____     ____ ");
	serial_println("|  _ \\    (_)   |  _  |   / ___|");
	serial_println("| |_)|    | |   | | | |   \\___  ");
	serial_println("|  __/    | |   | |_| |    ___)|");
	serial_println("|_|       |_|   |_____|   |____/ ");

	serial_println("\n\nPress button 1 to start motion detection test");
	serial_println("Press button 2 to start led blinking test");

    /* create manager task */
    create_task((void(*)(void*)) manager, (void*)0, 0, NULL);
}

void manager(void){
	uint32_t choice = 0;
	while(1){
	    serial_println("Press a button and choose the test to run");
		event_wait(testEvent);
		get_event_msg(testEvent, &choice);
        test_number = choice;
		disable_interrupts();
		switch (choice)
		{
		case 1:
			/* execute TEST 1: motion detection */
			setup_motion_detection();
		    serial_println("Demo 1: Motion Detection");
		    serial_println("Rotate the device and the screen will change accordingly");
			break;
		
		case 2:
			/* execute TEST 2: led blinking */
			setup_led_blinking();
			serial_println("Demo 2: Led blinking");
            serial_println("The RGB Led will blink following a defined pattern due to the PRIORITY INHERITANCE that has been implemented");
			break;
		
		default:
			break;
		}
		enable_interrupts();
		event_wait(testEvent);
		disable_interrupts();
		switch (choice)
		{
		case 1:
			/* exiting TEST 1: motion detection */
		    //serial_println("Exiting T1\n");
			exit_motion_detection();
			break;

		case 2:
			/* exiting TEST 2: led blinking */
		    //serial_println("Exiting T2\n");
			exit_led_blinking();
			break;

		default:
			break;
		}
		test_number = 0;
		enable_interrupts();


	}
}


/* Port3 ISR */
void PORT3_IRQHandler(void)
{
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    /* clear interrupt flag (to clear pending interrupt indicator */
    if(status & GPIO_PIN5){
		/* post the event and set the message accordingly*/
		if (test_number == 0){
			// setup new test
			int choice = 2; // MOTION DETECTION TEST
			busy_wait(0x5FFFF);
			event_post(testEvent, &choice);
		} else {
		    busy_wait(0x5FFFF);
		    event_post(testEvent, 0);
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
}

/* Port5 ISR */
void PORT5_IRQHandler(void)
{
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    /* clear interrupt flag (to clear pending interrupt indicator */
    if(status & GPIO_PIN1){
       	/* post the event and set the message accordingly*/
		if (test_number == 0){
			// setup new test
			int choice = 1; // MOTION DETECTION TEST
			busy_wait(0x5FFFF);
			event_post(testEvent, &choice);
		} else {
		    busy_wait(0x5FFFF);
		    event_post(testEvent, 0);
		}
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}

extern uint32_t CLOCK;

void pre_context_switch(void){
    if(test_number == 2){
		// LED BLINKING TEST
	    GPIO_setOutputLowOnPin(RED_PORT, RED_PIN);
        GPIO_setOutputLowOnPin(GREEN_PORT, GREEN_PIN);
        GPIO_setOutputLowOnPin(BLUE_PORT, BLUE_PIN);
        uint32_t current_timing = check_duration();
		if(current_timing == 25){
			kcreate_task((void(*)(void*)) taskGreen, (void*)0, 2, &(timeGreen.task));
		} else if (current_timing == 65)
		{
			kcreate_task((void(*)(void*)) taskRed, (void*)0, 1, &(timeRed.task));
		}
		
	}
}

