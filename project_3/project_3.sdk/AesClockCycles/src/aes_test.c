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
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_io.h"

#include "platform.h"
#include <time.h>
#include <stdlib.h>
#include <xtime_l.h>
#include "xtmrctr.h"
#include "xparameters.h"

#include "aes.h"

XTmrCtr timer_dev;

int main() {



	   /*
	   +--------------------------------------------------------------------------+
	   | * Test Vectors (added for CHStone)                                       |
	   |     statemt, key : input data                                            |
	   +--------------------------------------------------------------------------+
	   */
	   int statemt[32];
	   int key[32];

	   statemt[0] = 50;
	   statemt[1] = 67;
	   statemt[2] = 246;
	   statemt[3] = 168;
	   statemt[4] = 136;
	   statemt[5] = 90;
	   statemt[6] = 48;
	   statemt[7] = 141;
	   statemt[8] = 49;
	   statemt[9] = 49;
	   statemt[10] = 152;
	   statemt[11] = 162;
	   statemt[12] = 224;
	   statemt[13] = 55;
	   statemt[14] = 7;
	   statemt[15] = 52;

	   key[0] = 43;
	   key[1] = 126;
	   key[2] = 21;
	   key[3] = 22;
	   key[4] = 40;
	   key[5] = 174;
	   key[6] = 210;
	   key[7] = 166;
	   key[8] = 171;
	   key[9] = 247;
	   key[10] = 21;
	   key[11] = 136;
	   key[12] = 9;
	   key[13] = 207;
	   key[14] = 79;
	   key[15] = 60;


	   int hw_result[32];


	     XTime begin_time, end_time, run_time_sw;


	 	init_platform();

	 	XTmrCtr_Initialize(&timer_dev, XPAR_AXI_TIMER_0_DEVICE_ID);
	 	XTmrCtr_SetOptions(&timer_dev, XPAR_AXI_TIMER_0_DEVICE_ID, XTC_ENABLE_ALL_OPTION);

	 	begin_time = XTmrCtr_GetValue(&timer_dev, XPAR_AXI_TIMER_0_DEVICE_ID);




		   aes_main(hw_result, statemt, key);

			end_time = XTmrCtr_GetValue(&timer_dev, XPAR_AXI_TIMER_0_DEVICE_ID);
			run_time_sw = end_time - begin_time;

			printf("\n\r clock cycles %llu \n\r", run_time_sw);

	cleanup_platform();
	return 0;
}
