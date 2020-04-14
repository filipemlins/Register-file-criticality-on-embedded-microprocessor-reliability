:: MAKE A FIRST READBACK OF THE FPGA IN ORDER TO GENERATE THE REFERENCE ONE (golden.bin)
del golden.bin
C:\Xilinx\14.5\ISE_DS\ISE\bin\nt64\impact.exe -batch _readback.cmd
ren impact.bin golden.bin