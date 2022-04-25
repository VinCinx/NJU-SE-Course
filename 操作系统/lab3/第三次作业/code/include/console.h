
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			      console.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef _ORANGES_CONSOLE_H_
#define _ORANGES_CONSOLE_H_

#define SCREEN_SIZE (80 * 25)
#define SCREEN_WIDTH 80
/* CONSOLE */
typedef struct s_console
{
	/*
	  下面的变量
	  都是以两个char，也就是一个word为单位
	  全部在init_screen里初始化！！！！！！
	  */
	unsigned int	current_start_addr;	/* 当前显示到了什么位置——也就是显示屏上首个字符的位置*/
	unsigned int original_addr;			/* 当前控制台对应显存位置 ，对于屏幕1，这个变量就是0*/
	unsigned int	v_mem_limit;		/* 当前控制台占的显存大小 ，不变*/
	unsigned int	cursor;			/* 当前光标位置——也就是显示屏上最后一个字符的下一个空的位置 */

	//这个程序原来是按照3个显示台写的，32kb/3，每个页面差不多10kb，也就是可以显示两个屏幕多一点的内容
	//这里简化为可以显示两个屏幕的内容处理
	//用一个数组记录  从original_addr开始的所有的操作  一个操作也就是接收一个char并做相应的处理
	char ch[SCREEN_SIZE*2];//实际上即便填满整个屏幕可能也不会有这么多ch记录，因为一个ch可能表示tab，占好几个位置
	int ch_pointer;
	int escPos;

	int escCursor;
}CONSOLE;

#define SCR_UP	1	/* scroll forward */
#define SCR_DN	-1	/* scroll backward */


#define DEFAULT_CHAR_COLOR	0x07	/* 0000 0111 黑底白字 */


#endif /* _ORANGES_CONSOLE_H_ */
