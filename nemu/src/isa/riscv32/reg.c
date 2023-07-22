/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "local-include/reg.h"
#include <isa.h>

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

// 打印寄存器当前值
void isa_reg_display()
{
  printf("pc       %-#x    %-d\n", cpu.pc, cpu.pc);
  for (sword_t i = 0; i < 32; i++)
  {
    printf("%-3s      %-#10x    %-d\n", regs[i], gpr(i), gpr(i));
  }
}

// 返回名称为name的寄存器的当前值, 并设置success为true; 否则设置success为false
word_t isa_reg_str2val(const char *name, bool *success)
{

  if (!strcmp(name, "$0"))
  {
    *success = true;
    return gpr(0);
  }
  else if (!strcmp(name, "ra"))
  {
    *success = true;
    return gpr(1);
  }
  else if (!strcmp(name, "sp"))
  {
    *success = true;
    return gpr(2);
  }
  else if (!strcmp(name, "gp"))
  {
    *success = true;
    return gpr(3);
  }
  else if (!strcmp(name, "tp"))
  {
    *success = true;
    return gpr(4);
  }
  else if (!strcmp(name, "t0"))
  {
    *success = true;
    return gpr(5);
  }
  else if (!strcmp(name, "t1"))
  {
    *success = true;
    return gpr(6);
  }
  else if (!strcmp(name, "t2"))
  {
    *success = true;
    return gpr(7);
  }
  else if (!strcmp(name, "s0"))
  {
    *success = true;
    return gpr(8);
  }
  else if (!strcmp(name, "s1"))
  {
    *success = true;
    return gpr(9);
  }
  else if (!strcmp(name, "a0"))
  {
    *success = true;
    return gpr(10);
  }
  else if (!strcmp(name, "a1"))
  {
    *success = true;
    return gpr(11);
  }
  else if (!strcmp(name, "a2"))
  {
    *success = true;
    return gpr(12);
  }
  else if (!strcmp(name, "a3"))
  {
    *success = true;
    return gpr(13);
  }
  else if (!strcmp(name, "a4"))
  {
    *success = true;
    return gpr(14);
  }
  else if (!strcmp(name, "a5"))
  {
    *success = true;
    return gpr(15);
  }
  else if (!strcmp(name, "a6"))
  {
    *success = true;
    return gpr(16);
  }
  else if (!strcmp(name, "a7"))
  {
    *success = true;
    return gpr(17);
  }
  else if (!strcmp(name, "s2"))
  {
    *success = true;
    return gpr(18);
  }
  else if (!strcmp(name, "s3"))
  {
    *success = true;
    return gpr(19);
  }
  else if (!strcmp(name, "s4"))
  {
    *success = true;
    return gpr(20);
  }
  else if (!strcmp(name, "s5"))
  {
    *success = true;
    return gpr(21);
  }
  else if (!strcmp(name, "s6"))
  {
    *success = true;
    return gpr(22);
  }
  else if (!strcmp(name, "s7"))
  {
    *success = true;
    return gpr(23);
  }
  else if (!strcmp(name, "s8"))
  {
    *success = true;
    return gpr(24);
  }
  else if (!strcmp(name, "s9"))
  {
    *success = true;
    return gpr(25);
  }
  else if (!strcmp(name, "s10"))
  {
    *success = true;
    return gpr(26);
  }
  else if (!strcmp(name, "s11"))
  {
    *success = true;
    return gpr(27);
  }
  else if (!strcmp(name, "t3"))
  {
    *success = true;
    return gpr(28);
  }
  else if (!strcmp(name, "t4"))
  {
    *success = true;
    return gpr(29);
  }
  else if (!strcmp(name, "t5"))
  {
    *success = true;
    return gpr(30);
  }
  else if (!strcmp(name, "t6"))
  {
    *success = true;
    return gpr(31);
  }
  return 0;
}
