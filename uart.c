#include "uart.h"

const eUSCI_UART_ConfigV1 uartConfig =
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
    39,
    1,
    0,
    EUSCI_A_UART_NO_PARITY,                  // No Parity
    EUSCI_A_UART_LSB_FIRST,                  // LSB First
    EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
    EUSCI_A_UART_MODE,                       // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

void uart_setup(){
    /* Selecting P1.2 and P1.3 in UART mode */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

        /*
         * Setting DCO to 6MHz
         * This modifies MCLK and SMCLK frequency and set it to 6MHz
         */
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_6);

        /* Configuring UART Module */
        UART_initModule(EUSCI_A0_BASE, &uartConfig);

        /* Enable UART module */
        UART_enableModule(EUSCI_A0_BASE);

        /* Enabling interrupts */
        UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        Interrupt_enableInterrupt(INT_EUSCIA0);
}

void serial_println(char* string_to_print){
    int i = 0;
    uint8_t c;
    i = 0;
    c = string_to_print[i];
    while(c != '\0'){
        UART_transmitData(EUSCI_A0_BASE, c);
        ++i;
        c = string_to_print[i];
    }
    UART_transmitData(EUSCI_A0_BASE, '\n');
    UART_transmitData(EUSCI_A0_BASE, '\r');
}

void serial_print(char* string_to_print){
    int i = 0;
    uint8_t c;
    i = 0;
    c = string_to_print[i];
    while(c != '\0'){
        UART_transmitData(EUSCI_A0_BASE, c);
        ++i;
        c = string_to_print[i];
    }
}

void itoa(int n, char* dst) {
    int i = 0;
    int sign = n;
    if (sign < 0) {
        n = -n;
    }
    do {
        dst[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0) {
        dst[i++] = '-';
    }
    dst[i] = '\0';
    int j = 0;
    for (j = 0; j < i / 2; j++) {
        char temp = dst[j];
        dst[j] = dst[i - j - 1];
        dst[i - j - 1] = temp;
    }
}

void serial_print_int(int n){
    char number[10];
    itoa(n, number);
    serial_print(number);
}
