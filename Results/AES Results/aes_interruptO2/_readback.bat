:: READBACK OF THE FPGA
del impact.bin
C:\Xilinx\14.5\ISE_DS\ISE\bin\nt64\impact.exe -batch _readback.cmd
_bin_comparator.exe logs_readback.txt golden.bin impact.bin