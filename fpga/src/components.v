/*
 * 组件模块，包含FPGA常用组件
 */

/** N进制计数器
* clk: 时钟信号
* rstn: 复位信号，低电平有效
* count: 当前计数值，范围为0~N-1
* carry: 进位信号，当计数器从N-1回到0时，carry为1
* param N: 计数器的进制，默认为32
* param INITIAL_CNT: 计数器的初始值，默认为0
*/
module CTU
#(
    parameter N = 32,
    parameter INITIAL_CNT = 0
)
(
    input clk, rstn,
    output [$clog2(N)-1:0] count,
    output carry
);
    reg [$clog2(N)-1:0] r_count;
    reg r_carry;
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            r_count <= INITIAL_CNT;
            r_carry <= 1'b0;
        end
        else if (count == N - 1)
        begin
            r_count <= 0;
            r_carry <= 1'b1;
        end
        else
        begin
            r_count <= r_count + 1'b1;
            r_carry <= 1'b0;
        end
    end

    assign count = r_count;
    assign carry = r_carry;
endmodule

/** 分频器
* clk: 输入时钟信号
* rstn: 复位信号，低电平有效
* clk_div: 输出的分频时钟信号
* param NUM_DIV: 分频系数，必须为偶数，默认为10
*/
module DIV
#(
    parameter NUM_DIV = 10
)
(
    input clk, rstn,
    output clk_div
);
    localparam MAX_CNT = (NUM_DIV >> 1) - 1;

    reg r_clk_div;
    reg [$clog2(NUM_DIV>>1'b1)-1 : 0] count;
 
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            count <= 0;
            r_clk_div <= 1'b0;
        end
        else if (count < MAX_CNT)
        begin
            count <= count + 1'b1;
        end
        else
        begin
            count <= 0;
            r_clk_div <= ~r_clk_div;
        end
    end

    assign clk_div = r_clk_div;
endmodule



/** 二进制数转BCD码
* param W: 二进制位宽
* bin: W位二进制输入
* bcd: BCD码输出，每4位表示一个十进制数
*/
module BIN2BCD
#(
    parameter W = 16
)
(
    input [W-1:0] bin,
    output [W+(W-4)/3:0] bcd
);
    reg [W+(W-4)/3:0] r_bcd;
    integer i, j;

    always @(bin)
    begin
        for (i = 0; i <= W + (W - 4) / 3; i = i + 1)
        begin
            r_bcd[i] = 0;
        end

        r_bcd[W-1:0] = bin;

        for (i = 0; i <= W - 4; i = i + 1)
        begin
            for (j = 0; j <= i / 3; j = j + 1)
            begin
                if (r_bcd[W-i+4*j-:4] > 4)
                begin
                    r_bcd[W-i+4*j-:4] = r_bcd[W-i+4*j-:4] + 4'd3;
                end
            end
        end
    end

    assign bcd = r_bcd;
endmodule

/** 有限状态机
* clk: 时钟信号
* rstn: 复位信号，低电平有效
* stn: 下一个状态输入
* initial_state: 初始状态，复位时stc被设置为该值
* stc: 当前状态
*/
module FSM
#(
    parameter N = 4,
    parameter INITIAL_STATE = 0
)
(
    input clk, rstn,
    input [$clog2(N)-1:0] stn,
    output [$clog2(N)-1:0] stc
);
    reg [$clog2(N)-1 : 0] r_stc;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            r_stc <= INITIAL_STATE;
        end
        else
        begin
            r_stc <= stn;
        end
    end

    assign stc = r_stc;
endmodule
