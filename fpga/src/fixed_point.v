/**
 * 定点数运算模块
 */

/** 无符号数据四舍五入模块
 * in: 输入数据，位宽为DW_IN
 * out: 输出数据，位宽为DW_OUT
 */
module UnsignedRnd
#(
    parameter DW_IN = 16,  // 输入数据位宽
    parameter DW_OUT = 8   // 输出数据位宽
)
(
    input [DW_IN-1:0] in,
    output [DW_OUT-1:0] out
);
    reg [DW_OUT-1:0] out_reg;
    always @(*)
    begin
        if (in[DW_IN-1:DW_IN-DW_OUT] == {{DW_OUT}{1'b1}})
        begin
            out_reg = in[DW_IN-1:DW_IN-DW_OUT];
        end
        else
        begin
            out_reg = in[DW_IN-1:DW_IN-DW_OUT] + in[DW_IN-DW_OUT-1];;
        end
    end

    assign out = out_reg;
endmodule

/** 无符号数据饱和模块
 * in: 输入数据，位宽为DW_IN
 * out: 输出数据，位宽为DW_OUT
 */
module UnsignedSat
#(
    parameter DW_IN = 16,
    parameter DW_OUT = 8
)
(
    input [DW_IN-1:0] in,
    output [DW_OUT-1:0] out
);
    reg [DW_OUT-1:0] out_reg;
    always @(*)
    begin
        if (in[DW_IN-1:DW_OUT] == {{DW_IN-DW_OUT}{1'b0}})
        begin
            out_reg = in[DW_OUT-1:0];
        end
        else
        begin
            out_reg = {DW_OUT{1'b1}};
        end
    end

    assign out = out_reg;
endmodule

/** 有符号数据四舍五入模块
* in: 输入数据，位宽为DW_IN
* out: 输出数据，位宽为DW_OUT
*/
module SignedRnd
#(
    parameter DW_IN = 16,
    parameter DW_OUT = 8
)
(
    input [DW_IN-1:0] in,
    output [DW_OUT-1:0] out
);
    reg [DW_OUT-1:0] out_reg;
    always @(*)
    begin
        if ((in[DW_IN-1:DW_IN-DW_OUT] == {1'b0, {(DW_OUT-1){1'b1}}}) || ((in[DW_IN-1] == 1'b1) && (in[DW_IN-DW_OUT-1:0] == {1'b1, {(DW_IN-DW_OUT-1){1'b0}}})))
        begin
            out_reg = in[DW_IN-1:DW_IN-DW_OUT];
        end
        else
        begin
            out_reg = in[DW_IN-1:DW_IN-DW_OUT] + in[DW_IN-DW_OUT-1];
        end
    end

    assign out = out_reg;
endmodule

/** 有符号数据饱和模块
 * in: 输入数据，位宽为DW_IN
 * out: 输出数据，位宽为DW_OUT
 */
module SignedSat
#(
    parameter DW_IN = 16,
    parameter DW_OUT = 8
)
(
    input [DW_IN-1:0] in,
    output [DW_OUT-1:0] out
);
    reg [DW_OUT-1:0] out_reg;
    always @(*)
    begin
        if (in[DW_IN-1] == 1'b1 && in[DW_IN-2:DW_OUT-1] != {(DW_IN-DW_OUT){1'b1}})
        begin
            out_reg = {1'b1, {(DW_OUT-1){1'b0}}};
        end
        else if (in[DW_IN-1] == 1'b0 && in[DW_IN-2:DW_OUT-1] != {(DW_IN-DW_OUT){1'b0}})
        begin
            out_reg = {1'b0, {(DW_OUT-1){1'b1}}};
        end
        else
        begin
            out_reg = in[DW_OUT-1:0];
        end
    end

    assign out = out_reg;
endmodule

/** 无符号定点数标准化
 * in: 输入数据，位宽为 QII+QIF
 * out: 输出数据，位宽为 QOI+QOF
 */
