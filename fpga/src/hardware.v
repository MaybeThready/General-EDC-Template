/**
 * 硬件模块
 */

/** BCD 数码管显示器
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * bcd: 24位BCD码输入，表示6位十进制数，每4位表示一个数字
 * dots: 6位小数点控制信号，1表示对应位的小数点亮起，0表示不亮
 * sel: 数码管位选信号，6位，低电平有效
 * seg: 数码管段选信号，8位，低电平有效，最高位为小数点
 */
module BCDDGT
(
    input clk, rstn,
    input [23:0] bcd,
    input [5:0] dots,
    output [5:0] sel,
    output [7:0] seg
);
    localparam _0 = 7'b0111111, _1 = 7'b0000110, _2 = 7'b1011011,
               _3 = 7'b1001111, _4 = 7'b1100110, _5 = 7'b1101101,
               _6 = 7'b1111101, _7 = 7'b0000111, _8 = 7'b1111111,
               _9 = 7'b1101111, _X = 7'b0000000;

    reg [5:0] r_sel;
    reg [7:0] r_seg;
 
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            r_sel <= 6'b111111;
        end
        else
        begin
            r_sel <= (r_sel == 6'b111111) ? 6'b111110 : {r_sel[4:0], r_sel[5]};
        end
    end
    
    reg [3:0] dig_seg;
    always @(*)
    begin
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
    always @(*)
    begin
        dot = (sel == 6'b111110) ? dots[0] :
              (sel == 6'b111101) ? dots[1] :
              (sel == 6'b111011) ? dots[2] :
              (sel == 6'b110111) ? dots[3] :
              (sel == 6'b101111) ? dots[4] :
              (sel == 6'b011111) ? dots[5] : 1'b0;
        case (dig_seg)
            4'd0: r_seg = {dot, _0};
            4'd1: r_seg = {dot, _1};
            4'd2: r_seg = {dot, _2};
            4'd3: r_seg = {dot, _3};
            4'd4: r_seg = {dot, _4};
            4'd5: r_seg = {dot, _5};
            4'd6: r_seg = {dot, _6};
            4'd7: r_seg = {dot, _7};
            4'd8: r_seg = {dot, _8};
            4'd9: r_seg = {dot, _9};
            default: r_seg = {dot, _X};
        endcase
    end

    assign sel = r_sel;
    assign seg = r_seg;
endmodule

/** HEX 数码管显示器
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * hex: 24位十六进制码输入，表示6位十进制数，每4位表示一个数字
 * dots: 6位小数点控制信号，1表示对应位的小数点亮起，0表示不亮
 * sel: 数码管位选信号，6位，低电平有效
 * seg: 数码管段选信号，8位，低电平有效，最高位为小数点
 */
module HEXDGT
(
    input clk, rstn,
    input [23:0] hex,
    input [5:0] dots,
    output [5:0] sel,
    output [7:0] seg
);
    localparam _0 = 7'b0111111, _1 = 7'b0000110, _2 = 7'b1011011,
               _3 = 7'b1001111, _4 = 7'b1100110, _5 = 7'b1101101,
               _6 = 7'b1111101, _7 = 7'b0000111, _8 = 7'b1111111,
               _9 = 7'b1101111, _A = 7'b1110111, _B = 7'b1111100,
               _C = 7'b0111001, _D = 7'b1011110, _E = 7'b1111001,
               _F = 7'b1110001, _X = 7'b0000000;

    reg [5:0] r_sel;
    reg [7:0] r_seg;
 
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            r_sel <= 6'b111111;
        end
        else
        begin
            r_sel <= (r_sel == 6'b111111) ? 6'b111110 : {r_sel[4:0], r_sel[5]};
        end
    end
    
    reg [3:0] dig_seg;
    always @(*)
    begin
        case (sel)
            6'b111110: dig_seg = hex[3:0];
            6'b111101: dig_seg = hex[7:4];
            6'b111011: dig_seg = hex[11:8];
            6'b110111: dig_seg = hex[15:12];
            6'b101111: dig_seg = hex[19:16];
            6'b011111: dig_seg = hex[23:20]; 
            default: dig_seg = 4'b1111;
        endcase
    end
 
    reg dot;
    always @(*)
    begin
        dot = (sel == 6'b111110) ? dots[0] :
              (sel == 6'b111101) ? dots[1] :
              (sel == 6'b111011) ? dots[2] :
              (sel == 6'b110111) ? dots[3] :
              (sel == 6'b101111) ? dots[4] :
              (sel == 6'b011111) ? dots[5] : 1'b0;
        case (dig_seg)
            4'd0: r_seg = {dot, _0};
            4'd1: r_seg = {dot, _1};
            4'd2: r_seg = {dot, _2};
            4'd3: r_seg = {dot, _3};
            4'd4: r_seg = {dot, _4};
            4'd5: r_seg = {dot, _5};
            4'd6: r_seg = {dot, _6};
            4'd7: r_seg = {dot, _7};
            4'd8: r_seg = {dot, _8};
            4'd9: r_seg = {dot, _9};
            4'd10: r_seg = {dot, _A};
            4'd11: r_seg = {dot, _B};
            4'd12: r_seg = {dot, _C};
            4'd13: r_seg = {dot, _D};
            4'd14: r_seg = {dot, _E};
            4'd15: r_seg = {dot, _F};
            default: r_seg = {dot, _X};
        endcase
    end

    assign sel = r_sel;
    assign seg = r_seg;
endmodule

/** UART 接收模块
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * rx: UART接收信号
 * data: 接收到的数据，8位
 * done: 数据接收完成信号，当接收到一个完整的数据字节时，done为1
 */
module UARTReceiver
#(
    parameter BAUD_RATE = 9600,
    parameter CLK_FREQ = 100_000_000
)
(
    input clk, rstn, rx,
    output [7:0] data,
    output done
);
    localparam BIT_PERIOD = CLK_FREQ / BAUD_RATE;
    localparam IDLE = 2'b00, START = 2'b01, DATA = 2'b10, STOP = 2'b11;
    localparam BIT_PERIOD_I = BIT_PERIOD;
    localparam HALF_PERIOD_I = (BIT_PERIOD_I >> 1);

    reg [7:0] r_data;
    reg r_done;
    assign data = r_data;
    assign done = r_done;

    wire rx_sync;
    SignalSync #(
        .INITIAL_VALUE(1'b1)
    ) sync_inst
    (
        .clk(clk),
        .rstn(rstn),
        .signal(rx),
        .signal_sync(rx_sync)
    );

    wire rx_negedge;
    NegedgeDetector negedge_inst
    (
        .clk(clk),
        .rstn(rstn),
        .signal_sync(rx_sync),
        .signal_negedge(rx_negedge)
    );

    wire [$clog2(BIT_PERIOD_I)-1:0] tick_count;
    reg [2:0] bit_index = 3'd0;

    reg tick_rstn = 1'b0;
    wire tick_carry;
    CTU #(BIT_PERIOD_I, 0) tick_counter
    (
        .clk(clk),
        .rstn(tick_rstn),
        .count(tick_count),
        .carry(tick_carry)
    );

    reg [1:0] stn;
    wire [1:0] stc;
    FSM #(
        .N(4),
        .INITIAL_STATE(IDLE)
    ) fsm_inst
    (
        .clk(clk),
        .rstn(rstn),
        .stn(stn),
        .stc(stc)
    );

    // 状态转移逻辑
    always @(*)
    begin
        case (stc)
            IDLE: stn = rx_negedge ? START : stn;

            START: stn = (tick_count == HALF_PERIOD_I) ? (rx_sync ? IDLE : DATA) : stn;

            DATA: stn = (tick_carry && bit_index == 3'd7) ? STOP : stn;

            STOP: stn = tick_carry ? IDLE : stn;

            default: stn = IDLE;
        endcase
    end

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            bit_index <= 3'd0;
            r_data <= 8'b0;
            r_done <= 1'b0;
            tick_rstn <= 1'b0;
        end
        else
        begin
            case (stc)
                IDLE:
                begin
                    r_done <= 1'b0;
                    bit_index <= 3'd0;
                    tick_rstn <= 1'b0;
                end

                START:
                begin
                    r_done <= 1'b0;
                    bit_index <= 3'd0;
                    if (tick_count == HALF_PERIOD_I && rx_sync == 1'b0)
                    begin
                        tick_rstn <= 1'b0;  // 重置计数器，准备循环计数
                    end
                    else
                    begin
                        tick_rstn <= 1'b1;  // 伪启动，保持计数器直至状态转移回空闲状态
                    end
                end

                DATA:
                begin
                    tick_rstn <= 1'b1;
                    r_done <= 1'b0;
                    if (tick_carry)
                    begin
                        r_data[bit_index] <= rx_sync;
                        if (bit_index == 3'd7)
                        begin
                            bit_index <= 3'd0;
                        end
                        else
                        begin
                            bit_index <= bit_index + 1'b1;
                        end
                    end
                end

                STOP:
                begin
                    tick_rstn <= 1'b1;
                    r_done <= tick_carry;  // 在停止位结束时输出done信号
                end

                default:
                begin
                    bit_index <= 3'd0;
                    tick_rstn <= 1'b0;
                    r_done <= 1'b0;
                end
            endcase
        end
    end
endmodule
