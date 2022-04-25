
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

// #include "type.h"
// #include "const.h"
// #include "protect.h"
// #include "proto.h"
// #include "string.h"
// #include "proc.h"
// #include "global.h"
#include "type.h"
#include "const.h"
#include "protect.h"

#include "string.h"
#include "proc.h"
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
		p_proc->state=1;//新加的，一开始没有执行delay，每个进程都有机会被分到时间片
		p_proc->wakeup_tick=-1;
		p_proc->blocked=0;

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

		p_proc->regs.eip = (u32)p_task->initial_eip;/* 设置进程开始执行时从Test*的入口地址开始执行 */
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

/* todo-finish，各种锁的初始化，以及各个进程ticks的初始化 */
	
	//初始化记录正在读写进程数的变量
	reader_count = 0;
	writer_count = 0;
	reading_count=0;

	//初始化各种锁
	readerCount_signal.value=1;
	writerCount_signal.value=1;

	/*多个读者同时读同一本书，最大同时读的进程可以在这里修改*/
	readerNum_lock.value=3;
	write_lock.value=1;
	RW_lock.value=1;

	FAIR_lock.value = 1; //暂定的

	/*
proc_table赋值的中间注释
	。。。。。。。。。这里的tick每执行一次就会减去1，判断有没有分配时间片的依据    	每个进程里delay10ms，也就是一个时间中断的间隔，一个时间中断只会是的一个ticks减去1
	初始化需要的时间片
	如果需要实现时间片等于0的，下面加一条判断，不将它加入任务就可以了...
	*/
	

/* 写者优先  */
	proc_table[0].ticks = proc_table[0].time_slice_needed = 2;
	proc_table[1].ticks = proc_table[1].time_slice_needed = 3;
	proc_table[2].ticks = proc_table[2].time_slice_needed = 3;
	proc_table[3].ticks = proc_table[3].time_slice_needed = 3;
	proc_table[4].ticks = proc_table[4].time_slice_needed = 4;
	proc_table[5].ticks = proc_table[5].time_slice_needed = 1;
	proc_table[0].r_w_type = proc_table[1].r_w_type = proc_table[2].r_w_type = 'r';
	proc_table[3].r_w_type = proc_table[4].r_w_type = 'w';
/* 读者优先 */
	// proc_table[0].ticks = proc_table[0].time_slice_needed = 3;
	// proc_table[1].ticks = proc_table[1].time_slice_needed = 4;
	// proc_table[2].ticks = proc_table[2].time_slice_needed = 2;
	// proc_table[3].ticks = proc_table[3].time_slice_needed = 3;
	// proc_table[4].ticks = proc_table[4].time_slice_needed = 3;
	// proc_table[5].ticks = proc_table[5].time_slice_needed = 1;
	// proc_table[0].r_w_type = proc_table[1].r_w_type = 'w';
	// proc_table[2].r_w_type = proc_table[3].r_w_type = proc_table[3].r_w_type = 'r';
/* 解决一些饿死问题的 */
	// proc_table[0].ticks = proc_table[0].time_slice_needed = 2;
	// proc_table[1].ticks = proc_table[1].time_slice_needed = 3;
	// proc_table[2].ticks = proc_table[2].time_slice_needed = 3;
	// proc_table[3].ticks = proc_table[3].time_slice_needed = 3;
	// proc_table[4].ticks = proc_table[4].time_slice_needed = 4;
	// proc_table[5].ticks = proc_table[5].time_slice_needed = 1;
	// proc_table[0].r_w_type = proc_table[1].r_w_type = proc_table[2].r_w_type = 'r';
	// proc_table[3].r_w_type = proc_table[4].r_w_type = 'w';

	//初始化就绪队列
	readyQueue_pointer = 0;
	for (i = 0; i < NR_TASKS - 1; ++i){
		readyQueue_push(proc_table[i].pid);
	}


	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table; /* 为这个变量赋值，这个变量在restart里会调用到，它是一个指向进程表（PROCESS）的指针    这里是一个数组 */

	/* 初始化 8253 PIT */
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
	out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));

	put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);					   /* 让8259A可以接收时钟中断 */

	//清屏 
	disp_pos = 0;
	for (i = 0; i < 80 * 25; i++)
	{
		disp_str(" ");
	}
	disp_pos = 0;
	//disp_str(" ");  //调整清屏后打印后奇怪的光标位置？  应该需要像第三次作业一样控制cursor写显存才可以

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int i = 0;
	while (1) {
		disp_str("A.");
		new_milli_delay(100);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0x1000;
	while(1){
		disp_str("B.");
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
		disp_str("C.");
		milli_delay(10);
	}
}





