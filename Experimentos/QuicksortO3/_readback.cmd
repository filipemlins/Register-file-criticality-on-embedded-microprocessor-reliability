setmode -bscan
addDevice -p 1 -file C:\Xilinx\14.5\ISE_DS\ISE\zynq\data\zynq7000_arm_dap.bsd
addDevice -p 2 -part xc7z020
setCable -target "digilent_plugin device=SN:210248470133"
setCableSpeed -speed 20000000
assignFile -p 2 -file design_1_wrapper.bit
verify -p 2
quit