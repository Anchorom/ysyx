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

#include "sdb.h"
#include <cpu/cpu.h>
#include <isa.h>
#include <memory/vaddr.h>
#include <readline/history.h>
#include <readline/readline.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void load_print(char *args);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
  sword_t steps = 1;
  char *arg = strtok(NULL, " "); // 第一个参数为NULL时,从函数保存的指针处开始分解,分解原理为将分隔符替换为'\0'
  char op;
  if (arg == NULL)
    ;
  else
  {
    op = *arg;
    sword_t op_int = atoi(&op);
    if (isdigit(op) && op_int > 0)
      steps = op_int;
    else
    {
      printf("Wrong si option! default option == 1\n");
    }
  }
  cpu_exec(steps);
  return 0;
};

static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  char op;
  if (arg == NULL)
  {
    printf("Need option!\n");
    return 0;
  }
  else
  {
    op = *arg;
    if (op == 'r')
      isa_reg_display();
    else if (op == 'w')
    {
      printf("w'info:...\n");
    }
  }
  return 0;
};

static int cmd_x(char *args)
{
  char *first_arg = strtok(NULL, " ");
  char *second_arg = strtok(NULL, " ");
  int step;
  vaddr_t addr;
  if (first_arg == NULL || second_arg == NULL)
  {
    printf("Need option!\n");
    return 0;
  }
  else
  {
    sscanf(first_arg, "%d", &step);
    sscanf(second_arg, "%x", &addr);
    for (; step > 0; step--)
    {
      printf("%#x:    ", addr);
      printf("%#08x\n", vaddr_read(addr, 4));
      addr += 4;
    }
  }
  return 0;
};

static int cmd_p(char *args)
{
  bool success = true;
  word_t result = expr(args, &success);
  if (success)
    printf("%d\n", result);
  return 0;
};

static int cmd_w(char *args)
{
  char *arg = strtok(NULL, " ");
  load_print(arg);
  return 0;
};

// static int cmd_d(char *args)
// {
// 	return 0;
// };
static struct
{
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "单步执行:si N", cmd_si},
    {"info", "输出程序状态:info r", cmd_info},
    {"x", "扫描内存:x N EXPR", cmd_x},
    {"p", "表达式求值:p EXPR", cmd_p},
    {"w", "添加监视点,值变化时暂停:w EXPR", cmd_w},
    //{"d","删除监视点:d N",cmd_d},

    /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(args, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode()
{
  is_batch_mode = true;
}

void sdb_mainloop()
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " "); // 以空格分离为字符串组
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb()
{
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