/* 下面是新加的方法，所有的这些方法都要加到proto.h里，不然在别的地方不可以访问到 */
void F(){
	while(1){
		//实际上第二屏、第四屏这些偶数屏是看不到的，但是如果满一个屏就清空，会导致清得太了  看不见后半屏
		// if(disp_pos>=80*25*2){
		// 	disp_pos = 0;
		// 	for (int i = 0; i < 80 * 25; i++)
		// 	{
		// 		disp_str(" ");
		// 	}
		// 	disp_pos = 0;
		// }
		
		if (RW_STATUS == 'r')
		{
			// print(p_proc_ready->p_name);

			// //如果在读，则打印有几个进程在读
			// char num = '0' + reader_count - 0;
			// char tem[3] = {num, ' ', '\0'};
			// print("  notes: ");
			// print(tem);

			// print(" READING NOW");
			// print(New_Line);
			print("<read==");
			char num = '0' + reading_count -0;
			char tem[4] = {num, '>', ' ', '\0'};
			print(tem);
		}
		else if (RW_STATUS == 'w')
		{
			// print(p_proc_ready->p_name);
			// print("  notes: WRITING NOW");
			// print(New_Line);
			print("<writing> ");
		}
		else
		{
			// print(p_proc_ready->p_name);
			// print(" <START>");
			// print(New_Line);
			print("<<START>> ");
		}
		sleep(10);//间隔一个时间片再打印
	}
}

/* readFirst */
void Reader_kind1(){
	while(1){
		
/*  todo 这里可能有问题，最大3个同时写的时候，读过后reading没有出现3*/

		//判断是否已经申请文件资源   相当于是否需要在现在去剥夺写锁
		P(&readerCount_signal);
		if(reader_count==0)
			P(&write_lock);
		reader_count++;
		//循环的时候用的p_proc_ready->time_slice_needed一直没变，所以下面的取不取消注释都会打印满屏幕
		//todo 要不要在这里判断，ticks为0是否会被调度到
		// if(p_proc_ready->ticks==0)
		// 	p_proc_ready->ticks=p_proc_ready->time_slice_needed;
		V(&readerCount_signal);
		//end


		//写者优先的P必须放在这里，否则只允许两个同时写的时候，C不会预定占用写锁，AB全部完成时先释放写锁，再释放readerNum，写会优先，改一下就可以看出来
		P(&readerNum_lock); //放在外层，否则，只允许两个读得时候，第三个进来也会将readercount+1，错误的
		reading_count++;
		print(p_proc_ready->p_name);
		print(" start.  ");
		for(int timeSlice=0; timeSlice<p_proc_ready->time_slice_needed; timeSlice++){
			if(timeSlice==p_proc_ready->time_slice_needed-1){
				print(p_proc_ready->p_name);
				print(" reading.");
				print(p_proc_ready->p_name);
				print(" end.    ");
			}else{
				print(p_proc_ready->p_name);
				print(" reading.");
				milli_delay(10);//没有用sleep，delay只是为了输出尽可能与时钟周期同步
			}
		}

		p_proc_ready->finished=1;
		reading_count--;

		//disp_str(" 0=0 ");//debug
		 
		V(&readerNum_lock);


		//判断是否需要释放文件资源， 归还写锁
		P(&readerCount_signal);
		reader_count--;

		//输出readerCount
		// char num = '0' + reader_count - 0;
		// char tem[4] = {' ', num, ' ', '\0'};
		// print(tem);
		// print(" num ");

		if(reader_count==0){
			//print(" 00=00 ");
			V(&write_lock);
		}

		//print("*");//debug

		V(&readerCount_signal);
		//end

		

		milli_delay(10);//最后一次循环后需要把后续的代码全都执行掉，所以delay要放到最后 
	}
}

void Writer_kind1(){
	while(1){
		P(&write_lock);

		if (p_proc_ready->ticks == 0)
			p_proc_ready->ticks = p_proc_ready->time_slice_needed;
		print(p_proc_ready->p_name);
		print(" start.  ");
		for(int timeSlice=0;timeSlice<p_proc_ready->time_slice_needed;timeSlice++){
			if(timeSlice==p_proc_ready->time_slice_needed-1){
				print(p_proc_ready->p_name);
				print(" writing.");
				print(p_proc_ready->p_name);
				print(" end.    ");
			}else{
				print(p_proc_ready->p_name);
				print(" writing.");
				milli_delay(10);
			}
		}

		p_proc_ready->finished=1;
		
		V(&write_lock);
		milli_delay(10);
	}
}





