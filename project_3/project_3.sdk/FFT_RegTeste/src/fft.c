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
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

/* Factored discrete Fourier transform, or FFT, and its inverse iFFT */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

// Function prototypes
static void my_timer_interrupt_handler(void *CallBackRef);
// Global variables
int InterruptCounter = 0;

#define q	3		/* for 2^3 points */
#define N	(1<<q)		/* N-point FFT, iFFT */

typedef float real;
typedef struct {
	real Re;
	real Im;
} complex;

#ifndef PI
# define PI	3.14159265358979323846264338327950288
#endif

complex result[N];

/* Print a vector of complexes as ordered pairs. */
static void print_vector(const char *title, complex *x, int n) {
	int i;
	printf("%s (dim=%d):", title, n);
	for (i = 0; i < n; i++)
		printf(" %5.2f,%5.2f ", x[i].Re, x[i].Im);
	putchar('\n');
	return;
}

/*
 fft(v,N):
 [0] If N==1 then return.
 [1] For k = 0 to N/2-1, let ve[k] = v[2*k]
 [2] Compute fft(ve, N/2);
 [3] For k = 0 to N/2-1, let vo[k] = v[2*k+1]
 [4] Compute fft(vo, N/2);
 [5] For m = 0 to N/2-1, do [6] through [9]
 [6]   Let w.re = cos(2*PI*m/N)
 [7]   Let w.im = -sin(2*PI*m/N)
 [8]   Let v[m] = ve[m] + w*vo[m]
 [9]   Let v[m+N/2] = ve[m] - w*vo[m]
 */
void fft(complex *v, int n, complex *tmp) {
	if (n > 1) { /* otherwise, do nothing and return */
		int k, m;
		complex z, w, *vo, *ve;
		ve = tmp;
		vo = tmp + n / 2;
		for (k = 0; k < n / 2; k++) {
			ve[k] = v[2 * k];
			vo[k] = v[2 * k + 1];
		}
		fft(ve, n / 2, v); /* FFT on even-indexed elements of v[] */
		fft(vo, n / 2, v); /* FFT on odd-indexed elements of v[] */
		for (m = 0; m < n / 2; m++) {
			w.Re = cos(2 * PI * m / (double) n);
			w.Im = -sin(2 * PI * m / (double) n);
			z.Re = w.Re * vo[m].Re - w.Im * vo[m].Im; /* Re(w*vo[m]) */
			z.Im = w.Re * vo[m].Im + w.Im * vo[m].Re; /* Im(w*vo[m]) */
			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;
			v[m + n / 2].Re = ve[m].Re - z.Re;
			v[m + n / 2].Im = ve[m].Im - z.Im;
		}
	}
	return;
}

