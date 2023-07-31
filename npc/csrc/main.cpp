#include "Vtop.h"
#include <nvboard.h>

#ifndef TRACE_ENABLE
#define TRACE_ENABLE 0
#endif

#if TRACE_ENABLE
#include <verilated_vcd_c.h>
#endif

static Vtop dut;

void nvboard_bind_all_pins(Vtop *top);

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);

#if TRACE_ENABLE
    Verilated::traceEverOn(true);                      // 开启波形
    VerilatedContext *contextp = new VerilatedContext; // 环境
    VerilatedVcdC *tfp = new VerilatedVcdC;            // 波形
    dut.trace(tfp, 3);                                 // 深度3
    // mkdir(./vcd);
    tfp->open("./vcd/waveform.vcd"); // 波形文件
#endif

    nvboard_bind_all_pins(&dut);
    nvboard_init();

    while (1)
    {
        nvboard_update();
        dut.eval();
#if TRACE_ENABLE
        tfp->dump(contextp->time()); // dump数据
        contextp->timeInc(1);        // 增加仿真时间
#endif
    }
    dut.final();
#if TRACE_ENABLE
    tfp->close(); // 关闭trace对象以保存文件里的数据
#endif
    //    delete top;//释放内存
    return 0;
}
