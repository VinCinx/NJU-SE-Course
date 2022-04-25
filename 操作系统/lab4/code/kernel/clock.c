
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
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


//下面这段可以放入到代码里测试print
// disp_str("?");
// char *aa = "cacal";
// print(aa);
// char *ab = "11111111111";
// print(ab);
// disp_str("?");
/*======================================================================*
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	/* 可以再设置一个全局变量记录进程newDelay时的tick，在clock里用新的tick判断时间延迟 
	类似于维护一个等待队列，谁在等待，等待多久，开始等待的时间
	每次中断判断一下是不是可以释放等待队列里的进程了
	因为进程一旦不被分配时间片后不会被执行，也就不会执行newDelay ，不可能自己释放自己
	
	
	*/
	//disp_str("#");
	ticks++;
	//disp_int(ticks);
	//disp_str(" ");


	// disp_str(" ");
	// disp_str(p_proc_ready->p_name);
	// disp_int(p_proc_ready->ticks);
	// disp_str(" ");


	p_proc_ready->ticks--;
	

	// if (k_reenter != 0) {
	// 	//disp_str("! ");
	// 	// disp_int(ticks);
	// 	// disp_str(" ! ");
	// 	//return;
	// }//关于中断重入的问题

	//非抢占式的
	// if (p_proc_ready->ticks > 0) {
	// 	return;
	// }

	schedule();

}

/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
    int t = get_ticks();
    while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

/*======================================================================*
                              new_milli_delay
 *======================================================================*/
PUBLIC void new_milli_delay(int milli_sec)
{
	/* 这里将state恢复为1是错误的 */
	p_proc_ready->state = 0;//在这里设置的话 调度的时候 永远都不会调度到A，也就不会继续执行while函数进行时间的延迟，不会将state状态恢复为1

	int t = get_ticks();
	while (((get_ticks() - t) * 1000 / HZ) < milli_sec)
	{
	}

	p_proc_ready->state = 1;
}
