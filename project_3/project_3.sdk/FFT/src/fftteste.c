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
#include <math.h>
#include "stim.h"
#include "fft.h"
#define INTERRUPT_COUNT_TIMEOUT_VALUE 1
cplx_data_t* stim_buf;
cplx_data_t* arm_result_buf;

// Function prototypes
static void my_timer_interrupt_handler(void *CallBackRef);
// Global variables
int InterruptCounter = 0;


/*
int run_fft(fft_t* p_fft_inst)
{
	int status = 0;
	XTime tBegin, tEnd;
	unsigned long long sw_cycles = 0;
	// Make sure the buffer is clear before we populate it (this is generally not necessary and wastes time doing memory accesses, but for proving the DMA working, we do it anyway)


		XTime_GetTime(&tBegin);
		status = fft(p_fft_inst, (cplx_data_t*)stim_buf, (cplx_data_t*)arm_result_buf);
		XTime_GetTime(&tEnd);

		sw_cycles = sw_cycles + 2*(tEnd-tBegin);
        printf("%uld", sw_cycles);

	if (status != FFT_SUCCESS)
	{
		status = 2;
		return status;
	}
	status = 0;
	//xil_printf("FFT complete!\n\r");
    return status;
}

*/

/*
   This computes an in-place complex-to-complex FFT
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform
*/
short FFT(short int dir,long m,double *x,double *y)
{
   long n,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   n = 1;
   for (i=0;i<m;i++)
      n *= 2;

   /* Do the bit reversal */
   i2 = n >> 1;
   j = 0;
   for (i=0;i<n-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0;
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0;
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<n;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1;
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1)
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<n;i++) {
         x[i] /= n;
         y[i] /= n;
      }
   }

   return(TRUE);
}


int main() {

     int re[5] = {1,2,3,4};
     int im[5] = {0,2,3,1};
	//fft_t*       p_fft_inst;
	int Status;
	// Declare variables that we'll use later
	int TimerRate;
	XScuTimer my_Timer;
	XScuTimer_Config *Timer_Config;

	XScuGic my_Gic;
	XScuGic_Config *Gic_Config;

	XTime Xtime;
    XTime_GetTime(&Xtime);

	srand(Xtime);

	TimerRate = rand()%60000+1;

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


/*	 stim_buf = (cplx_data_t*) malloc(sizeof(cplx_data_t)*FFT_MAX_NUM_PTS);
	    if (stim_buf == NULL)
	    {
	    	xil_printf("ERROR! Failed to allocate memory for the stimulus buffer.\n\r");
	    	return -1;
	    }

	    arm_result_buf = (cplx_data_t*) malloc(sizeof(cplx_data_t)*FFT_MAX_NUM_PTS);
	    if (arm_result_buf == NULL)
	    {
	    	xil_printf("ERROR! Failed to allocate memory for the ARM result buffer.\n\r");
	    	return -1;
	    }

	    int i=0;
	     while (i<FFT_MAX_NUM_PTS)
	     {
	     	stim_buf[i].data_re = xn_re[i];
	     	stim_buf[i].data_im = xn_im[i];
	     	i++;
	     }*/

	     // Main control loop



	// Create an infinite loop of nothing-ness
	//while (1) {

		// There's nothing in here, the processor will just sit doing nothing.
		// The only way we'll see messages on the UART is if there's an interrupt.
		// In a real application, this is where the rest of our code would be.

	    printf("n %d;\n\r",TimerRate);

	//	XScuTimer_Start(&my_Timer);

    //	Status = run_fft(p_fft_inst);


		//XScuTimer_Stop(&my_Timer);

	/*	   if(Status != 0)
		   {
			   printf("FAILED;%d;\n",Status);
			   return 0;
		   }
		   else
		   {
		      printf("SUCCESS\n");

			InterruptCounter = 0;
		    XTime_GetTime(&Xtime);

			srand(Xtime);
			TimerRate = rand()%60000+1;
			XScuTimer_LoadTimer(&my_Timer, TimerRate);

			XScuTimer_EnableAutoReload(&my_Timer);

		}*/

	//}
	    FFT(1, 2, re, im);

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
	int RegsAddress[9] = {0x0011cbf8, 0x0011cbfc, 0x0011cc00, 0x0011cc04, 0x0011cb1c, 0x0011cb20, 0x0011cb10, 0x0011cc08, 0x0011cc0c};

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
		InterruptCounter++;



		/*Way to find the registers */
/*
		for (i = 0; i<10000; i++){
					word = Xil_In32(0x00118900+i*4);
					printf("\n\r 0x%08x = 0x%08x", 0x00118900+i*4, word);
				}
*/


       /*Imprime os registradores salvos no contexto */

/*
	    for (i = 0; i<8; i++){
			word = Xil_In32(RegsAddress[i]);
			printf("\n\r word = 0x%08x", word);

		}
*/
/*
	    Xil_Out32(RegsAddress[0], 0x0A);
	    Xil_Out32(RegsAddress[1], 0x0B);
	    Xil_Out32(RegsAddress[2], 0x0C);
	    Xil_Out32(RegsAddress[3], 0x0D);
	    Xil_Out32(RegsAddress[4], 0x0E);
	    Xil_Out32(RegsAddress[5], 0x09);
	    Xil_Out32(RegsAddress[6], 0x08);
	    Xil_Out32(RegsAddress[7], 0x07);
	    Xil_Out32(RegsAddress[8], 0x06);

*/
		 /*GERANDO BITFLIP ALEATORIO*/

		for (i = 0; i < 0; i++){

			randword = (rand()%14);

			if(randword <= 5){
				word = Xil_In32(RegsAddress[randword]);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				Xil_Out32(RegsAddress[randword], word);
			}

			else if(randword == 6 ){
				__asm __volatile__ ("mov %[result], r6" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r6, %[result]" : [result] "=r" (word):);

			}

			else if( randword == 7){
				__asm __volatile__ ("mov %[result], r7" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r7, %[result]" : [result] "=r" (word):);
			}
			else if(randword == 8 ){
				__asm __volatile__ ("mov %[result], r8" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r8, %[result]" : [result] "=r" (word):);
			}

			else if( randword == 9){
				__asm __volatile__ ("mov %[result], r9" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r9, %[result]" : [result] "=r" (word):);
			}

			else if(randword == 10 ){
				__asm __volatile__ ("mov %[result], r10" : [result] "=r" (word):);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				__asm __volatile__ ("mov  r10, %[result]" : [result] "=r" (word):);
			}

			else if(randword == 11 || randword == 12){

				word = Xil_In32(RegsAddress[randword-6]);
				//printf("\n\rBefore Bitflip r%d = 0x%08x", randword, word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip r%d - randbit = %d - word = 0x%08x\n\r", randword, randbit, word);
				printf("r%d;\n\r", randword);
				Xil_Out32(RegsAddress[randword], word);

			}
			else if(randword == 13){

				word = Xil_In32(RegsAddress[randword-6]);
				//printf("\n\rBefore Bitflip lr = 0x%08x", word);
				randbit = rand()%32;
				word = word ^ randbit;
				//printf("\n\rAfter Bitflip lr - randbit = %d - word = 0x%08x\n\r",  randbit, word);
				printf("lr;\n\r");
				Xil_Out32(RegsAddress[randword], word);

			}
			else if(randword == 14 ){
				__asm __volatile__ ("mov %[result], pc" : [result] "=r" (word):);
			//	printf("\n\rBefore Bitflip pc = 0x%08x", word);
				randbit = rand()%32;
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
