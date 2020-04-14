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
 *   uartlite    C�onfigurable only in HW design
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

// Function prototypes
static void my_timer_interrupt_handler(void *CallBackRef);
// Global variables
int InterruptCounter = 0;

/* MULTIPLICACAO DE MATRIZES*/
#define MATRIX_SIZE 20

typedef unsigned long MATRIX_TYPE[MATRIX_SIZE][MATRIX_SIZE];

int mem[(sizeof(MATRIX_TYPE) / sizeof(int)) * 3];

MATRIX_TYPE* A = (MATRIX_TYPE *) &mem[(0 * sizeof(MATRIX_TYPE)) / sizeof(int)];

MATRIX_TYPE* B = (MATRIX_TYPE *) &mem[(1 * sizeof(MATRIX_TYPE)) / sizeof(int)];

MATRIX_TYPE* C = (MATRIX_TYPE *) &mem[(2 * sizeof(MATRIX_TYPE)) / sizeof(int)];

void init_matrix(MATRIX_TYPE M, int value) {
	int i, j;

	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			M[i][j] = value;
		}
	}
}

void multi_matrix(MATRIX_TYPE A, MATRIX_TYPE B, MATRIX_TYPE C) {
	int i, j;

	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			int k;
			int sum = 0;
			for (k = 0; k < MATRIX_SIZE; k++) {
				sum += A[i][k] * B[k][j];
			}
			C[i][j] = sum;
		}
	}

}

void print_matrix(MATRIX_TYPE C) {
	int i, j;

	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			printf("C[%d][%d] = %d \n\r", i, j, C[i][j]);

		}
	}
}


int verify_matrix(MATRIX_TYPE C){
	int i, j;
    int counter = 0;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			if(C[i][j] != MATRIX_SIZE*2){
                counter++;
			}
		}
	}
	if(counter != 0 ){
		printf("FAILED\n\r");
		printf("%d;\n\r", counter);
	}
	else if(counter == 0){
		printf("%d;\n\r", counter);
		printf("SUCCESS;\n\r");

		return 0;
	}
}


void print_regs(){
	static unsigned int dump_regs[16] = { 0xFFFF };
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
	for (i = 0; i < 16; i++) {
		printf("dump[%d] = 0x%08x\n\r", i, dump_regs[i]);
	}

}

int main() {
	// Declare variables that we'll use later


	XTime Start;
	XTime Stop;
    printf("Start!\n\r");

    XTime_GetTime((XTime *) &Start);

	init_matrix( *A, 2);

	init_matrix( *B, 1);

	init_matrix( *C, 0);

	multi_matrix( *A, *B, *C );



	XTime_GetTime((XTime *) &Stop);

    printf("%llu\n\r", Stop-Start);



	cleanup_platform();
	return 0;
}