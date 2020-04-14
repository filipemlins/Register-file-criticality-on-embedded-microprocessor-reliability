set_property SRC_FILE_INFO {cfile:c:/Users/Tambara/Desktop/zynq_scrubbing/sem_v1/sem_0_example/sem_0_example.srcs/sources_1/bd/ps_0/ip/ps_0_processing_system7_0_0/ps_0_processing_system7_0_0.xdc rfile:../../../sem_0_example.srcs/sources_1/bd/ps_0/ip/ps_0_processing_system7_0_0/ps_0_processing_system7_0_0.xdc id:1 order:EARLY scoped_inst:ps_0_i/processing_system7_0/inst} [current_design]
set_property SRC_FILE_INFO {cfile:C:/Users/Tambara/Desktop/zynq_scrubbing/sem_v1/sem_0_example/sem_0_example.srcs/constrs_1/imports/example_design/sem_0_sem_example.xdc rfile:../../../sem_0_example.srcs/constrs_1/imports/example_design/sem_0_sem_example.xdc id:2} [current_design]
set_property src_info {type:SCOPED_XDC file:1 line:21 export:INPUT save:INPUT read:READ} [current_design]
set_input_jitter clk_fpga_0 0.29544
set_property src_info {type:XDC file:2 line:27 export:INPUT save:INPUT read:READ} [current_design]
set_max_delay 9.0 -from [get_pins example_cfg/example_frame_ecc/*] -quiet -datapath_only
set_property src_info {type:XDC file:2 line:28 export:INPUT save:INPUT read:READ} [current_design]
set_max_delay 20.0 -from [get_pins example_cfg/example_frame_ecc/*] -to [all_outputs] -quiet -datapath_only
set_property src_info {type:XDC file:2 line:144 export:INPUT save:INPUT read:READ} [current_design]
set_property LOC FRAME_ECC_X0Y0 [get_cells example_cfg/example_frame_ecc]
set_property src_info {type:XDC file:2 line:145 export:INPUT save:INPUT read:READ} [current_design]
set_property LOC ICAP_X0Y1 [get_cells example_cfg/example_icap]
