/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Cçonfigurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xscutimer.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_io.h"
#include <time.h>
#include <stdlib.h>
#include <xtime_l.h>

#define INTERRUPT_COUNT_TIMEOUT_VALUE 1
int GLOBAL = 0;
// Function prototypes
static void my_timer_interrupt_handler(void *CallBackRef);
// Global variables
int InterruptCounter = 0;


void print_regs(){
	static unsigned int dump_regs[16] = { 0x0F };
    int i;

	__asm __volatile__("push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr, sp, pc}");
	__asm __volatile__ ("mov %[result], r0" : [result] "=r" (dump_regs[0]):);
	__asm __volatile__ ("mov %[result], r1" : [result] "=r" (dump_regs[1]):);
	__asm __volatile__ ("mov %[result], r2" : [result] "=r" (dump_regs[2]):);
	__asm __volatile__ ("mov %[result], r3" : [result] "=r" (dump_regs[3]):);
	__asm __volatile__ ("mov %[result], r4" : [result] "=r" (dump_regs[4]):);
	__asm __volatile__ ("mov %[result], r5" : [result] "=r" (dump_regs[5]):);
	__asm __volatile__ ("mov %[result], r6" : [result] "=r" (dump_regs[6]):);
	__asm __volatile__ ("mov %[result], r7" : [result] "=r" (dump_regs[7]):);
	__asm __volatile__ ("mov %[result], r8" : [result] "=r" (dump_regs[8]):);
	__asm __volatile__ ("mov %[result], r9" : [result] "=r" (dump_regs[9]):);
	__asm __volatile__ ("mov %[result], r10" : [result] "=r" (dump_regs[10]):);
	__asm __volatile__ ("mov %[result], r11" : [result] "=r" (dump_regs[11]):);
	__asm __volatile__ ("mov %[result], r12" : [result] "=r" (dump_regs[12]):);
	__asm __volatile__ ("mov %[result], lr" : [result] "=r" (dump_regs[13]):);
	__asm __volatile__ ("mov %[result], sp" : [result] "=r" (dump_regs[14]):);
	__asm __volatile__ ("mov %[result], pc" : [result] "=r" (dump_regs[15]):);

	printf("\n\r");
	for (i = 0; i < 15; i++) {
		printf("r%d = 0x%08x\n\r", i, dump_regs[i]);
	}

}



int main() {
	// Declare variables that we'll use later
	XScuTimer my_Timer;
	XScuTimer_Config *Timer_Config;

	XScuGic my_Gic;
	XScuGic_Config *Gic_Config;

	int a;

	Gic_Config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	XScuGic_CfgInitialize(&my_Gic, Gic_Config, Gic_Config->CpuBaseAddress);

	Timer_Config = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	XScuTimer_CfgInitialize(&my_Timer, Timer_Config, Timer_Config->BaseAddr);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler, &my_Gic);
	XScuGic_Connect(&my_Gic, XPAR_SCUTIMER_INTR,
			(Xil_ExceptionHandler) my_timer_interrupt_handler,
			(void *) &my_Timer);

	XScuGic_Enable(&my_Gic, XPAR_SCUTIMER_INTR);

	XScuTimer_EnableInterrupt(&my_Timer);

	Xil_ExceptionEnable();

	// Load the timer with a value that represents one second of real time
	// HINT: The SCU Timer is clocked at half the frequency of the CPU.
	//XScuTimer_LoadTimer(&my_Timer, XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ);
	XScuTimer_LoadTimer(&my_Timer, XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ/1000);

	// Enable Auto reload mode on the timer.  When it expires, it re-loads
	// the original value automatically.  This means that the timing interval
	// is never skewed by the time taken for the interrupt handler to run
	XScuTimer_EnableAutoReload(&my_Timer);


	// Create an infinite loop of nothing-ness
	while (1) {

		// There's nothing in here, the processor will just sit doing nothing.
		// The only way we'll see messages on the UART is if there's an interrupt.
		// In a real application, this is where the rest of our code would be.



		XScuTimer_Start(&my_Timer);

		a = 1;

		XScuTimer_Stop(&my_Timer);


		InterruptCounter = 0;

		XScuTimer_EnableAutoReload(&my_Timer);



	}

	// Print a message to the UART to show that we've made it out of the while loop

	// Disable interrupts in the Processor.
	Xil_ExceptionDisable();

	// Disconnect the interrupt for the Timer.
	XScuGic_Disconnect(&my_Gic, XPAR_SCUTIMER_INTR);

	cleanup_platform();
	return 0;
}

static void my_timer_interrupt_handler(void *CallBackRef) {
	// The Xilinx drivers automatically pass an instance of
	// the peripheral which generated in the interrupt into this
	// function, using the special parameter called "CallBackRef".
	// We will locally declare an instance of the timer, and assign
	// it to CallBackRef.  You'll see why in a minute.
	XScuTimer *my_Timer_LOCAL = (XScuTimer *) CallBackRef;
	int i;
	int word;
	// r0 r1 r2 r3 r4 r5 r11 r12 r13
	int RegsAddress[9] = {0x00118478, 0x0011847c, 0x00118480, 0x00118484, 0x0011839c, 0x001183a0, 0x00118390, 0x00118488, 0x0011848c};
    int Regs[15];

    int Status = 0;
	int randword;
	int randbit;

	// Here we'll check to see if the timer counter has expired.
	// Technically speaking, this check is not necessary.
	// We know that the timer has expired because that's the
	// reason we're in this handler in the first place!
	// However, this is an example of how a callback reference
	// can be used as a pointer to the instance of the timer
	// that expired.  If we had two timers then we could use the same
	// handler for both, and the "CallBackRef" would always tell us
	// which timer had generated the interrupt.

	if (XScuTimer_IsExpired(my_Timer_LOCAL) && InterruptCounter < INTERRUPT_COUNT_TIMEOUT_VALUE) {
		// Clear the interrupt flag in the timer, so we don't service
		// the same interrupt twice.
		XScuTimer_ClearInterruptStatus(my_Timer_LOCAL);

		// Increment a counter so that we know how many interrupts
		// have been generated.  The counter is a global variable
	//	InterruptCounter++;
        print_regs();

	}
	// Check to see if we've had more than the defined number of interrupts
	else {
		// Stop the timer from automatically re-loading, so
		// that we don't get any more interrupts.
		// We couldn't do this if we didn't have the CallBackRef

//		XScuTimer_DisableAutoReload(my_Timer_LOCAL);

	}
}
