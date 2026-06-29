/**
 * 信号处理模块
 */

/** 异步信号同步器
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * signal: 输入的异步信号
 * signal_sync: 输出的同步信号
 */
module SignalSync
#(
    parameter INITIAL_VALUE = 0
)
(
    input clk, rstn, signal,
    output signal_sync
);
    reg signal_buff1, signal_buff2;
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            signal_buff1 <= INITIAL_VALUE;
            signal_buff2 <= INITIAL_VALUE;
        end
        else
        begin
            signal_buff1 <= signal;
            signal_buff2 <= signal_buff1;
        end
    end

    assign signal_sync = signal_buff2;
endmodule

/** 上升沿检测器
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * signal_sync: 输入的同步信号。模块内部为了避免延迟没有进行二次同步，因此要求输入信号已经过同步处理
 * signal_posedge: 输出的上升沿检测信号，当signal_sync从0变为1时，signal_posedge为1
 */
module PosedgeDetector
(
    input clk, rstn, signal_sync,
    output signal_posedge
);
    reg signal_sync_prev;
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            signal_sync_prev <= 1'b0;
        end
        else
        begin
            signal_sync_prev <= signal_sync;
        end
    end

    assign signal_posedge = signal_sync & ~signal_sync_prev;
endmodule

/** 下降沿检测器
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * signal_sync: 输入的同步信号。模块内部为了避免延迟没有进行二次同步，因此要求输入信号已经过同步处理
 * signal_negedge: 输出的下降沿检测信号，当signal_sync从1变为0时，signal_negedge为1
 */
module NegedgeDetector
(
    input clk, rstn, signal_sync,
    output signal_negedge
);
    reg signal_sync_prev;
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            signal_sync_prev <= 1'b0;
        end
        else
        begin
            signal_sync_prev <= signal_sync;
        end
    end

    assign signal_negedge = ~signal_sync & signal_sync_prev;
endmodule
