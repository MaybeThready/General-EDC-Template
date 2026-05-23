/*
 * 组件模块，包含FPGA常用组件
 */

// N进制计数器
// clk: 时钟信号
// rstn: 复位信号，低电平有效
// count: 当前计数值，范围为0~N-1
// carry: 进位信号，当计数器从N-1回到0时，carry为1
// param N: 计数器的进制，默认为32
// param INITIAL_CNT: 计数器的初始值，默认为0
module CTU(
    input                           clk, rstn,
    output  reg [$clog2(N)-1 : 0]   count,
    output  reg                     carry
);
    parameter N = 32;
    parameter INITIAL_CNT = 0;
    always @(posedge clk or negedge rstn) begin
        if (!rstn) begin
            count <= INITIAL_CNT;
            carry <= 1'b0;
        end
        else
            if (count == N - 1) begin
                count <= 0;
                carry <= 1'b1;
            end
            else begin
                count <= count + 1'b1;
                carry <= 1'b0;
            end
    end
endmodule

// 分频器
// clk: 输入时钟信号
// rstn: 复位信号，低电平有效
// clk_div: 输出的分频时钟信号
// param NUM_DIV: 分频系数，必须为偶数，默认为10
module DIV(
    input        clk, rstn,
    output  reg  clk_div
);
    parameter NUM_DIV = 10;
    reg [$clog2(NUM_DIV>>1'b1)-1 : 0] count;
 
    always @(posedge clk or negedge rstn) begin
        if (!rstn) begin
            count <= 0;
            clk_div <= 1'b0;
        end
        else if (count < (NUM_DIV >> 1'b1) - 1) begin
            count <= count + 1'b1;
        end
        else begin
            count <= 0;
            clk_div <= ~clk_div;
        end
    end
endmodule

// 数码管显示器
// clk: 时钟信号
// rstn: 复位信号，低电平有效
// bcd: 24位BCD码输入，表示6位十进制数，每4位表示一个数字
// dots: 6位小数点控制信号，1表示对应位的小数点亮起，0表示不亮
// sel: 数码管位选信号，6位，低电平有效
// seg: 数码管段选信号，8位，低电平有效，最高位为小数点
module DGT(
    input               clk, rstn,
    input       [23:0]  bcd,
    input       [5:0]   dots,
    output  reg [5:0]   sel,
    output  reg [7:0]   seg
);
    parameter _0 = 7'b0111111, _1 = 7'b0000110, _2 = 7'b1011011,
              _3 = 7'b1001111, _4 = 7'b1100110, _5 = 7'b1101101,
              _6 = 7'b1111101, _7 = 7'b0000111, _8 = 7'b1111111,
              _9 = 7'b1101111, _X = 7'b0000000;
 
    always @(posedge clk or negedge rstn) begin
        if (!rstn)
            sel <= 6'b111111;
        else
            sel <= (sel == 6'b111111) ? 6'b111110 : {sel[4:0], sel[5]};
    end
    
    reg [3:0] dig_seg;
    always @(*) begin
        case (sel)
            6'b111110: dig_seg = bcd[3:0];
            6'b111101: dig_seg = bcd[7:4];
            6'b111011: dig_seg = bcd[11:8];
            6'b110111: dig_seg = bcd[15:12];
            6'b101111: dig_seg = bcd[19:16];
            6'b011111: dig_seg = bcd[23:20]; 
            default: dig_seg = 4'b1111;
        endcase
    end
 
    reg dot;
    always @(*) begin
        dot = (sel == 6'b111110) ? dots[0] :
              (sel == 6'b111101) ? dots[1] :
              (sel == 6'b111011) ? dots[2] :
              (sel == 6'b110111) ? dots[3] :
              (sel == 6'b101111) ? dots[4] :
              (sel == 6'b011111) ? dots[5] : 1'b0;
        case (dig_seg)
            0: seg = {dot, _0};
            1: seg = {dot, _1};
            2: seg = {dot, _2};
            3: seg = {dot, _3};
            4: seg = {dot, _4};
            5: seg = {dot, _5};
            6: seg = {dot, _6};
            7: seg = {dot, _7};
            8: seg = {dot, _8};
            9: seg = {dot, _9};
            default: seg = {dot, _X};
        endcase
    end
endmodule

// 二进制数转BCD码
// bin: 16位二进制输入
// bcd: 24位BCD码输出，表示6位十进制数，每4位表示一个数字
module BIN2BCD
(
    input       [15:0]  bin,
    output  reg [23:0]  bcd
);
    integer i;
    always @(*) begin
        bcd = 24'b0;
        for (i = 15; i >= 0; i = i - 1) begin
            if (bcd[3:0] > 4)
                bcd[3:0] = bcd[3:0] + 3;
            if (bcd[7:4] > 4)
                bcd[7:4] = bcd[7:4] + 3;
            if (bcd[11:8] > 4)
                bcd[11:8] = bcd[11:8] + 3;
            if (bcd[15:12] > 4)
                bcd[15:12] = bcd[15:12] + 3;
            if (bcd[19:16] > 4)
                bcd[19:16] = bcd[19:16] + 3;
            if (bcd[23:20] > 4)
                bcd[23:20] = bcd[23:20] + 3;
            bcd = {bcd, bin[i]};
        end
    end
endmodule

// 有限状态机
// clk: 时钟信号
// rstn: 复位信号，低电平有效
// stn: 下一个状态输入
// initial_state: 初始状态，复位时stc被设置为该值
// stc: 当前状态输出
module FSM
(
    input               clk, rstn,
    input       [$clog2(N)-1 : 0]   stn,
    input       [$clog2(N)-1 : 0]   initial_state,
    output  reg [$clog2(N)-1 : 0]   stc
);
    parameter N = 4;
    always @(posedge clk or negedge rstn) begin
        if (!rstn)
            stc <= initial_state;
        else
            stc <= stn;
    end
endmodule
