
/*****************************************************************************/
/* PIOS_BOOT.C          - Initialize the ARM C runtime environment for PiOS  */
/*                                                                           */
/* to generate this boot routine, the source code has been modified.         */
/* Here, Texas Instruments' conditions are provided                          */
/*                                                                           */
/* Copyright (c) 2017 Texas Instruments Incorporated                         */
/* http://www.ti.com/                                                        */
/*                                                                           */
/*  Redistribution and  use in source  and binary forms, with  or without    */
/*  modification,  are permitted provided  that the  following conditions    */
/*  are met:                                                                 */
/*                                                                           */
/*     Redistributions  of source  code must  retain the  above copyright    */
/*     notice, this list of conditions and the following disclaimer.         */
/*                                                                           */
/*     Redistributions in binary form  must reproduce the above copyright    */
/*     notice, this  list of conditions  and the following  disclaimer in    */
/*     the  documentation  and/or   other  materials  provided  with  the    */
/*     distribution.                                                         */
/*                                                                           */
/*     Neither the  name of Texas Instruments Incorporated  nor the names    */
/*     of its  contributors may  be used to  endorse or  promote products    */
/*     derived  from   this  software  without   specific  prior  written    */
/*     permission.                                                           */
/*                                                                           */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS    */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT    */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT    */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT    */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     */
/*                                                                           */
/*****************************************************************************/
#include <stdint.h>
#include <stddef.h>

#ifdef __TI_RTS_BUILD
/*---------------------------------------------------------------------------*/
/* __TI_default_c_int00 indicates that the default TI entry routine is being  */
/* used.  The linker makes assumptions about what exit does when this symbol */
/* is seen. This symbols should NOT be defined if a customized exit routine  */
/* is used.                                                                  */
/*---------------------------------------------------------------------------*/
//__asm("__TI_default_c_int00 .set 1");
#endif

/*----------------------------------------------------------------------------*/
/* Define the user mode stack. The size will be determined by the linker.     */
/*----------------------------------------------------------------------------*/
//__attribute__((section(".stack")))
//int __stack;

/*----------------------------------------------------------------------------*/
/* Linker defined symbol that will point to the end of the user mode stack.   */
/* The linker will enforce 8-byte alignment.                                  */
/*----------------------------------------------------------------------------*/
extern int __STACK_END;

/*----------------------------------------------------------------------------*/
/* Function declarations.                                                     */
/*----------------------------------------------------------------------------*/
__attribute__((weak)) extern void __mpu_init(void);
extern int _system_pre_init(void);
extern void __TI_auto_init(void);
extern void _args_main(void);
extern void __attribute__((noreturn)) exit(int);

extern int SystemCoreClock;

#ifndef __ARM_FP
#define NEEDS_FP 0
#else
#define NEEDS_FP 1
#endif

extern void uart_setup();
extern void kernel_init();
extern void SysTick_init(int val);
extern void SysTick_enable();
extern void start_scheduler(void);
extern void setup(void);

void start();


#define systick_freq (SystemCoreClock / 100)



#define _pios_c_int00_template(NEEDS_ARGS, NEEDS_INIT)                     \
{                                                                          \
   /* Initialize the stack pointer */                                      \
   __set_MSP((uint32_t)&__STACK_END);                                      \
                                                                           \
   /* Initialize the FPU if building for floating point */                 \
   if (NEEDS_FP)                                                           \
   {                                                                       \
      register volatile uint32_t* cpacr = (volatile uint32_t*)0xE000ED88;  \
      *cpacr |= (0xf0 << 16);                                              \
   }                                                                       \
                                                                           \
   __mpu_init();                                                           \
   if (_system_pre_init())                                                 \
   {                                                                       \
      if (NEEDS_INIT)                                                      \
         __TI_auto_init();                                                 \
   }                                                                       \
}


void start(){

    // UART SETUP
    uart_setup();

    // __asm__("MRS R0, CONTROL\n\t");
    // __asm__("MOV R0, #0");
    // __asm__("MSR CONTROL, R0");
    kernel_init();
    setup();
    // setup SysTick
    SysTick_init(systick_freq); // +- 1ms
    SysTick_enable();
    start_scheduler();

   // non dovrebbe mai raggiungere questa exit perchè task_switch
   // mette in esecuzione le tasks

   while(1);
}

__attribute__((section(".text:_pios_c_int00"), used, naked))
void _pios_c_int00(void)
{
    _pios_c_int00_template(0, 1);
    start();

}

__attribute__((section(".text:_pios_c_int00_noargs"), used, naked))
void _pios_c_int00_noargs(void)
{
    _pios_c_int00_template(0, 1);
    start();
}

__attribute__((section(".text:_pios_c_int00_noinit"), used, naked))
void _pios_c_int00_noinit(void)
{
    _pios_c_int00_template(0, 0);
    start();
}

__attribute__((section(".text:_pios_c_int00_noinit_noargs"), used, naked))
void _pios_c_int00_noinit_noargs(void)
{
    _pios_c_int00_template(0, 0);
    start();
}

/* defined here just because TI compiler looks for the main function */
void main(void){}

void HardFault_Handler(){
    serial_println("Hard fault");
    while(1);
}

