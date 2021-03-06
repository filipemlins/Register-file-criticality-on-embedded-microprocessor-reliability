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
#include "xparameters.h"
#include "xil_printf.h"
#include "xtime_l.h"
#include "platform.h"
#include "xil_io.h"
//#include "xtmrctr.h"

#define START_DUT  0x40000000
#define START_DUTT 0x42000000

#define r12_ADDR    0x40000004

int main()
{

    u32 *start_dut, *start_dutt;


    register int p0 asm("r0") = 0;
    register int p1 asm("r1") = 0;

    register int p4 asm("r4") = 0;
    register int p5 asm("r5") = 0;
    register int p6 asm("r6") = 0;
    register int p7 asm("r7") = 0;
    register int p8 asm("r8") = 0;
    register int p9 asm("r9") = 0;
    register int p10 asm("r10") = 0;
    register int p11 asm("r11") = 0;
    register int *p12 asm("r12");

    init_platform();

    Xil_Out32(r12_ADDR, 0x00);
    p12 = (int*) r12_ADDR;

	start_dut = (u32 *)START_DUT;
	start_dutt = (u32 *)START_DUTT;

	while(1){

		while((start_dut[0] != 0xAAAAAAAA)&&(start_dutt[0] != 0xAAAAAAAA));

		p0 += 512;
		p1 += 511;
		p4 += 512;
		p5 += 512;
		p6 += 512;
		p7 += 512;
		p8 += 512;
		p9 += 512;
		p10 = 512;
		p11 += 512;

		if(p0 != 512){
			p12 += 1;
		}
		if(p1 != 512){
			p12 += 2;
		}
		if(p4 != 512){
			p12 += 4;
		}
		if(p5 != 512){
			p12 += 8;
		}
		if(p6 != 512){
			p12 += 16;
		}
		if(p7 != 512){
			p12 += 32;
		}
		if(p8 != 512){
			p12 += 64;
		}
		if(p9 != 512){
			p12 += 128;
		}
		if(p10 != 512){
			p12 += 256;
			}
		if(p11 != 512){
			p12 += 512;
			}


		start_dutt[0] = 0xDDDDDDDD;
		start_dut[0] = 0xDDDDDDDD;


	}

    //cleanup_platform();
    return 0;
}
