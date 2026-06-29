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
 * in: 输入数据，位宽为Q_IN_INT+Q_IN_FRAC
 * out: 输出数据，位宽为Q_OUT_INT+Q_OUT_FRAC
 */
module UnsignedFixedPointNorm
#(
    parameter Q_IN_INT = 8,
    parameter Q_IN_FRAC = 8,
    parameter Q_OUT_INT = 8,
    parameter Q_OUT_FRAC = 8
)
(
    input [Q_IN_INT+Q_IN_FRAC-1:0] in,
    output [Q_OUT_INT+Q_OUT_FRAC-1:0] out
);
    wire [Q_IN_INT+Q_OUT_FRAC-1:0] norm_frac;
    generate
        if (Q_IN_FRAC > Q_OUT_FRAC)
        begin
            UnsignedRnd #(
                .DW_IN(Q_IN_INT + Q_IN_FRAC),
                .DW_OUT(Q_IN_INT + Q_OUT_FRAC)
            ) rnd_inst(
                .in(in),
                .out(norm_frac)
            );
        end
        else if (Q_IN_FRAC < Q_OUT_FRAC)
        begin
            assign norm_frac = {in, {(Q_OUT_FRAC - Q_IN_FRAC){1'b0}}};
        end
        else
            assign norm_frac = in;
    endgenerate

    generate
        if (Q_IN_INT > Q_OUT_INT)
        begin
            UnsignedSat #(
                .DW_IN(Q_IN_INT + Q_OUT_FRAC),
                .DW_OUT(Q_OUT_INT + Q_OUT_FRAC)
            ) sat_inst(
                .in(norm_frac),
                .out(out)
            );
        end
        else if (Q_IN_INT < Q_OUT_INT)
        begin
            assign out = { {(Q_OUT_INT - Q_IN_INT){1'b0}}, norm_frac };
        end
        else
        begin
            assign out = norm_frac;
        end
    endgenerate
endmodule

/** 有符号定点数标准化
 * in: 输入数据，位宽为Q_IN_INT+Q_IN_FRAC+1
 * out: 输出数据，位宽为Q_OUT_INT+Q_OUT_FRAC+1
 */
module SignedFixedPointNorm
#(
    parameter Q_IN_INT = 7,
    parameter Q_IN_FRAC = 8,
    parameter Q_OUT_INT = 7,
    parameter Q_OUT_FRAC = 8
)
(
    input [Q_IN_INT+Q_IN_FRAC:0] in,
    output [Q_OUT_INT+Q_OUT_FRAC:0] out
);
    wire [Q_IN_INT+Q_OUT_FRAC:0] norm_frac;
    generate
        if (Q_IN_FRAC > Q_OUT_FRAC)
        begin
            SignedRnd #(
                .DW_IN(Q_IN_INT + Q_IN_FRAC + 1),
                .DW_OUT(Q_IN_INT + Q_OUT_FRAC + 1)
            ) rnd_inst(
                .in(in),
                .out(norm_frac)
            );
        end
        else if (Q_IN_FRAC < Q_OUT_FRAC)
        begin
            assign norm_frac = {in, {(Q_OUT_FRAC-Q_IN_FRAC){1'b0}}};
        end
        else
            assign norm_frac = in;
    endgenerate

    generate
        if (Q_IN_INT > Q_OUT_INT)
        begin
            SignedSat #(
                .DW_IN(Q_IN_INT + Q_OUT_FRAC + 1),
                .DW_OUT(Q_OUT_INT + Q_OUT_FRAC + 1)
            ) sat_inst(
                .in(norm_frac),
                .out(out)
            );
        end
        else if (Q_IN_INT < Q_OUT_INT)
        begin
            assign out = { {(Q_OUT_INT-Q_IN_INT){norm_frac[Q_IN_INT+Q_OUT_FRAC]}}, norm_frac };
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
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B
 * out: 输出数据A+B，位宽为Q_OUT_INT+Q_OUT_FRAC
 */
module UnsignedFixedPointAdd
#(
    parameter Q_IN_INT_A = 8,
    parameter Q_IN_FRAC_A = 8,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A-1:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B-1:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC-1:0] out
);
    localparam Q_RES_INT = (Q_IN_INT_A > Q_IN_INT_B ? Q_IN_INT_A : Q_IN_INT_B) + 1;
    localparam Q_RES_FRAC = Q_IN_FRAC_A > Q_IN_FRAC_B ? Q_IN_FRAC_A : Q_IN_FRAC_B;
    reg [Q_RES_INT+Q_RES_FRAC-1:0] result;
    
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
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 无符号定点数减法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B
 * out: 输出数据A-B，位宽为Q_OUT_INT+Q_OUT_FRAC
 */
module UnsignedFixedPointSub
#(
    parameter Q_IN_INT_A = 8,
    parameter Q_IN_FRAC_A = 8,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A-1:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B-1:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC-1:0] out
);
    localparam Q_RES_INT = Q_IN_INT_A > Q_IN_INT_B ? Q_IN_INT_A : Q_IN_INT_B;
    localparam Q_RES_FRAC = Q_IN_FRAC_A > Q_IN_FRAC_B ? Q_IN_FRAC_A : Q_IN_FRAC_B;
    reg [Q_RES_INT+Q_RES_FRAC-1:0] result;

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
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 无符号定点数乘法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B
 * out: 输出数据A*B，位宽为Q_OUT_INT+Q_OUT_FRAC
 */
module UnsignedFixedPointMult
#(
    parameter Q_IN_INT_A = 8,
    parameter Q_IN_FRAC_A = 8,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A-1:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B-1:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC-1:0] out
);
    localparam Q_RES_INT = Q_IN_INT_A + Q_IN_INT_B;
    localparam Q_RES_FRAC = Q_IN_FRAC_A + Q_IN_FRAC_B;
    reg [Q_RES_INT+Q_RES_FRAC-1:0] result;

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
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 无符号定点数除法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B
 * out: 输出数据A/B，位宽为Q_OUT_INT+Q_OUT_FRAC
 */