module UnsignedFixedPointNorm
#(
    parameter QII = 8,
    parameter QIF = 8,
    parameter QOI = 8,
    parameter QOF = 8
)
(
    input [QII+QIF-1:0] in,
    output [QOI+QOF-1:0] out
);
    wire [QII+QOF-1:0] norm_frac;
    generate
        if (QIF > QOF)
        begin
            UnsignedRnd #(
                .DW_IN(QII + QIF),
                .DW_OUT(QII + QOF)
            ) rnd_inst(
                .in(in),
                .out(norm_frac)
            );
        end
        else if (QIF < QOF)
        begin
            assign norm_frac = {in, {(QOF - QIF){1'b0}}};
        end
        else
            assign norm_frac = in;
    endgenerate

    generate
        if (QII > QOI)
        begin
            UnsignedSat #(
                .DW_IN(QII + QOF),
                .DW_OUT(QOI + QOF)
            ) sat_inst(
                .in(norm_frac),
                .out(out)
            );
        end
        else if (QII < QOI)
        begin
            assign out = { {(QOI - QII){1'b0}}, norm_frac };
        end
        else
        begin
            assign out = norm_frac;
        end
    endgenerate
endmodule

/** 有符号定点数标准化
 * in: 输入数据，位宽为QII+QIF+1
 * out: 输出数据，位宽为QOI+QOF+1
 */
module SignedFixedPointNorm
#(
    parameter QII = 7,
    parameter QIF = 8,
    parameter QOI = 7,
    parameter QOF = 8
)
(
    input [QII+QIF:0] in,
    output [QOI+QOF:0] out
);
    wire [QII+QOF:0] norm_frac;
    generate
        if (QIF > QOF)
        begin
            SignedRnd #(
                .DW_IN(QII + QIF + 1),
                .DW_OUT(QII + QOF + 1)
            ) rnd_inst(
                .in(in),
                .out(norm_frac)
            );
        end
        else if (QIF < QOF)
        begin
            assign norm_frac = {in, {(QOF-QIF){1'b0}}};
        end
        else
            assign norm_frac = in;
    endgenerate

    generate
        if (QII > QOI)
        begin
            SignedSat #(
                .DW_IN(QII + QOF + 1),
                .DW_OUT(QOI + QOF + 1)
            ) sat_inst(
                .in(norm_frac),
                .out(out)
            );
        end
        else if (QII < QOI)
        begin
            assign out = { {(QOI-QII){norm_frac[QII+QOF]}}, norm_frac };
        end
        else
        begin
            assign out = norm_frac;
        end
    endgenerate
endmodule

/** 无符号定点数加法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A
 * in_b: 输入数据B，位宽为QII_B+QIF_B
 * out: 输出数据A+B，位宽为QOI+QOF
 */
module UnsignedFixedPointAdd
#(
    parameter QII_A = 8,
    parameter QIF_A = 8,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A-1:0] in_a,
    input [QII_B+QIF_B-1:0] in_b,
    output [QOI+QOF-1:0] out
);
    localparam QRI = (QII_A > QII_B ? QII_A : QII_B) + 1;
    localparam QRF = QIF_A > QIF_B ? QIF_A : QIF_B;
    reg [QRI+QRF-1:0] result;
    
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= in_a + in_b;
        end
    end

    UnsignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 无符号定点数减法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A
 * in_b: 输入数据B，位宽为QII_B+QIF_B
 * out: 输出数据A-B，位宽为QOI+QOF
 */
module UnsignedFixedPointSub
#(
    parameter QII_A = 8,
    parameter QIF_A = 8,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A-1:0] in_a,
    input [QII_B+QIF_B-1:0] in_b,
    output [QOI+QOF-1:0] out
);
    localparam QRI = QII_A > QII_B ? QII_A : QII_B;
    localparam QRF = QIF_A > QIF_B ? QIF_A : QIF_B;
    reg [QRI+QRF-1:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= in_a - in_b;
        end
    end

    UnsignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 无符号定点数乘法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A
 * in_b: 输入数据B，位宽为QII_B+QIF_B
 * out: 输出数据A*B，位宽为QOI+QOF
 */
module UnsignedFixedPointMult
#(
    parameter QII_A = 8,
    parameter QIF_A = 8,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A-1:0] in_a,
    input [QII_B+QIF_B-1:0] in_b,
    output [QOI+QOF-1:0] out
);
    localparam QRI = QII_A + QII_B;
    localparam QRF = QIF_A + QIF_B;
    reg [QRI+QRF-1:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= in_a * in_b;
        end
    end

    UnsignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 无符号定点数除法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A
 * in_b: 输入数据B，位宽为QII_B+QIF_B
 * out: 输出数据A/B，位宽为QOI+QOF
 */
module UnsignedFixedPointDiv
#(
    parameter QII_A = 8,
    parameter QIF_A = 8,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A-1:0] in_a,
    input [QII_B+QIF_B-1:0] in_b,
    output [QOI+QOF-1:0] out
);
    localparam QRI = QII_A + QIF_B;
    localparam QRF = QIF_A + QII_B;
    reg [QRI+QRF-1:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= {in_a, {(QII_B + QIF_B){1'b0}}} / in_b;
        end
    end

    UnsignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数加法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A+1
 * in_b: 输入数据B，位宽为QII_B+QIF_B+1
 * out: 输出数据A+B，位宽为QOI+QOF+1
 */
module SignedFixedPointAdd
#(
    parameter QII_A = 7,
    parameter QIF_A = 7,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A:0] in_a,
    input [QII_B+QIF_B:0] in_b,
    output [QOI+QOF:0] out
);
    localparam QRI = (QII_A > QII_B ? QII_A : QII_B) + 1;
    localparam QRF = QIF_A > QIF_B ? QIF_A : QIF_B;
    reg [QRI+QRF:0] result;
    
    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= $signed(in_a) + $signed(in_b);
        end
    end

    SignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数减法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A+1
 * in_b: 输入数据B，位宽为QII_B+QIF_B+1
 * out: 输出数据A-B，位宽为QOI+QOF+1
 */
module SignedFixedPointSub
#(
    parameter QII_A = 7,
    parameter QIF_A = 7,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A:0] in_a,
    input [QII_B+QIF_B:0] in_b,
    output [QOI+QOF:0] out
);
    localparam QRI = QII_A > QII_B ? QII_A : QII_B;
    localparam QRF = QIF_A > QIF_B ? QIF_A : QIF_B;
    reg [QRI+QRF:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= $signed(in_a) - $signed(in_b);
        end
    end

    SignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数乘法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A+1
 * in_b: 输入数据B，位宽为QII_B+QIF_B+1
 * out: 输出数据A*B，位宽为QOI+QOF+1
 */
module SignedFixedPointMult
#(
    parameter QII_A = 7,
    parameter QIF_A = 7,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A:0] in_a,
    input [QII_B+QIF_B:0] in_b,
    output [QOI+QOF:0] out
);
    localparam QRI = QII_A + QII_B;
    localparam QRF = QIF_A + QIF_B;
    reg [QRI+QRF:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= $signed(in_a) * $signed(in_b);
        end
    end

    SignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数除法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为QII_A+QIF_A+1
 * in_b: 输入数据B，位宽为QII_B+QIF_B+1
 * out: 输出数据A/B，位宽为QOI+QOF+1
 */
module SignedFixedPointDiv
#(
    parameter QII_A = 7,
    parameter QIF_A = 7,
    parameter QII_B = QII_A,
    parameter QIF_B = QIF_A,
    parameter QOI = QII_A,
    parameter QOF = QIF_A
)
(
    input clk, rstn,
    input [QII_A+QIF_A:0] in_a,
    input [QII_B+QIF_B:0] in_b,
    output [QOI+QOF:0] out
);
    localparam QRI = QII_A + QIF_B;
    localparam QRF = QIF_A + QII_B;
    reg [QRI+QRF:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= $signed({in_a, {(QII_B + QIF_B){1'b0}}}) / $signed(in_b);
        end
    end

    SignedFixedPointNorm #(
        .QII(QRI),
        .QIF(QRF),
        .QOI(QOI),
        .QOF(QOF)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule