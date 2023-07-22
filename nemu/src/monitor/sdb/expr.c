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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <debug.h>
#include <regex.h>

enum token_type
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NEQ,
  TK_AND,
  TK_OR,
  TK_NOT,
  TK_NUM,
  TK_HEX,
  TK_REG,
  TK_POINTER,
  TK_MIN,
  /* TODO: Add more token types */

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */
    {"!=", TK_NEQ},    // not equal
    {"==", TK_EQ},     // equal
    {" +", TK_NOTYPE}, // spaces

    {"\\+", '+'}, // plus
    {"\\-", '-'}, //
    {"\\*", '*'}, // mul
    {"\\/", '/'}, // div

    {"\\(", '('}, //
    {"\\)", ')'}, //

    {"&&", TK_AND},    //
    {"\\|\\|", TK_OR}, //
    {"!", TK_NOT},     //

    {"0[xX][0-9a-fA-F]+", TK_HEX}, // 匹配顺序很重要
    {"[0-9]+", TK_NUM},            //
    {"\\$?[a-z0-9]+", TK_REG},     //
};

#define NR_REGEX ARRLEN(rules)
#define Tokens_Size 65536

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[Tokens_Size] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  for (size_t j = 0; j < Tokens_Size; j++)
  {
    memset(tokens[j].str, '\0', 32); // 新表达式清空tokens
  }

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (nr_token == Tokens_Size) // tokens缓冲区溢出
          Assert(0, "tokens缓冲区溢出");

        switch (rules[i].token_type)
        {
        case '+':
          tokens[nr_token].type = '+';
          nr_token++;
          break;
        case '-':
          tokens[nr_token].type = '-';
          nr_token++;
          break;
        case '*':
          tokens[nr_token].type = '*';
          nr_token++;
          break;
        case '/':
          tokens[nr_token].type = '/';
          nr_token++;
          break;
        case '(':
          tokens[nr_token].type = '(';
          nr_token++;
          break;
        case ')':
          tokens[nr_token].type = ')';
          nr_token++;
          break;
        case TK_AND:
          tokens[nr_token].type = TK_AND;
          strcpy(tokens[nr_token].str, "&&");
          nr_token++;
          break;
        case TK_OR:
          tokens[nr_token].type = TK_OR;
          strcpy(tokens[nr_token].str, "||");
          nr_token++;
          break;
        case TK_NOT:
          tokens[nr_token].type = TK_NOT;
          strcpy(tokens[nr_token].str, "!");
          nr_token++;
          break;
        case TK_NEQ:
          tokens[nr_token].type = TK_NEQ;
          strcpy(tokens[nr_token].str, "!=");
          nr_token++;
          break;
        case TK_EQ:
          tokens[nr_token].type = TK_EQ;
          strcpy(tokens[nr_token].str, "==");
          nr_token++;
          break;
        case TK_NOTYPE:
          break;
        case TK_NUM:
          if (substr_len > 32) // str缓冲区溢出
            Assert(0, "str缓冲区溢出");
          tokens[nr_token].type = TK_NUM;
          strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
          nr_token++;
          break;
        case TK_HEX:
          if (substr_len > 32) // str缓冲区溢出
            Assert(0, "str缓冲区溢出");
          tokens[nr_token].type = TK_HEX;
          strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
          nr_token++;
          break;
        case TK_REG:
          tokens[nr_token].type = TK_REG;
          strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
          nr_token++;
          break;
        default:
          Assert(0, "无匹配规则");
        }
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
static bool check_parentheses(int left, int right) // right传入nr_token,为token数+1,left传入0
{
  int left_count = 0, right_count = 0;
  if (tokens[left].type == '(' || tokens[right].type == ')')
  {
    for (; left < right; left++)
    {
      if (tokens[left].type == '(')
        left_count++;
      if (tokens[left].type == ')')
        right_count++;
      if (left != (right - 1) && left_count == right_count)
        return false;
    }
    if (left_count == right_count)
      return true;
    else
      return false;
  }
  return false;
}

