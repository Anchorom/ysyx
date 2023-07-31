#include <stdio.h>
#include <verilated.h>
#include "Vtop.h"

#ifndef TRACE_ENABLE
#define TRACE_ENABLE 1
#endif

#if TRACE_ENABLE
#include <verilated_vcd_c.h>
#endif

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    VerilatedContext *contextp = new VerilatedContext;//环境
    Vtop *top = new Vtop;//模块

#if TRACE_ENABLE
    contextp->traceEverOn(true);//开启波形
    VerilatedVcdC *tracep = new VerilatedVcdC;//波形
    top->trace(tracep,3);//深度3
    //mkdir(./vcd);
    tracep->open("./vcd/waveform.vcd");//波形文件
#endif
    while (!contextp->gotFinish()) {
        int a = rand() & 1;
        int b = rand() & 1;
        top->a = a;
        top->b = b;
        top->eval();//刷新电路状态
        printf("a = %d, b = %d, f = %d\n", a, b, top->f);
#if TRACE_ENABLE
        tracep->dump(contextp->time());//dump数据
        contextp->timeInc(1);//增加仿真时间
#endif
        assert(top->f == (a ^ b));
    }
    top->final();
#if TRACE_ENABLE
    tracep->close();//关闭trace对象以保存文件里的数据
#endif
    delete top;//释放内存
    return 0;
}
