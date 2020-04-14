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

#define INTERRUPT_COUNT_TIMEOUT_VALUE 1

typedef int type;
// Function prototypes
static void my_timer_interrupt_handler(void *CallBackRef);
// Global variables
int InterruptCounter = 0;


int sortedarray[400] = {3, 3, 4, 8, 69, 72, 136, 150, 154, 162, 190, 202, 208, 245, 334, 352, 354, 357, 374, 386, 423, 457, 473, 506, 605, 607, 612, 614, 676, 710, 784, 810, 838, 854, 873, 878, 914, 963, 964, 975, 986, 992, 1027, 1042, 1067, 1067, 1072, 1073, 1099, 1126, 1165, 1171, 1176, 1185, 1206, 1231, 1235, 1253, 1268, 1334, 1339, 1341, 1341, 1359, 1399, 1406, 1477, 1520, 1521, 1547, 1554, 1586, 1626, 1630, 1648, 1703, 1715, 1825, 1832, 1866, 1881, 1895, 1912, 1946, 2003, 2034, 2044, 2063, 2073, 2121, 2123, 2157, 2214, 2219, 2244, 2369, 2398, 2402, 2408, 2408, 2413, 2439, 2478, 2490, 2510, 2536, 2548, 2565, 2604, 2647, 2663, 2690, 2715, 2737, 2746, 2763, 2790, 2919, 2934, 2946, 2989, 3011, 3022, 3029, 3042, 3065, 3068, 3099, 3103, 3133, 3153, 3182, 3221, 3238, 3242, 3267, 3269, 3310, 3312, 3329, 3409, 3414, 3454, 3506, 3532, 3541, 3546, 3548, 3564, 3622, 3654, 3664, 3665, 3764, 3770, 3793, 3826, 3834, 3873, 3887, 3923, 3927, 3944, 3999, 4014, 4033, 4084, 4102, 4109, 4134, 4152, 4173, 4174, 4177, 4204, 4209, 4261, 4275, 4279, 4348, 4397, 4453, 4485, 4527, 4649, 4650, 4656, 4657, 4679, 4709, 4726, 4727, 4730, 4747, 4764, 4871, 4876, 4879, 4883, 4888, 4924, 4928, 4932, 4961, 4981, 5010, 5041, 5054, 5070, 5090, 5120, 5130, 5137, 5138, 5147, 5163, 5182, 5217, 5251, 5327, 5386, 5405, 5429, 5431, 5435, 5448, 5472, 5473, 5474, 5594, 5623, 5626, 5631, 5698, 5724, 5729, 5756, 5762, 5766, 5790, 5796, 5815, 5841, 5866, 5878, 5893, 5913, 5979, 5981, 6032, 6065, 6071, 6136, 6154, 6162, 6178, 6179, 6206, 6219, 6227, 6229, 6243, 6252, 6263, 6264, 6318, 6330, 6346, 6349, 6400, 6424, 6459, 6478, 6495, 6545, 6620, 6647, 6651, 6669, 6679, 6732, 6745, 6769, 6788, 6795, 6802, 6883, 6897, 6929, 6953, 6960, 6972, 7000, 7004, 7041, 7076, 7136, 7178, 7179, 7206, 7315, 7358, 7401, 7408, 7414, 7445, 7485, 7509, 7515, 7524, 7537, 7564, 7707, 7709, 7761, 7768, 7813, 7826, 7852, 7869, 7880, 7904, 7922, 7968, 7983, 8002, 8036, 8161, 8184, 8218, 8238, 8326, 8335, 8354, 8357, 8370, 8377, 8395, 8413, 8467, 8468, 8541, 8545, 8570, 8577, 8577, 8586, 8591, 8614, 8625, 8678, 8704, 8718, 8738, 8751, 8786, 8805, 8828, 8861, 8903, 9016, 9037, 9049, 9065, 9081, 9095, 9115, 9130, 9141, 9240, 9262, 9264, 9283, 9285, 9305, 9306, 9363, 9375, 9407, 9437, 9453, 9465, 9480, 9481, 9491, 9504, 9536, 9545, 9588, 9712, 9721, 9742, 9820, 9855, 9883, 9887, 9944, 9987, 65535, 1051672};
int randomarray[400] = {3242,1399,8678,6972,5878,4279,3564,5766,5327,9240,9363,4261,878,6349,9712,7401,1912,8577,5729,5893,8413,2565,9049,9536,3329,386,7179,2647,6478,8238,8377,5010,4679,3011,8218,6795,7813,2663,5054,873,5474,1341,6545,975,3927,5182,4453,9130,8036,9742,9887,810,7869,4981,6178,1235,4649,8738,1586,1268,5756,4650,9016,6330,3454,5435,4932,6788,7041,7485,4657,3238,3099,7707,6252,5138,3541,8545,150,6206,6136,2123,6745,5041,7206,9453,9115,4397,1171,5626,3068,7537,6647,6953,1703,5130,3764,963,6669,1099,2408,5217,3310,6071,5866,1881,506,986,1895,2034,3,5251,5981,2746,9455,7611,7922,4888,1253,607,1547,8614,6929,7761,1073,2763,2003,9491,7826,8467,7315,7445,3269,4134,245,1946,9283,4014,4275,2219,1626,7524,334,4109,7515,1521,8354,4961,3267,2715,2478,357,2934,2402,8357,2398,3873,710,5979,4209,190,2548,6459,374,9987,2439,6229,7968,7358,423,7076,7880,5070,6227,5448,1126,8861,3887,9481,9820,9305,3999,7178,7509,2157,5796,4173,8541,3153,6243,4527,1176,8161,8326,2989,4102,5724,1067,5431,6065,3,4,6264,162,4924,4656,4709,6802,4033,5405,7004,8570,6219,612,2063,4485,5594,8704,1825,3834,1042,8903,1520,1359,1231,4174,2790,9141,8591,5090,5631,2369,5698,9306,72,1185,2490,6318,3548,5790,7768,2214,4883,7000,3409,4084,838,9437,4726,8786,6154,9545,5429,5762,3029,5623,605,8577,964,2044,6162,4879,2919,4876,8184,9465,8718,1067,614,136,1630,8395,4727,9081,8370,8805,3182,3414,9264,1477,6769,3654,7136,5137,4204,2408,6897,4871,1406,3022,3506,1866,3923,3312,3664,7983,7852,8828,6179,8002,2536,5473,9407,1554,4747,6400,9721,9855,6620,1206,992,2946,2244,4764,6263,9095,3665,3826,7564,473,2737,1334,208,4177,6424,1715,3042,9065,676,9504,6495,784,69,3103,5913,5386,3622,202,1165,6346,7709,9480,8586,1648,352,3065,2690,3532,3546,854,457,9037,3793,8751,154,5147,354,2510,9883,8,4348,6883,1832,7414,2121,4730,3133,6732,1339,5815,9285,5472,3221,9944,1072,6679,9375,4152,914,8335,6651,8625,9588,6960,8468,1027,9262,2604,6032,3944,3770,7408,7904,5120,2073,4928,1341,5163,5841,2413,3854,2435,6};


