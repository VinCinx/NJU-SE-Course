
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
#include "const.h"
#include "protect.h"

#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                              schedule
 *======================================================================*/
//todo 新的调度策略
PUBLIC void schedule()
{
	//disp_str("S  ");
	// PROCESS* p;
	// int	 greatest_ticks = 0;

	// while (!greatest_ticks) {
	// 	for (p = proc_table; p < proc_table+NR_TASKS; p++) {
	// 		if(p->state==1){
	// 			if (p->ticks > greatest_ticks) {
	// 				//disp_str("X.");
	// 				greatest_ticks = p->ticks;
	// 				p_proc_ready = p;
	// 			}
	// 		}
	// 	}
	// }






	PROCESS *p;
	for (int i = 0; i < NR_TASKS; i++){
		p = proc_table + i;
		if (p->wakeup_tick>0 && p->wakeup_tick <= get_ticks() && p->blocked==0){//其实blocked是多余的..........
			p->wakeup_tick = -1;
			//必须加一个判断，就是p->wakeup_tick>0,这里加入队列的必须是被sleep过的！
			if (i != NR_TASKS - 1){
				readyQueue_push(i);
			}
		}
	}
	PROCESS *F = proc_table + NR_TASKS-1;
	if (F->wakeup_tick ==-1){
		p_proc_ready = F;
	}else{

		
			// for (int i = 0; i < readyQueue_pointer; i++)
			// {
			// 	disp_int(ready_queue[i]);
			// 	if (i == readyQueue_pointer-1)
			// 		disp_str(" ");
			// }
		

		int taskNum=readyQueue_pop();

		// disp_int(taskNum);//两行debug
		// disp_str("  ");

		readyQueue_push(taskNum);
		p_proc_ready=proc_table+taskNum;
	}
	if (p_proc_ready->r_w_type == 'r' || p_proc_ready->r_w_type == 'w')
	{ // 修改状态，供F打印
		RW_STATUS = p_proc_ready->r_w_type;
	}
	
	//
	// disp_int(readyQueue_pop());
	// PROCESS *select = proc_table + 2;
	// if (isRunnable(select))
	// {
	// 	//nothing
	// 	p_proc_ready = select;
	// }
	// else
	// {
	// 	while (!isRunnable(pre_proc))
	// 	{
	// 		pre_proc++;
	// 		if (pre_proc == select)
	// 		{
	// 			pre_proc = proc_table;
	// 		}
	// 	}
	// 	p_proc_ready = pre_proc;
	// 	pre_proc++;
	// 	if (pre_proc == select)
	// 	{
	// 		pre_proc = proc_table;
	// 	}
	// }
	// if (p_proc_ready->r_w_type == 'r' || p_proc_ready->r_w_type == 'w')
	// { // 修改状态，供F打印
	// 	RW_STATUS = p_proc_ready->r_w_type;
	// }
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}



/*======================================================================*
                           sys_sleep
 *======================================================================*/
PUBLIC int sys_sleep(int milli_seconds)
{
	//disp_str("sleep");
	p_proc_ready->sleep_start_tick = sys_get_ticks();
	p_proc_ready->wakeup_tick = get_ticks() + milli_seconds / (1000 / HZ);//当前的tivk加上延迟时间的tick
	if (p_proc_ready->pid != NR_TASKS-1){
		readyQueue_remove(p_proc_ready->pid);
	}
	schedule();
	return 0;
}

/*======================================================================*
                           sys_print
 *======================================================================*/
PUBLIC int sys_print(char *str)
{
	int offset = p_proc_ready - proc_table;
	switch (offset)
	{
	case 0:
		disp_color_str(str, RED);
		break;
	case 1:
		disp_color_str(str, GREEN);
		break;
	case 2:
		disp_color_str(str, BLUE);
		break;
	case 3:
		disp_color_str(str, YELLO);
		break;
	case 4:
		disp_color_str(str, PURPLE);
		break;
	default:
		disp_str(str);//F输出时也用普通颜色
		break;
	}
	return 0;
}

/*======================================================================*
                           sys_P
 *======================================================================*/
PUBLIC int sys_P(Semaphore *s)
{
	s->value--;
	if (s->value >= 0)
		return 0;

	// disp_str(" P!");//debug
	// disp_str(p_proc_ready->p_name);

	//小于等于0的时候说明已经没有可用资源了
	s->queue[-s->value-1] = p_proc_ready;//从queue的位置0处开始存储
	p_proc_ready->blocked = 1; // 阻塞
	readyQueue_remove(p_proc_ready->pid);
	schedule();
	return 0;
}

/*======================================================================*
                           sys_V
 *======================================================================*/
PUBLIC int sys_V(Semaphore *s)
{

	s->value++;
	if (s->value > 0) //与s->value++;在前面做有关
		return 0;
	


	//disp_str(s->queue[0]->p_name);//debug


	readyQueue_push(s->queue[0]->pid);

	//交换这个和前一个的位置，比如A的写里唤醒B，B加入到末尾，A写完再将A加入到末尾
	//通过这个交换可以实现
	//同样的，当A读完成时，唤醒写D，D加到末尾，A加到D的末尾
	//先不写改的代码，原来的是可以的

// if(s->value<=0){
// 	disp_str("*");
// 	for (int i = 0; i < readyQueue_pointer; i++)
// 	{
// 		disp_int(ready_queue[i]);
// 	}
// 	global0 = 1;
// 	disp_str("*");
// }
	

	PROCESS *processTowake = s->queue[0];
	//将等待队列的进程全部向前移动一个单位
	for (int i = 0; i < (-s->value); i++) //(-s->value)也与s->value++;在前面做有关
	{
		s->queue[i] = s->queue[i + 1];//queue后面的元素向前移动将0位置补全 
	}
	processTowake->blocked = 0;

	return 0;
}
