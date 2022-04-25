
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].priority = 15;
	proc_table[1].ticks = proc_table[1].priority =  5;
	proc_table[2].ticks = proc_table[2].priority =  3;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

	init_clock();
    init_keyboard();
	cleanScreen();

	restart();

	while(1){}
}


// 清屏，将一些不需要显示的地址删除掉
PUBLIC void cleanScreen()
{
	disp_pos = 0;
	int i;
	for (i = 0; i < SCREEN_SIZE; ++i)
	{
		disp_str(" ");
	}
	disp_pos = 0;
}
//一个任务要执行的方法，要添加到proc.h里    然后才能在global.c里调用
void cleanScreenMethod()
{
	int i = 0;
	while (1)
	{
		if (mode == 0)
		{
			cleanScreen();
			init_all_screen();
			milli_delay(70000);//等20s再循环  0.001*1000*20=20？  20000 多个0...
		}
		else
		{
			milli_delay(10); //20s后只间隔很短的时间就循环，执行前面的if (mode == 0)，询问mode是不是普通模式，一旦进入普通模式就清空屏幕
		}
	}
}/*这种延时方式在esc回到普通模式的时候，如果已经到了20s，立即清除，并重新开始计时20s，进入esc模式后依然计时*/
/*如果想要不计算esc模式的时间，每次mode为0才delay一个很短的时间，delay一定的次数才能达到20s    mode不为0的时候不delay*/
void cleanScreenMethod1(){
	int i = 0;
	int count=0;
	while (1)
	{
		if (mode == 0)
		{
			count+=1;
			if(count==4000 && mode==0){//？因为有count的开销，这个数比较小？？？？？？？？？？？？？？？？？
				//已经增加到1500，但是还没处理到清屏，此时按下esc，照样清屏
				//在上面的括号里增加mode==0解决在count+=1，if之前按下esc的情况
				if(mode==0){//这个if和上一个if之间按下esc的情况解决
				//在这里按下esc又会出现问题
				//要想完全没有问题，必须在做这个的时候别的任务停止处理
					cleanScreen();
				}
				init_all_screen();
				count=0;
			}
			milli_delay(10);
		}
	}
}
/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int i = 0;
	while (1) {
		/* disp_str("A."); */
		milli_delay(10);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0x1000;
	while(1){
		/* disp_str("B."); */
		milli_delay(10);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestC()
{
	int i = 0x2000;
	while(1){
		/* disp_str("C."); */
		milli_delay(10);
	}
}
