:: RESET WITH POWER CYCLE
python _swi off
ping -n 2 127.0.0.1>nul
python _swi on
ping -n 2 127.0.0.1>nul
::C:\Xilinx\14.5\ISE_DS\EDK\bin\nt64\xmd.exe -tcl ps7_init.tcl
