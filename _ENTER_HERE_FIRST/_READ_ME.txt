######## SETUP INSTRUCTIONS ########

Author: Lucas Antunes Tambara {lucas.a.tambara@gmail.com}

01. OVERVIEW
There are 7 versions of the same program using different system architectures. Each version (scheme) is located in a different folder together with your respective files.

02. Mounting the setup
- First of all, we are in a Windows 8 system and I know that life is not fair...

- Test of the electrical switch:
-- With the switch plugged (power supply and USB), go to the terminal.
-- Enter in the folder "_ENTER_HERE_FIRST" and type "python _swi on" and "python _swi off".
-- If you do not see errors, the switch driver is working properly.
-- If you see errors, we have to reinstall the switch driver.
--- Go to the folder "_ENTER_HERE_FIRST\libusb-win32-bin-1.2.6.0\bin" and execute "inf-wizard.exe".
--- In the device selection step, search for a device with something like "...controller D" in the description.
--- Save the file that will be generated in the folder "_ENTER_HERE_FIRST\libusb-win32" and then click in "install driver" or "install device".
--- Now the switch should be installed correctly.
--- Go to the terminal, enter in the folder "_ENTER_HERE_FIRST" and type "python _swi on" and "python _swi off".
--- If you do not see errors, the switch driver is working properly.
--- If you see errors, try to reinstall the switch driver again.

- Mounting the board:
-- Check the picture "_setup.png" in the folder "_ENTER_HERE_FIRST".
-- The power supply must be connected to the electrical switch.
-- The cables labeled as "PROG." and "SERIAL ARM" must be connected to one USB extension.
-- The cables labeled as "SERIAL MICROBLAZE" and "SWITCH" must be connected to the other USB extension.
-- Plug both USB extensions in the USB 3.0 ports of the laptop.

- Ok, now the setup is mounted.

03. Contrary to what Chielle mentions in his setup instructions, you do not need to change anything in my configuration files. Of course, if you are using the board that I sent...

04. Finding out the COM ports

- Turn off Chielle's board.
- With the board turned off, go to the Device Manager and check the active COM port. THIS PORT IS THE MICROBLAZE'S PORT!
- Turn on the board and check the new COM port. THIS PORT IS THE ARM'S PORT!
- Check if both ports are setted to work at a baud rate of 115200 (Port > Properties > Port Settings > Bits per Second = 115200).

05. Starting the test

- Important tips:
-- Windows has a bug with COM ports that are labeled with more than one digit. If the COM port has two digits (>= COM10) you must add \\.\ before it (\\.\COM10).
-- Timeouts:
--- I usually use 60 for ARM.
--- I usually use 150 for MicroBlaze.

- s1_mm
-- Open a terminal.
-- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
--- <com_port> is the ARM's COM port.
--- <timeout> is the timeout of the script. I usually use 60 for ARM.
--- <log_file> the log file must be named as "logs_s1_arm.txt".

- s2_mm
-- Open a terminal.
-- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
--- <com_port> is the MicroBlaze's COM port.
--- <timeout> is the timeout of the script. I usually use 150 for MicroBlaze.
--- <log_file> the log file must be named as "logs_s2_mb.txt".

- s3_mm
-- Open two terminals.
-- Terminal 01:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the ARM's COM port.
--- <timeout> is the timeout of the script. I usually use 60 for ARM.
--- <log_file> the log file must be named as "logs_s3_arm.txt".
-- Terminal 02:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the MicroBlaze's COM port.
--- <timeout> is the timeout of the script. I usually use 150 for MicroBlaze.
--- <log_file> the log file must be named as "logs_s3_mb.txt".

- s4_mm
-- Open two terminals.
-- Terminal 01:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the ARM's COM port.
--- <timeout> is the timeout of the script. I usually use 60 for ARM.
--- <log_file> the log file must be named as "logs_s4_arm.txt".
-- Terminal 02:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the MicroBlaze's COM port.
--- <timeout> is the timeout of the script. I usually use 150 for MicroBlaze.
--- <log_file> the log file must be named as "logs_s4_mb.txt".

- s5_mm
-- Open two terminals.
-- Terminal 01:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the ARM's COM port.
--- <timeout> is the timeout of the script. I usually use 60 for ARM.
--- <log_file> the log file must be named as "logs_s5_arm.txt".
-- Terminal 02:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the MicroBlaze's COM port.
--- <timeout> is the timeout of the script. I usually use 150 for MicroBlaze.
--- <log_file> the log file must be named as "logs_s5_mb.txt".

- s6a_mm
-- Open two terminals.
-- Terminal 01:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the ARM's COM port.
--- <timeout> is the timeout of the script. I usually use 60 for ARM.
--- <log_file> the log file must be named as "logs_s6a_arm.txt".
-- Terminal 02:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the MicroBlaze's COM port.
--- <timeout> is the timeout of the script. I usually use 150 for MicroBlaze.
--- <log_file> the log file must be named as "logs_s6a_mb.txt".

- s6b_mm
-- Open two terminals.
-- Terminal 01:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the ARM's COM port.
--- <timeout> is the timeout of the script. I usually use 60 for ARM.
--- <log_file> the log file must be named as "logs_s6b_arm.txt".
-- Terminal 02:
--- Type: _CAMPAIGN_START.bat <com_port> <timeout> <log_file>.
---- <com_port> is the MicroBlaze's COM port.
--- <timeout> is the timeout of the script. I usually use 150 for MicroBlaze.
--- <log_file> the log file must be named as "logs_s6b_mb.txt".

Only run the _CAMPAIGN_START.bat the first time you are executing that version, if you need to run again use _CAMPAIGN_RESUME.bat.

06. Resuming the test

In case of any unusual problem that makes necessary to restart the test:
- Stop the script (Ctrl + C).
- Type: _CAMPAIGN_RESUME.bat <com_port> <timeout> <log_file>.

07. Final considerations

- The log files of each test version are located in their respective directories. 
- At each 5 minutes a backup of the test that is running is performed in the folder ".\backup".

- The expected characters are mainly "cc" and "cm". However, there are others like "wd", "cg", "dc", "dm", ...

- Tests 05, 06a and 06b are based on complex architectures with control and data dependencies. For these tests I need to have a very precise synchronization between the Processing System and Programmable Logic. This way, the system can be reprogrammed several times before starting executing in the expected way.

I THINK THAT IS ALL!

MAY THE FORCE BE WITH YOU!