static word_t str2ix(int current)
{
  word_t result = 0;
  if (tokens[current].type == TK_NUM)
  {
    sscanf(tokens[current].str, "%d", &result);
  }
  else if (tokens[current].type == TK_HEX)
  {
    int i = 2;
    while (tokens[current].str[i] != 0)
    {
      result *= 16;
      result += tokens[current].str[i] <= '9' ? tokens[current].str[i] - '0' : tokens[current].str[i] - 'a' + 10;
      i++;
    }
  }
  // printf("result = %d\n", result);
  return result;
}

static int main_op(int left, int right)
{
  int step = 0;
  int op = -1;
  int flag = 1000;
  for (; left < right; left++)
  {
    if (tokens[left].type == '(')
      step++;
    else if (tokens[left].type == ')')
      step--;

    if (step == 0)
    {
      if (tokens[left].type == TK_OR && flag >= 0)
      {
        op = left;
        flag = 0;
      }
      else if (tokens[left].type == TK_AND && flag >= 1)
      {
        op = left;
        flag = 1;
      }
      else if ((tokens[left].type == TK_EQ || tokens[left].type == TK_NEQ) && flag >= 2)
      {
        op = left;
        flag = 2;
      }
      else if ((tokens[left].type == '+' || tokens[left].type == '-') && flag >= 3)
      {
        op = left;
        flag = 3;
      }
      else if ((tokens[left].type == '*' || tokens[left].type == '/') && flag >= 4)
      {
        op = left;
        flag = 4;
      }
      // else if ((tokens[left].type == TK_POINTER || tokens[left].type == TK_MIN) && flag >= 5)
      // {
      //     op = left;
      //     flag = 5;
      // }
    }
    else if (step < 0)
    {
      return -2;
    }
  }
  return op;
}

static word_t eval(int left, int right)
{
  // printf("left = %d, right = %d\n", left, right);
  word_t val1 = 0, val2 = 0;
  int op;
  static bool success = false;
  if (left > right - 1) // 非法
  {
    /* Bad expression */
    Assert(0, "error in eval 0");
  }
  else if (left == right - 1 && (tokens[left].type == TK_NUM || tokens[left].type == TK_HEX)) // 单数字
  {
    // printf("str2ix\n");
    return str2ix(left);
  }
  else if (check_parentheses(left, right) == true) // 去括号
  {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(left + 1, right - 1);
  }
  else
  {
    op = main_op(left, right); // the position of 主运算符 in the token expression;
    // printf("op = %d\n", op);

    if (op == -2) // 非法表达式
      Assert(0, "error in eval 1");
    else if (op == -1)
    {
      if (tokens[left].type == TK_MIN && tokens[left + 1].type == TK_MIN)
        return eval(left + 2, right); //--连减
      else if (tokens[left].type == TK_MIN)
      {
        return -str2ix(left + 1); //-负数
      }
      else if (tokens[left].type == TK_NOT)
      {
        return !str2ix(left + 1); // 非
      }
      else if (tokens[left].type == TK_POINTER)
      {
        word_t ret = isa_reg_str2val(tokens[left + 1].str, &success);
        return success ? ret : 0;
      }
    }
    val2 = eval(op + 1, right);
    val1 = eval(left, op);
    // printf("val1 = %d, val2 = %d\n", val1, val2);
    switch (tokens[op].type)
    {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case '/':
      return val1 / val2;
    case TK_NEQ:
      return val1 != val2;
    case TK_EQ:
      return val1 == val2;
    case TK_AND:
      return val1 && val2;
    case TK_OR:
      return val1 || val2;

    default:
      Assert(0, "error in eval 2");
    }
  }
  return 0;
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  for (size_t i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == '-' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HEX && tokens[i - 1].type != ')')))
    {
      tokens[i].type = TK_MIN;
    }
    else if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HEX && tokens[i - 1].type != ')')))
    {
      tokens[i].type = TK_POINTER;
    }
  }
  return eval(0, nr_token);
  /* TODO: Insert codes to evaluate the expression. */
}
