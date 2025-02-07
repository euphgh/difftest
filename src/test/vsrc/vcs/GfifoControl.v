`ifdef PALLADIUM

`define STEP_WIDTH 8

module GfifoControl(
  input clock,
  input reset,
  input [`STEP_WIDTH - 1:0] step,
  output reg simv_result
);

import "DPI-C" function int simv_result_fetch();
import "DPI-C" function void simv_nstep(int step);

initial $ixc_ctrl("gfifo", "simv_nstep");

reg [63:0] fetch_cycles;
initial fetch_cycles = 4999;

reg [63:0] fetch_timer;
always @(posedge clock) begin
  if (reset) begin
    simv_result <= 1'b0;
    fetch_timer <= 64'b0;
  end
  else begin
    if (fetch_timer == fetch_cycles) begin
      fetch_timer <= 1'b0;
      if (simv_result_fetch()) begin
        simv_result <= 1'b1;
      end
    end
    else begin
      fetch_timer <= fetch_timer + 64'b1;
    end

    if ((~simv_result) && (|step)) begin
      simv_nstep(step);
    end
  end
end

endmodule;
`endif // PALLADIUM