int verify(){
int i;
int errors = 0;

for(i=0; i<400; i++){
   if(sortedarray[i] != randomarray[i] ){
    errors++;
  }

}

return errors;
}



static void swap(void *x, void *y, size_t l) {
   char *a = x, *b = y, c;
   while(l--) {
      c = *a;
      *a++ = *b;
      *b++ = c;
   }
}

static void sort(char *array, size_t size, int (*cmp)(void*,void*), int begin, int end) {
   if (end > begin) {
      void *pivot = array + begin;
      int l = begin + size;
      int r = end;
      while(l < r) {
         if (cmp(array+l,pivot) <= 0) {
            l += size;
         } else if ( cmp(array+r, pivot) > 0 )  {
            r -= size;
         } else if ( l < r ) {
            swap(array+l, array+r, size);
         }
      }
      l -= size;
      swap(array+begin, array+l, size);
      sort(array, size, cmp, begin, l);
      sort(array, size, cmp, r, end);
   }
}

void quicksort(void *array, size_t nitems, size_t size, int (*cmp)(void*,void*)) {
   sort(array, size, cmp, 0, nitems*size);
}

int type_cmp(void *a, void *b){ return (*(type*)a)-(*(type*)b); }


int main()
{
    int len=sizeof(randomarray)/sizeof(type);
  //  char *sep="";
  //  int i;

	// Declare variables that we'll use later
	int TimerRate;
	XScuTimer my_Timer;
	XScuTimer_Config *Timer_Config;

	XScuGic my_Gic;
	XScuGic_Config *Gic_Config;

	XTime Xtime;


    init_platform();
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


	// Create an infinite loop of nothing-ness
	while (1) {

		// There's nothing in here, the processor will just sit doing nothing.
		// The only way we'll see messages on the UART is if there's an interrupt.
		// In a real application, this is where the rest of our code would be.

	    printf("n %d;\n\r",TimerRate);

		XScuTimer_Start(&my_Timer);


	quicksort(randomarray,len,sizeof(type),type_cmp);
	XScuTimer_Stop(&my_Timer);

	   if(Status != 0)
	   {
		   printf("FAILED;%d;\n",main_result);
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

		for (i = 0; i < 1; i++){

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
