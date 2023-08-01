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
#include <debug.h>
#define NR_WP 32

static WP *head = NULL, *free_ = NULL;
static WP wp_pool[NR_WP] = {};

void init_wp_pool()
{
	int i;
	for (i = 0; i < NR_WP; i++)
	{
		wp_pool[i].NO = i;
		wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
		// wp_pool[i].args = '\0';
		// wp_pool[i].result = 0;
	}

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp()
{
	if (free_ == NULL)
	{
		// Assert(0, "无空闲节点"); // 无空闲节点
		// printf("无空闲节点\n");
		return NULL;
	}
	WP *temp = free_;
	free_ = free_->next;

	temp->next = head; // 头插法
	head = temp;
	return head;
}

// void free_wp(WP *wp)
// {
// 	// if (wp == NULL)
// 	// 	Assert(0, "试图free空指针");
// 	head = wp->next;
// 	wp->next = free_;
// 	free_ = wp;
// 	return;
// }
void free_wp(WP *wp)
{
	// if (wp == NULL)
	// {
	// 	printf("试图free空指针\n");
	// 	return;
	// 	// Assert(0, "试图free空指针");
	// }

	WP *prev = NULL;
	WP *curr = head;

	// Find the watchpoint in the list
	while (curr != NULL)
	{
		if (curr == wp)
		{
			if (prev == NULL)
			{
				head = head->next;
			}
			else
			{
				prev->next = curr->next;
			}

			wp->next = free_;
			free_ = wp;
			return;
		}

		prev = curr;
		curr = curr->next;
	}

	// Watchpoint not found in the list
	// Assert(0, "试图free不存在于链表中的WP结构体");
}

void print_wp()
{
	WP *temp = head;
	if (temp == NULL)
	{
		printf("No watchpoints\n");
	}
	while (temp != NULL)
	{
		printf("Watch point [%d]: [%s] value:[%d]\n", temp->NO, temp->expr, temp->value);
		temp = temp->next;
	}
}

WP *find_wp_no(int no, bool *success)
{
	WP *temp = head;
	while (temp != NULL && temp->NO != no)
	{
		temp = temp->next;
	}
	if (temp == NULL)
	{
		*success = false;
	}
	return temp;
}

WP *wp_head()
{
	return head;
}
