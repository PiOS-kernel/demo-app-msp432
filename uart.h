#ifndef __UART_H__
#define __UART_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void uart_setup(void);
void serial_println(char* string_to_print);
void serial_print(char* string_to_print);
void serial_print_int(int n);



#endif