module UnsignedFixedPointDiv
#(
    parameter Q_IN_INT_A = 8,
    parameter Q_IN_FRAC_A = 8,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A-1:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B-1:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC-1:0] out
);
    localparam Q_RES_INT = Q_IN_INT_A + Q_IN_FRAC_B;
    localparam Q_RES_FRAC = Q_IN_FRAC_A + Q_IN_INT_B;
    reg [Q_RES_INT+Q_RES_FRAC-1:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= {in_a, {(Q_IN_INT_B + Q_IN_FRAC_B){1'b0}}} / in_b;
        end
    end

    UnsignedFixedPointNorm #(
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数加法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A+1
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B+1
 * out: 输出数据A+B，位宽为Q_OUT_INT+Q_OUT_FRAC+1
 */
module SignedFixedPointAdd
#(
    parameter Q_IN_INT_A = 7,
    parameter Q_IN_FRAC_A = 7,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC:0] out
);
    localparam Q_RES_INT = (Q_IN_INT_A > Q_IN_INT_B ? Q_IN_INT_A : Q_IN_INT_B) + 1;
    localparam Q_RES_FRAC = Q_IN_FRAC_A > Q_IN_FRAC_B ? Q_IN_FRAC_A : Q_IN_FRAC_B;
    reg [Q_RES_INT+Q_RES_FRAC:0] result;
    
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
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数减法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A+1
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B+1
 * out: 输出数据A-B，位宽为Q_OUT_INT+Q_OUT_FRAC+1
 */
module SignedFixedPointSub
#(
    parameter Q_IN_INT_A = 7,
    parameter Q_IN_FRAC_A = 7,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC:0] out
);
    localparam Q_RES_INT = Q_IN_INT_A > Q_IN_INT_B ? Q_IN_INT_A : Q_IN_INT_B;
    localparam Q_RES_FRAC = Q_IN_FRAC_A > Q_IN_FRAC_B ? Q_IN_FRAC_A : Q_IN_FRAC_B;
    reg [Q_RES_INT+Q_RES_FRAC:0] result;

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
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数乘法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A+1
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B+1
 * out: 输出数据A*B，位宽为Q_OUT_INT+Q_OUT_FRAC+1
 */
module SignedFixedPointMult
#(
    parameter Q_IN_INT_A = 7,
    parameter Q_IN_FRAC_A = 7,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC:0] out
);
    localparam Q_RES_INT = Q_IN_INT_A + Q_IN_INT_B;
    localparam Q_RES_FRAC = Q_IN_FRAC_A + Q_IN_FRAC_B;
    reg [Q_RES_INT+Q_RES_FRAC:0] result;

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
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule

/** 有符号定点数除法
 * clk: 时钟信号
 * rstn: 复位信号，低电平有效
 * in_a: 输入数据A，位宽为Q_IN_INT_A+Q_IN_FRAC_A+1
 * in_b: 输入数据B，位宽为Q_IN_INT_B+Q_IN_FRAC_B+1
 * out: 输出数据A/B，位宽为Q_OUT_INT+Q_OUT_FRAC+1
 */
module SignedFixedPointDiv
#(
    parameter Q_IN_INT_A = 7,
    parameter Q_IN_FRAC_A = 7,
    parameter Q_IN_INT_B = Q_IN_INT_A,
    parameter Q_IN_FRAC_B = Q_IN_FRAC_A,
    parameter Q_OUT_INT = Q_IN_INT_A,
    parameter Q_OUT_FRAC = Q_IN_FRAC_A
)
(
    input clk, rstn,
    input [Q_IN_INT_A+Q_IN_FRAC_A:0] in_a,
    input [Q_IN_INT_B+Q_IN_FRAC_B:0] in_b,
    output [Q_OUT_INT+Q_OUT_FRAC:0] out
);
    localparam Q_RES_INT = Q_IN_INT_A + Q_IN_FRAC_B;
    localparam Q_RES_FRAC = Q_IN_FRAC_A + Q_IN_INT_B;
    reg [Q_RES_INT+Q_RES_FRAC:0] result;

    always @(posedge clk or negedge rstn)
    begin
        if (!rstn)
        begin
            result <= 0;
        end
        else
        begin
            result <= $signed({in_a, {(Q_IN_INT_B + Q_IN_FRAC_B){1'b0}}}) / $signed(in_b);
        end
    end

    SignedFixedPointNorm #(
        .Q_IN_INT(Q_RES_INT),
        .Q_IN_FRAC(Q_RES_FRAC),
        .Q_OUT_INT(Q_OUT_INT),
        .Q_OUT_FRAC(Q_OUT_FRAC)
    ) norm_inst(
        .in(result),
        .out(out)
    );
endmodule