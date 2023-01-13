#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Kernel/kernel/kernel.h"
#include "uart.h"

#include "led_blinking.h"


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
	// pin buttons to let the user choice the test to run
	// P35
	// P51

    uart_setup();
    _graphicsInit();

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	testEvent = NEW_EVENT(uint32_t);

	/* Buttons setup to let the user choose the test to run */

	/* P3.5 as input for button  */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);

    /* P5.1 as input for button */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);


    /* Enable interrupts on Port 3 and 5 */
    Interrupt_enableInterrupt(INT_PORT3);
	Interrupt_enableInterrupt(INT_PORT5);
    /* activate interrupt notification */
    Interrupt_enableMaster();



    /* create manager task */
    create_task((void(*)(void*)) manager, (void*)0, 8, NULL);
}

void manager(void){


//    disable_interrupts();
//    GPIO_setAsOutputPin(RED_PORT, RED_PIN);
//    GPIO_setAsOutputPin(GREEN_PORT, GREEN_PIN);
//    GPIO_setAsOutputPin(BLUE_PORT, BLUE_PIN);
//    GPIO_setOutputHighOnPin(RED_PORT, RED_PIN);
//    busy_wait(0xAFFFF);
//    GPIO_setOutputLowOnPin(RED_PORT, RED_PIN);
//    GPIO_setOutputHighOnPin(GREEN_PORT, GREEN_PIN);
//    busy_wait(0xAFFFF);
//    GPIO_setOutputLowOnPin(GREEN_PORT, GREEN_PIN);
//    GPIO_setOutputHighOnPin(BLUE_PORT, BLUE_PIN);
//    busy_wait(0xAFFFF);
//    GPIO_setOutputLowOnPin(BLUE_PORT, BLUE_PIN);
//    enable_interrupts();

//        GPIO_setAsOutputPin(RED_PORT, RED_PIN);
//        GPIO_setAsOutputPin(GREEN_PORT, GREEN_PIN);
//        GPIO_setAsOutputPin(BLUE_PORT, BLUE_PIN);
//
//    while(1){
//        GPIO_setOutputHighOnPin(GREEN_PORT, GREEN_PIN);
//        busy_wait(0xFFFF);
//        GPIO_setOutputLowOnPin(GREEN_PORT, GREEN_PIN);
//        busy_wait(0xFFF);
//    }

    serial_println("READY");


	uint32_t choice = 0;
	while(1){
		GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
		GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
		event_wait(testEvent);
		GPIO_disableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
		GPIO_disableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
		get_event_msg(testEvent, &choice);
        test_number = choice;
		disable_interrupts();
		switch (choice)
		{
		case 1:
			/* execute TEST 1: motion detection */
			//setup_motion_detection();
		    serial_println("T1\n");
			break;
		
		case 2:
			/* execute TEST 2: led blinking */
			setup_led_blinking();
		    serial_println("T2\n");
			break;
		
		default:
			break;
		}
		enable_interrupts();
		yield();

		// it will be get back here when all other tasks are killed (due to its lower priority)
		disable_interrupts();
		switch (choice)
		{
		case 1:
			/* exiting TEST 1: motion detection */
		    serial_println("Exiting T1\n");
			//exit_motion_detection();
			break;
		
		case 2:
			/* exiting TEST 2: led blinking */
		    serial_println("Exiting T2\n");
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
			// exiting from a test
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
			// exiting from a test
			event_post(testEvent, 0);
		}
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}

extern uint32_t CLOCK;

void pre_context_switch(void){
//    serial_print_int(CLOCK);
//        serial_print("\r\n");
    if(test_number == 2){
		// LED BLINKING TEST
	    GPIO_setOutputLowOnPin(RED_PORT, RED_PIN);
        GPIO_setOutputLowOnPin(GREEN_PORT, GREEN_PIN);
        GPIO_setOutputLowOnPin(BLUE_PORT, BLUE_PIN);
        uint32_t current_timing = check_duration();
		if(current_timing == 100){
		    serial_print_int(CLOCK);
		    serial_print("\r\n");
			kcreate_task((void(*)(void*)) taskGreen, (void*)0, 2, &(timeGreen.task));
		} else if (current_timing == 300)
		{
			kcreate_task((void(*)(void*)) taskRed, (void*)0, 1, &(timeRed.task));
		}
		
	}
}