/* writeFirst */
void Reader_kind2(){
	while(1){
		P(&RW_lock); //写优先的时候，要设置一个信号量看是不是有进程在写，如果写还没有好，这里获取的时候就是0不能读

		P(&readerCount_signal);
		if (!reader_count)
			P(&write_lock);
		reader_count++;
		if (!p_proc_ready->ticks)
			p_proc_ready->ticks = p_proc_ready->time_slice_needed;
		V(&readerCount_signal);

		V(&RW_lock);


		P(&readerNum_lock);

		/*todo增加4-6的内容*/
		reading_count++;
		print(p_proc_ready->p_name);
		print(" start.  ");
		for(int timeSlice=0; timeSlice<p_proc_ready->time_slice_needed; timeSlice++){
			if(timeSlice==p_proc_ready->time_slice_needed-1){
				print(p_proc_ready->p_name);
				print(" reading.");
				print(p_proc_ready->p_name);
				print(" end.    ");
			}else{
				print(p_proc_ready->p_name);
				print(" reading.");
				milli_delay(10);//没有用sleep，delay只是为了输出尽可能与时钟周期同步
			}
		}

		p_proc_ready->finished=1;
		reading_count--;

		V(&readerNum_lock);


		P(&readerCount_signal);
		reader_count--;
		if (!reader_count)
			V(&write_lock);
		V(&readerCount_signal);
		milli_delay(10);
	}
}

void Writer_kind2(){
	while(1){
		P(&writerCount_signal);

		if (!writer_count)
			P(&RW_lock);
		writer_count++;

		V(&writerCount_signal);


		P(&write_lock);
		if (!p_proc_ready->ticks)
			p_proc_ready->ticks = p_proc_ready->time_slice_needed;
		print(p_proc_ready->p_name);
		print(" start.  ");
		for(int timeSlice=0;timeSlice<p_proc_ready->time_slice_needed;timeSlice++){
			if(timeSlice==p_proc_ready->time_slice_needed-1){
				print(p_proc_ready->p_name);
				print(" writing.");
				print(p_proc_ready->p_name);
				print(" end.    ");
			}else{
				print(p_proc_ready->p_name);
				print(" writing.");
				milli_delay(10);
			}
		}
		p_proc_ready->finished=1;
		V(&write_lock);


		P(&writerCount_signal);
		//disp_str(" ... ");
		writer_count--;
		if (!writer_count){
			//disp_str("-");//debug
			V(&RW_lock);
			//disp_str("-"); //debug
		}
		V(&writerCount_signal);
		milli_delay(10);//必须要有这个，不然立即执行下一轮循环，中断还没有出发，也没有运行调度函数
	}
}

/* 解决饿死问题 */
/* 主要是Fairlock大家都有获得的机会 */
void Reader_kind3(){
	while(1){
		//这一组pv要不要调整到中间
		P(&readerNum_lock);


		P(&FAIR_lock);
		P(&readerCount_signal);
		if (reader_count==0)
			P(&write_lock);
		reader_count++;
		if (!p_proc_ready->ticks)
			p_proc_ready->ticks = p_proc_ready->time_slice_needed;
		V(&readerCount_signal);
		V(&FAIR_lock);


		reading_count++;
		print(p_proc_ready->p_name);
		print(" start.  ");
		for(int timeSlice=0; timeSlice<p_proc_ready->time_slice_needed; timeSlice++){
			if(timeSlice==p_proc_ready->time_slice_needed-1){
				print(p_proc_ready->p_name);
				print(" reading.");
				print(p_proc_ready->p_name);
				print(" end.    ");
			}else{
				print(p_proc_ready->p_name);
				print(" reading.");
				milli_delay(10);//没有用sleep，delay只是为了输出尽可能与时钟周期同步
			}
		}
		p_proc_ready->finished=1;
		reading_count--;


		P(&readerCount_signal);
		reader_count--;
		if (reader_count==0)
			V(&write_lock);
		V(&readerCount_signal);


		V(&readerNum_lock);
		milli_delay(10);
	}
}

void Writer_kind3(){
	while(1){
		P(&FAIR_lock);
		P(&write_lock);

		if (!p_proc_ready->ticks)
			p_proc_ready->ticks = p_proc_ready->time_slice_needed;
		print(p_proc_ready->p_name);
		print(" start.  ");
		for(int timeSlice=0;timeSlice<p_proc_ready->time_slice_needed;timeSlice++){
			if(timeSlice==p_proc_ready->time_slice_needed-1){
				print(p_proc_ready->p_name);
				print(" writing.");
				print(p_proc_ready->p_name);
				print(" end.    ");
			}else{
				print(p_proc_ready->p_name);
				print(" writing.");
				milli_delay(10);
			}
		}
		p_proc_ready->finished=1;

		V(&write_lock);
		V(&FAIR_lock);
		milli_delay(10);
	}
}
