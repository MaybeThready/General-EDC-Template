module main
(
    input clk_50M, rstn,
    output  [5:0] sel,
    output  [7:0] seg
);
    wire clk_100M;
    pll pll_inst
    (
        .inclk0(clk_50M),
        .c0(clk_100M)
    );

    wire clk_1k, clk_100H;
    DIV #100000 div1k(clk_100M, rstn, clk_1k);
    DIV #10 div100H(clk_1k, rstn, clk_100H);

    wire [18:0] count;
    CTU #1000000 ctu(clk_100H, rstn, count, );

    wire [23:0] bcd;
    BIN2BCD bin2bcd(count, bcd);

    BCDDGT dgt(clk_1k, rstn, bcd, 6'b000100, sel, seg);
endmodule
