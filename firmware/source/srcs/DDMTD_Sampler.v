`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Rohith 
// 
// Create Date: 08/12/2020 08:00:43 AM
// Design Name: 
// Module Name: DDMTD_Sampler
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module DDMTD_Sampler
#(
    parameter integer DATA_WIDTH = 32
 )
 (
    // Inputs for the sampling logic
    input WR_CLK, // should be synchronous with BEAT_CLK
    input BEAT_CLK,
    // input [DATA_WIDTH-1:0] external_counter,
    input RST,
    input en_SAMPLING_LOGIC, // Active high
    input wire  RD_CLK,
    output wire [DATA_WIDTH-1 : 0] R_TDATA,
    input wire  READ_EN,
    output wire PROG_FULL,
    output wire PROG_EMPTY,
    output wire EMPTY,
    output wire FULL,
    output wire [7:0] WRITE_COUNT,
    output wire [7:0] READ_COUNT

 );

// SYNCING INPUTS & OUTPUTS to domain clocks
wire beat_clk;
SYNC sync_beat_clk(.clk(WR_CLK),.I(BEAT_CLK),.O(beat_clk),.reset(0));
wire rst;
SYNC sync_rst(.clk(WR_CLK),.I(RST),.O(rst),.reset(0));
wire en_sampling_logic;
SYNC sync_en_sampling_logic(.clk(WR_CLK),.I(en_SAMPLING_LOGIC),.O(en_sampling_logic),.reset(0));


wire read_en;
SYNC sync_read_en(.clk(RD_CLK),.I(READ_EN),.O(read_en),.reset(0));
wire full;
SYNC sync_full(.clk(RD_CLK),.I(full),.O(FULL),.reset(0));




// COUNTER LOGIC
reg [31:0] external_counter;
always @(posedge WR_CLK) begin
    if (en_sampling_logic & (~rst)) external_counter <= external_counter +1;
    else external_counter <=0;
end
// END COUNTER LOGIC

reg [31:0] DATA_IN;
reg temp_mem=0;
reg write_en=0;
integer test_counter = 0;
always @(negedge WR_CLK) 
begin
    test_counter <= test_counter + 1;
    if(temp_mem !=  beat_clk && en_sampling_logic) begin
        temp_mem <= beat_clk;
        write_en <= 1;
        DATA_IN  <= external_counter[31:0];
    end
    else begin
        write_en <=0;
    end
    if (rst|~en_sampling_logic)
        test_counter <= 0 ;
end


//SYNC write_en & DATA_IN
reg write_en_sync_pos;
reg [31:0] data_in_sync_pos;
always @(posedge WR_CLK ) begin
    write_en_sync_pos <= write_en;
    data_in_sync_pos <= DATA_IN;
end


FIFO_Array FIFO_Array_inst (
    .rst(rst),
    .wr_clk(WR_CLK),
    .rd_clk(RD_CLK), //Done to flush out the data
    .din(data_in_sync_pos),
    .wr_en(write_en_sync_pos),
    .rd_en(read_en),
    .dout(R_TDATA),
    .full(full),
    .empty(EMPTY)
    //,.rd_data_count(READ_COUNT), if you are using Block RAM FIFO
    //.wr_data_count(WRITE_COUNT), if you are using Block RAM FIFO
    //.prog_empty(PROG_EMPTY),
    //.prog_full(PROG_FULL)
    //.wr_rst_busy,
    //.rd_rst_busy
    );

endmodule