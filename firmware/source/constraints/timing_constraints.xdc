create_clock -period 10.000 -name sys_clk_pin -waveform {0.000 5.000} -add [get_ports clk]
create_clock -period 160.000 -name spi_sck -waveform {0.000 80.000} -add [get_ports spi_sck]


create_clock -period 5.000 -name CLK_200 -waveform {0.000 2.500} [get_ports CLK_200_P]


#create_clock -period 25.000 -name CLK_P -waveform {0.000 12.500} -add [get_ports CLK_P]
create_clock -period 5 -name CLK_P -waveform {0.000 2.500} -add [get_ports CLK_P]
# create_clock -period 4 -name CLK_P -waveform {0.000 2} -add [get_ports CLK_P]


set_input_delay -clock [get_clocks spi_sck] -clock_fall 12.500 [get_ports spi_mosi]





# Rising Edge System Synchronous Inputs
#
# A Single Data Rate (SDR) System Synchronous interface is
# an interface where the external device and the FPGA use
# the same clock, and a new data is captured one clock cycle
# after being launched
#
# input      __________            __________
# clock   __|          |__________|          |__
#           |
#           |------> (tco_min+trce_dly_min)
#           |------------> (tco_max+trce_dly_max)
#         __________      ________________    
# data    __________XXXXXX_____ Data _____XXXXXXX
#

set input_clock     CLK_P;   # Name of input clock
set tco_max         0.350;          # Maximum clock to out delay (external device)
set tco_min         0.000;          # Minimum clock to out delay (external device)
set trce_dly_max    1.000;          # Maximum board trace delay
set trce_dly_min    0.000;          # Minimum board trace delay
set input_ports     {Q1A_P Q1B_P Q2A_P Q2B_P};  # List of input ports

# Input Delay Constraint
set_input_delay -clock $input_clock -max [expr $tco_max + $trce_dly_max] [get_ports $input_ports];
set_input_delay -clock $input_clock -min [expr $tco_min + $trce_dly_min] [get_ports $input_ports];

# # Report Timing Template
# # report_timing -from [get_ports $input_ports] -max_paths 20 -nworst 1 -delay_type min_max -name sys_sync_rise_in  -file sys_sync_rise_in.txt;		
        
        
# set input_clock     spi_sck;   # Name of input clock
# set tco_max         100;          # Maximum clock to out delay (external device)
# set tco_min         0.000;          # Minimum clock to out delay (external device)
# set trce_dly_max    1.000;          # Maximum board trace delay
# set trce_dly_min    0.000;          # Minimum board trace delay
# set input_ports     {spi_cs0,spi_cs1};  # List of input ports

# # Input Delay Constraint
# set_input_delay -clock $input_clock -max [expr $tco_max + $trce_dly_max] [get_ports $input_ports];
# set_input_delay -clock $input_clock -min [expr $tco_min + $trce_dly_min] [get_ports $input_ports];

