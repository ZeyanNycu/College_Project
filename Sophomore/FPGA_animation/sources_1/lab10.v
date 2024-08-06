`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Dept. of Computer Science, National Chiao Tung University
// Engineer: Chun-Jen Tsai 
// 
// Create Date: 2018/12/11 16:04:41
// Design Name: 
// Module Name: lab9
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: A circuit that show the animation of a fish swimming in a seabed
//              scene on a screen through the VGA interface of the Arty I/O card.
// 
// Dependencies: vga_sync, clk_divider, sram 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module lab10(
    input  clk,
    input  reset_n,
    input  [3:0] usr_btn,
    output [3:0] usr_led,
    
    // VGA specific I/O ports
    output VGA_HSYNC,
    output VGA_VSYNC,
    output [3:0] VGA_RED,
    output [3:0] VGA_GREEN,
    output [3:0] VGA_BLUE
    );

// Declare system variables
reg  [31:0] fish_clock;
reg  [31:0] fish_clock_1;
reg  [31:0] fish_clock_2;
reg  [31:0] fish_clock_3;
reg  [31:0] fish_clock_4;
reg [3:0] R;
reg [3:0] G;
reg [3:0] B;
wire [9:0]  pos[0:5];
wire        fish_region;
wire        fish_region_1;
wire        fish_region_2;
wire        fish_region_3;
wire        fish_region_4;
wire        pod_region;
// declare SRAM control signals
wire [16:0] sram_addr[0:5];
wire [11:0] data_in;
wire [11:0] data_out[0:5];
wire [11:0] data_out_re;
wire        sram_we, sram_en;
wire [9:0] wir_pod_VPOS;
assign wir_pod_VPOS = pod_VPOS+pos_ball;
// General VGA control signals
wire vga_clk;         // 50MHz clock for VGA control
wire video_on;        // when video_on is 0, the VGA controller is sending
                      // synchronization signals to the display device.
  
wire pixel_tick;      // when pixel tick is 1, we must update the RGB value
                      // based for the new coordinate (pixel_x, pixel_y)
  
wire [9:0] pixel_x;   // x coordinate of the next pixel (between 0 ~ 639) 
wire [9:0] pixel_y;   // y coordinate of the next pixel (between 0 ~ 479)
  
reg  [11:0] rgb_reg;  // RGB value for the current pixel
reg  [11:0] rgb_next; // RGB value for the next pixel
  
// Application-specific VGA signals
reg  [17:0] pod_pixel_addr;
reg  [17:0] fish5_pixel_addr;
reg  [17:0] fish4_pixel_addr;
reg  [17:0] fish3_pixel_addr;
reg  [17:0] fish2_pixel_addr;
reg  [17:0] fish1_pixel_addr;
reg  [17:0] back_pixel_addr;
// Declare the video buffer size
localparam VBUF_W = 320; // video buffer width
localparam VBUF_H = 240; // video buffer height
reg [2:0] P;
reg [2:0] P_next;
localparam [2:0] S_MAIN_INIT = 'b000,S_MAIN_RGB = 'b001,
                 S_MAIN_MOVE = 'b010,S_MAIN_WAIT = 'b011;
// Set parameters for the fish images
localparam FISH_VPOS   = 30; // Vertical location of the fish in the sea image.
localparam FISH_VPOS_1  = 50; 
localparam FISH_VPOS_2  = 128;
localparam FISH_VPOS_3  = 140;
localparam FISH_VPOS_4  = 180;
localparam pod_VPOS =     0;
localparam FISH_W      = 64; // Width of the fish.
localparam FISH_H      = 32;
localparam FISH_W_1      = 64; // Width of the fish.
localparam FISH_H_1      = 44;
localparam FISH_W_2      = 64;
localparam FISH_H_2      = 32;
localparam FISH_W_3      = 64;
localparam FISH_H_3      = 44;
localparam FISH_W_4      = 64;
localparam FISH_H_4      = 72;
localparam pod_W         = 64;
localparam pod_H         = 22; // Height of the fish.
reg [17:0] fish_addr[0:7]; 
reg [17:0] fish_addr_1[0:7];
reg [17:0] fish_addr_2[0:7];
reg [17:0] fish_addr_4[0:7];  // Address array for up to 8 fish images.

// Initializes the fish images starting addresses.
// Note: System Verilog has an easier way to initialize an array,
//       but we are using Verilog 2001 :(
initial begin
  fish_addr[0] = 18'd0;         /* Addr for fish image #1 */
  fish_addr[1] = FISH_W*FISH_H;
  fish_addr[2] = 2*FISH_W*FISH_H;
  fish_addr[3] = 3*FISH_W*FISH_H;
  fish_addr[4] = 4*FISH_W*FISH_H;
  fish_addr[5] = 5*FISH_W*FISH_H;
  fish_addr[6] = 6*FISH_W*FISH_H;
  fish_addr[7] = 7*FISH_W*FISH_H;
  
  fish_addr_1[0] = 18'd0;         /* Addr for fish image #1 */
  fish_addr_1[1] = FISH_W_1*FISH_H_1;
  fish_addr_1[2] = 2*FISH_W_1*FISH_H_1;
  fish_addr_1[3] = 3*FISH_W_1*FISH_H_1;
  fish_addr_1[4] = 4*FISH_W_1*FISH_H_1;
  fish_addr_1[5] = 5*FISH_W_1*FISH_H_1;
  fish_addr_1[6] = 6*FISH_W_1*FISH_H_1;
  fish_addr_1[7] = 7*FISH_W_1*FISH_H_1;/* Addr for fish image #2 */
  
  fish_addr_2[0] = 18'd0;         /* Addr for fish image #1 */
  fish_addr_2[1] = FISH_W_2*FISH_H_2;
  fish_addr_2[2] = 2*FISH_W_2*FISH_H_2;
  fish_addr_2[3] = 3*FISH_W_2*FISH_H_2;
  fish_addr_2[4] = 4*FISH_W_2*FISH_H_2;
  fish_addr_2[5] = 5*FISH_W_2*FISH_H_2;
  fish_addr_2[6] = 6*FISH_W_2*FISH_H_2;
  fish_addr_2[7] = 7*FISH_W_2*FISH_H_2;
  
  fish_addr_4[0] = 18'd0;         /* Addr for fish image #1 */
  fish_addr_4[1] = FISH_H*FISH_W;
  fish_addr_4[2] = 2*FISH_H*FISH_W;
  fish_addr_4[3] = 3*FISH_H*FISH_W;
  fish_addr_4[4] = 4*FISH_H*FISH_W;
  fish_addr_4[5] = 5*FISH_H*FISH_W;
  fish_addr_4[6] = 6*FISH_H*FISH_W;
  fish_addr_4[7] = 7*FISH_H*FISH_W;
end

// Instiantiate the VGA sync signal generator
vga_sync vs0(
  .clk(vga_clk), .reset(~reset_n), .oHS(VGA_HSYNC), .oVS(VGA_VSYNC),
  .visible(video_on), .p_tick(pixel_tick),
  .pixel_x(pixel_x), .pixel_y(pixel_y)
);
wire btn_level[0:3];
debounce btn_db0(
  .clk(clk),
  .btn_input(usr_btn[0]),
  .btn_output(btn_level[0])
);
debounce btn_db1(
  .clk(clk),
  .btn_input(usr_btn[1]),
  .btn_output(btn_level[1])
);
debounce btn_db2(
  .clk(clk),
  .btn_input(usr_btn[2]),
  .btn_output(btn_level[2])
);
debounce btn_db3(
  .clk(clk),
  .btn_input(usr_btn[3]),
  .btn_output(btn_level[3])
);
clk_divider#(2) clk_divider0(
  .clk(clk),
  .reset(~reset_n),
  .clk_out(vga_clk)
);
reg prev_btn_level [0:3];
always @(posedge clk) begin
    if(~reset_n) begin
        prev_btn_level[0] <=0;
        prev_btn_level[1] <=0;
        prev_btn_level[2] <=0;
        prev_btn_level[3] <=0;
    end
    else begin
        prev_btn_level[0] <=btn_level[0];
        prev_btn_level[1] <=btn_level[1];
        prev_btn_level[2] <=btn_level[2];
        prev_btn_level[3] <=btn_level[3];
    end
end 
wire btn_pressed[0:3];
assign btn_pressed[0] = (btn_level[0] == 1 && prev_btn_level[0] == 0)? 1 : 0;
assign btn_pressed[1] = (btn_level[1] == 1 && prev_btn_level[1] == 0)? 1 : 0;
assign btn_pressed[2] = (btn_level[2] == 1 && prev_btn_level[2] == 0)? 1 : 0;
assign btn_pressed[3] = (btn_level[3] == 1 && prev_btn_level[3] == 0)? 1 : 0;// ------------------------------------------------------------------------
// The following code describes an initialized SRAM memory block that
// stores a 320x240 12-bit seabed image, plus two 64x32 fish images.
sram #(.DATA_WIDTH(12), .ADDR_WIDTH(18), .RAM_SIZE(VBUF_W*VBUF_H))
  ram0 (.clk(clk), .we(sram_we), .en(sram_en),
          .addr(sram_addr[0]), .data_i(data_in), .data_o(data_out[0]));
sram_fish_1 #(.DATA_WIDTH(12), .ADDR_WIDTH(18), .RAM_SIZE(FISH_H*FISH_W*8))
  ram1 (.clk(clk), .we(sram_we), .en(sram_en),
          .addr(sram_addr[1]), .data_i(data_in), .data_o(data_out[1]));
sram_fish_2 #(.DATA_WIDTH(12), .ADDR_WIDTH(18), .RAM_SIZE(FISH_H_1*FISH_W_1*8))
  ram2 (.clk(clk), .we(sram_we), .en(sram_en),
          .addr(sram_addr[2]), .data_i(data_in), .data_o(data_out[2]));
sram_fish_3 #(.DATA_WIDTH(12), .ADDR_WIDTH(18), .RAM_SIZE(FISH_H_2*FISH_W_2*8))
  ram3 (.clk(clk), .we(sram_we), .en(sram_en),
          .addr(sram_addr[3]), .data_i(data_in), .data_o(data_out[3]));
sram_fish_4 #(.DATA_WIDTH(12), .ADDR_WIDTH(18), .RAM_SIZE(FISH_H*FISH_W*8))
  ram4 (.clk(clk), .we(sram_we), .en(sram_en),
          .addr(sram_addr[4]), .data_i(data_in), .data_o(data_out[4]));
sram_pod #(.DATA_WIDTH(12), .ADDR_WIDTH(18), .RAM_SIZE(pod_W*pod_H))
  ram5(.clk(clk), .we(sram_we), .en(sram_en),
          .addr(sram_addr[5]), .data_i(data_in), .data_o(data_out[5]));
assign sram_we = usr_btn[3]; // In this demo, we do not write the SRAM. However, if
                             // you set 'sram_we' to 0, Vivado fails to synthesize
                             // ram0 as a BRAM -- this is a bug in Vivado.
assign sram_en = 1;          // Here, we always enable the SRAM block.
assign sram_addr[0] = back_pixel_addr;
assign sram_addr[1] = fish1_pixel_addr;
assign sram_addr[2] = ((pixel_y)>'d256)?fish4_pixel_addr:fish2_pixel_addr;
assign sram_addr[3] = fish3_pixel_addr;
assign sram_addr[4] = fish5_pixel_addr;
assign sram_addr[5] = pod_pixel_addr;
assign data_in = 12'h000; // SRAM is read-only so we tie inputs to zeros.
// End of the SRAM memory block.
// ------------------------------------------------------------------------
// VGA color pixel generator
assign {VGA_RED, VGA_GREEN, VGA_BLUE} = rgb_reg;

// ------------------------------------------------------------------------
// An animation clock for the motion of the fish, upper bits of the
// fish clock is the x position of the fish on the VGA screen.
// Note that the fish will move one screen pixel every 2^20 clock cycles,
// or 10.49 msec
assign pos[0] = fish_clock[31:20]; // the x position of the right edge of the fish image
assign pos[1] = fish_clock_1[31:20];
assign pos[2] = fish_clock_2[31:20];
assign pos[3] = fish_clock_1[31:20];
assign pos[4] = fish_clock_4[31:20];
assign pos[5] = 320;      // in the 640x480 VGA screen
always @(posedge clk) begin
  if (~reset_n || fish_clock[31:21] > VBUF_W + FISH_W) begin
    fish_clock <= 0;
  end
  else begin
    fish_clock <= fish_clock + 5;
  end
end
always @(posedge clk) begin 
    if(~reset_n)  P <= S_MAIN_INIT;
    else P <= P_next;  
end 
reg sign = 0;
always @(posedge clk) begin
    case (P)
    S_MAIN_INIT: 
        if(btn_pressed[0]) P_next = S_MAIN_RGB;
        else if(btn_pressed[1]) P_next = S_MAIN_WAIT;
        else P_next = S_MAIN_INIT;
    S_MAIN_RGB:
         P_next = S_MAIN_RGB;
    S_MAIN_WAIT:
        if(btn_pressed[0] || btn_pressed[1] || btn_pressed[2] ) P_next =S_MAIN_MOVE;
        else P_next = S_MAIN_WAIT;
    S_MAIN_MOVE:
        if(sign) P_next = S_MAIN_WAIT;
        else P_next = S_MAIN_MOVE;
    endcase  
end
reg [9:0] pos_ball;
always @(posedge clk) begin
    if(~reset_n) begin
        pos_ball = 0;
        sign = 0;
        touch = 0;
     end
     else if(P==S_MAIN_MOVE)begin
        if(btn_pressed[2]) begin
            if (pos_ball>10) pos_ball = pos_ball - 10;
             else pos_ball = 0;
         end
        else if(btn_pressed[1]) begin
             if (pos_ball<=210) pos_ball = pos_ball + 10;
             else pos_ball = pos_ball;
         end
        if((pod_region && fish_region)) touch[0] =1; 
        if (pod_region && fish_region_1) touch[1] =1;
        if (pod_region && fish_region_2) touch[2] =1; 
        if (pod_region && fish_region_3) touch[3] =1;
        if (pod_region && fish_region_4) touch[4] =1;
        if(touch[0] && touch[1] && touch[2] && touch[3] && touch[4])
        sign = 1;
     end
    else  begin 
    sign = 0; 
    pos_ball = 160;
    touch = 0;
    end   
end
always @(posedge clk) begin
    if(~reset_n || P==S_MAIN_INIT) begin
        R = 0;
        G = 0;
        B = 0;
    end
    else if(P==S_MAIN_RGB) begin
        if(btn_pressed[0]) R = R + 1;
        if(btn_pressed[1]) G = G + 1;
        if(btn_pressed[2]) B = B + 1;
    end
end
always @(posedge clk) begin
   if (~reset_n || fish_clock_2[31:21] > VBUF_W + FISH_W_2) begin
    fish_clock_2 <= 0;
  end
  else begin
    fish_clock_2 <= fish_clock_2 + 2;
  end
end
always @(posedge clk) begin
   if (~reset_n || fish_clock_1[31:21] > VBUF_W + FISH_W_1) begin
    fish_clock_1 <= 0;
  end
  else begin
    fish_clock_1 <= fish_clock_1 + 1;
  end
end
always @(posedge clk) begin
   if (~reset_n || fish_clock_4 < 1) begin
    fish_clock_4[31:21] <= VBUF_W + FISH_W_4;
  end
  else begin
    fish_clock_4 <= fish_clock_4 - 1;
  end
end
// End of the animation clock code.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// Video frame buffer address generation unit (AGU) with scaling control
// Note that the width x height of the fish image is 64x32, when scaled-up
// on the screen, it becomes 128x64. 'pos' specifies the right edge of the
// fish image.
assign fish_region =
           pixel_y >= (FISH_VPOS<<1) && pixel_y < (FISH_VPOS+FISH_H)<<1 &&
           (pixel_x + 127) >= pos[0] && pixel_x < pos[0] + 1;
assign fish_region_1 =
           pixel_y >= (FISH_VPOS_1<<1) && pixel_y < (FISH_VPOS_1+FISH_H_1)<<1 &&
           (pixel_x + 127) >= pos[1] && pixel_x < pos[1] + 1;
assign fish_region_2 =
            pixel_y >= (FISH_VPOS_2<<1) && pixel_y < (FISH_VPOS_2+FISH_H_2)<<1 &&
           (pixel_x + 127) >= pos[2] && pixel_x < pos[2] + 1;
assign fish_region_3 =
            pixel_y >= (FISH_VPOS_3<<1) && pixel_y < (FISH_VPOS_3+FISH_H_3)<<1 &&
           (pixel_x + 127) >= pos[3] && pixel_x < pos[3] + 1;
assign fish_region_4 =
            pixel_y >= (FISH_VPOS_4<<1) && pixel_y < (FISH_VPOS_4+FISH_H)<<1 &&
           (pixel_x + 127) >= pos[4] && pixel_x < pos[4] + 1;
assign pod_region =
            pixel_y >=(wir_pod_VPOS<<1) && pixel_y < (wir_pod_VPOS + pod_H)<<1 &&
            (pixel_x + 127) >= pos[5] && pixel_x <pos[5] + 1;
always @ (posedge clk) begin
  if (~reset_n) begin
    back_pixel_addr <= 0;
    fish1_pixel_addr <=0;
    fish2_pixel_addr <=0;
    fish3_pixel_addr <=0;
    fish4_pixel_addr <=0;
    fish5_pixel_addr <=0;
    pod_pixel_addr <= 0;
  end
  else 
  begin
      back_pixel_addr <= (pixel_y >> 1) * VBUF_W + (pixel_x >> 1);
      fish1_pixel_addr <= fish_addr[fish_clock[25:23]] +
                       ((pixel_y>>1)-FISH_VPOS)*FISH_W +
                       ((pixel_x +(FISH_W*2-1) - pos[0])>>1);
      fish2_pixel_addr <= fish_addr_1[fish_clock_1[25:23]] +
                       ((pixel_y>>1)-FISH_VPOS_1)*FISH_W_1 +
                       ((pixel_x +(FISH_W_1*2-1) - pos[1])>>1);
      fish3_pixel_addr <= fish_addr_2[fish_clock_2[25:23]] +
                       ((pixel_y>>1)-FISH_VPOS_2)*FISH_W_2 +
                       ((pixel_x +(FISH_W_2*2-1) - pos[2])>>1);
      fish4_pixel_addr <= fish_addr_1[fish_clock_1[25:23]] +
                       ((pixel_y>>1)-FISH_VPOS_3)*FISH_W_1 +
                       ((pixel_x +(FISH_W_3*2-1) - pos[3])>>1);
      fish5_pixel_addr <= fish_addr_4[fish_clock_4[25:23]] +
                       ((pixel_y>>1)-FISH_VPOS_4)*FISH_W +
                       ((pixel_x +(FISH_W*2-1) - pos[4])>>1);
      pod_pixel_addr <=  ((pixel_y>>1)-wir_pod_VPOS)*pod_W +
                       ((pixel_x +(pod_W*2-1) - pos[5])>>1);
    end
end
// End of the AGU code.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// Send the video data in the sram to the VGA controller
always @(posedge clk) begin
  if (pixel_tick) rgb_reg <= rgb_next;
end
reg change;
reg [0:4]touch;
always @(*) begin
  if (~video_on) begin
    rgb_next = 12'h000;
    change = 0;
  end // Synchronization period, must set RGB values to zero.
  else  begin
    if(fish_region && data_out[1] !=12'h0f0 && !touch[0]) rgb_next  = data_out[1];
    else if (fish_region_1 && data_out[2] !=12'h0f0 && !touch[1]) rgb_next  = data_out[2];
    else if (fish_region_2 && data_out[3] !=12'h0f0 && !touch[2]) rgb_next  = data_out[3];
    else if (fish_region_3 && data_out[2] !=12'h0f0 && !touch[3]) rgb_next  = data_out[2];
    else if (fish_region_4 && data_out[4] !=12'h0f0 && !touch[4]) rgb_next  = data_out[4];
    else if ((P==S_MAIN_MOVE || P==S_MAIN_WAIT) && pod_region && data_out[5] != 12'h0f0
    && data_out[5] != 12'h1d1 && data_out[5] != 12'h2c2 && data_out[5] != 12'h292 && data_out[5] != 12'h1e1 && data_out[5] != 12'h1f1) rgb_next = data_out[5];
    else rgb_next = data_out[0] + {R,G,B};
  end
     // RGB value at (pixel_x, pixel_y)
end
// End of the video data display code.
// ------------------------------------------------------------------------

endmodule