int main(void) {
	complex v1[N],  scratch[N];

	int k;
	int errors = 0;
	/* Fill v[] with a function of known FFT: */

	// Declare variables that we'll use later


	int TimerRate;
	XScuTimer my_Timer;
	XScuTimer_Config *Timer_Config;

	XScuGic my_Gic;
	XScuGic_Config *Gic_Config;

	XTime Xtime;

	// resultado teste

	result[0].Im = -0.00;
	result[0].Re = -0.00;

	result[1].Im = -0.00;
	result[1].Re = 0.00;

	result[2].Im = -0.00;
	result[2].Re = 0.00;

	result[3].Im = 0.00;
	result[3].Re = 0.00;

	result[4].Im = -0.00;
	result[4].Re = -0.00;

	result[5].Im = -0.00;
	result[5].Re = 0.00;

	result[6].Im = -0.00;
	result[6].Re = 0.00;

	result[7].Im = -0.00;
	result[7].Re = 2.40;

	XTime_GetTime(&Xtime);

	srand(Xtime);

	TimerRate = rand() % 110000 + 1;

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
	XScuTimer_LoadTimer(&my_Timer, TimerRate);

	// Enable Auto reload mode on the timer.  When it expires, it re-loads
	// the original value automatically.  This means that the timing interval
	// is never skewed by the time taken for the interrupt handler to run
	XScuTimer_EnableAutoReload(&my_Timer);

	while (1) {

		// There's nothing in here, the processor will just sit doing nothing.
		// The only way we'll see messages on the UART is if there's an interrupt.
		// In a real application, this is where the rest of our code would be.

		printf("n %d;\n\r", TimerRate);

		XScuTimer_Start(&my_Timer);

		// programa
		for (k = 0; k < N; k++) {

			v1[k].Re = 0.3 * cos(2 * PI * k / (double) N);
			v1[k].Im = -0.3 * sin(2 * PI * k / (double) N);
		}

		/* FFT, iFFT of v1[]: */
		//print_vector("Orig", v1, N);
		fft(v1, N, scratch);

		XScuTimer_Stop(&my_Timer);



//  print_vector(" FFT", v1, N);

		// end program

		// verificação
		for (k = 0; k < 8; k++) {

			if (fabs(v1[k].Im - result[k].Im) > 0.00001) {
				errors++;
			}

			if (fabs(v1[k].Re - result[k].Re) > 0.00001) {
				errors++;
			}

		}

		if(errors != 0){
          return errors;
		}
		else{

			InterruptCounter = 0;
		    XTime_GetTime(&Xtime);

			srand(Xtime);
			TimerRate = rand()%110000+1;
			XScuTimer_LoadTimer(&my_Timer, TimerRate);

			XScuTimer_EnableAutoReload(&my_Timer);


		}

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
	int randword;
	int randbit;
	int word;
	// r0 r1 r2 r3 r4 r5 r11 r12 r13
	int RegsAddress[9] = { 0x00119738, 0x0011973c, 0x00119740, 0x00119744,
			0x0011965c, 0x00119660, 0x00119650, 0x00119748, 0x0011974c };

	// Here we'll check to see if the timer counter has expired.
	// Technically speaking, this check is not necessary.
	// We know that the timer has expired because that's the
	// reason we're in this handler in the first place!
	// However, this is an example of how a callback reference
	// can be used as a pointer to the instance of the timer
	// that expired.  If we had two timers then we could use the same
	// handler for both, and the "CallBackRef" would always tell us
	// which timer had generated the interrupt.

	if (XScuTimer_IsExpired(
			my_Timer_LOCAL) && InterruptCounter < INTERRUPT_COUNT_TIMEOUT_VALUE) {
		// Clear the interrupt flag in the timer, so we don't service
		// the same interrupt twice.
		XScuTimer_ClearInterruptStatus(my_Timer_LOCAL);

		// Increment a counter so that we know how many interrupts
		// have been generated.  The counter is a global variable
		InterruptCounter++;

		/*Way to find the registers */

		 for (i = 0; i<2000; i++){
		 word = Xil_In32(0x00118500+i*4);
		 printf("\n\r 0x%08x = 0x%08x", 0x00118500+i*4, word);
		 }



		/*Imprime os registradores salvos no contexto */

		/*
		 for (i = 0; i<8; i++){
		 word = Xil_In32(RegsAddress[i]);
		 printf("\n\r word = 0x%08x", word);

		 }
		 */

		 Xil_Out32(RegsAddress[0], 0x0A);
		 Xil_Out32(RegsAddress[1], 0x0B);
		 Xil_Out32(RegsAddress[2], 0x0C);
		 Xil_Out32(RegsAddress[3], 0x0D);
		 Xil_Out32(RegsAddress[4], 0x0E);
		 Xil_Out32(RegsAddress[5], 0x09);
		 Xil_Out32(RegsAddress[6], 0x08);
		 Xil_Out32(RegsAddress[7], 0x07);
		 Xil_Out32(RegsAddress[8], 0x06);

		/*GERANDO BITFLIP ALEATORIO*/

		for (i = 0; i < 0; i++) {

			randword = (rand() % 14);

			if (randword <= 5) {
				word = Xil_In32(RegsAddress[randword]);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				Xil_Out32(RegsAddress[randword], word);
			}

			else if (randword == 6) {
				__asm __volatile__ ("mov %[result], r6" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r6, %[result]" : [result] "=r" (word):);

			}

			else if (randword == 7) {
				__asm __volatile__ ("mov %[result], r7" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r7, %[result]" : [result] "=r" (word):);
			} else if (randword == 8) {
				__asm __volatile__ ("mov %[result], r8" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r8, %[result]" : [result] "=r" (word):);
			}

			else if (randword == 9) {
				__asm __volatile__ ("mov %[result], r9" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r9, %[result]" : [result] "=r" (word):);
			}

			else if (randword == 10) {
				__asm __volatile__ ("mov %[result], r10" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r10, %[result]" : [result] "=r" (word):);
			}

			else if (randword == 11 || randword == 12) {

				word = Xil_In32(RegsAddress[randword - 6]);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				Xil_Out32(RegsAddress[randword], word);

			} else if (randword == 13) {

				word = Xil_In32(RegsAddress[randword - 6]);
				//printf("\n\rBefore Bitflip lr = 0x%08x", word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip lr - randbit = %d - word = 0x%08x\n\r",  randbit, word);
				printf("lr;\n\r");
				Xil_Out32(RegsAddress[randword], word);

			} else if (randword == 14) {
				__asm __volatile__ ("mov %[result], pc" : [result] "=r" (word):);
				//	printf("\n\rBefore Bitflip pc = 0x%08x", word);
				randbit = rand() % 32;
				word = word ^ randbit;
				//	printf("\n\rAfter Bitflip  pc - randbit = %d - word = 0x%08x\n\r", randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  pc, %[result]" : [result] "=r" (word):);
			}

		}

		//    printf("\n\r-------------------------------------------------------\n\r\n\r");
	}
	// Check to see if we've had more than the defined number of interrupts
	else {
		// Stop the timer from automatically re-loading, so
		// that we don't get any more interrupts.
		// We couldn't do this if we didn't have the CallBackRef

		XScuTimer_DisableAutoReload(my_Timer_LOCAL);

	}
}

