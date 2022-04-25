
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               tty.c
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
#include "keyboard.h"
#include "proto.h"
// #include <time.h>//用于记录时间，每20s清空屏幕一次

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);

PUBLIC void init_all_screen()
{
	TTY *p_tty;
	for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
	{
		init_screen(p_tty);
	}
	//因为init_screen()方法里会重新set_cursor() √
	select_console(0);
}

/*======================================================================*
                           task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
	TTY *p_tty;

	init_keyboard();

	for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}
	
	select_console(0);
	while (1) {
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}

/*======================================================================*
			   init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

	mode=0;//初始化为普通模式

	init_screen(p_tty);
}

/*======================================================================*
				in_process  将从key_board读入的数据放到对应的tty的缓冲区，调用put_key实现
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, u32 key)
{
        char output[2] = {'\0', '\0'};

		/*
		  锁定模式不处理除了esc以外的任何输入的字符
		  */
		if (mode == 2)
		{ 
			if ((key & MASK_RAW) == ESC)
			{
				mode = 0;
				/*
				  从锁定模式按esc退出
				  1.原文本中变为红色的文字恢复为白色 ，2.清除输入
				  */
				//todo一个恢复字体颜色的方法
				resetSearchedColor(p_tty->p_console);
				deleteEscInput(p_tty->p_console); //清除输入恢复指针和光标都在此方法实现
			}
			return;
		}
		/*
		  分支处理正确，已使用disp_str("A")测试
		  */
		if((key & FLAG_CTRL)!=0){
			char u8_key=key;//截取低8位
			if (u8_key == 'z' || u8_key == 'Z')
			{
				if(mode==0)
					ctrlzForMode0(p_tty->p_console);//普通模式下的回退
				else
					ctrlzForMode1(p_tty->p_console);//mode为1也就是查找模式下输入查找字符时
				return;
			}
		}
		/*
		  如果是可打印字符，放入缓冲区； 如果是不可打印的特殊字符，则在else里进行相应的特殊处理  但是回车和退格键也要往缓冲区写数据
		  */
        if (!(key & FLAG_EXT)) {
			//如果是可以输出的字符，调用put_key，在该函数中调用out_char，因为可输出的字符一定是一个char就可以表示的
			put_key(p_tty, key);
        }
        else {
            int raw_code = key & MASK_RAW;
			switch (raw_code)
			{
			case ENTER:
				if (mode == 0)
				{
					put_key(p_tty, '\n'); /*普通模式下，打印一个回车*/
				}
				else if (mode == 1) //else+括号不要把if漏掉
				{
					mode = 2; /*0b10，跳入锁定模式*/
					 /*
						todo：
						查找对应的字符并标红
						*/
					escSearch(p_tty->p_console);
				}
				break;
			case BACKSPACE:
				if(mode==0)
					put_key(p_tty, '\b');
				else
				{//这里mode只可能等于1
					if (p_tty->p_console->cursor>p_tty->p_console->escCursor)
						put_key(p_tty, '\b');
					else
						;//do nothing 在esc模式下不能删除普通模式下的字符  如果cursor等于esc进入时的cursor，不允许再删除
				}
				break;
			case UP:
				if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
				{
					scroll_screen(p_tty->p_console, SCR_DN);
				}
				break;
			case DOWN:
				if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
				{
					scroll_screen(p_tty->p_console, SCR_UP);
				}
				break;
			case TAB:
				put_key(p_tty, '\t');
				break;
			/*  
			  ESC查找模式:
			  */
			case ESC:
				if (mode == 0){
					mode = 1; /*设置为查找模式*/
					setEscpos(p_tty->p_console);/*调用consle里的函数记录escpos，为当前的console记录esc开始时候的指针————让escpos等于当前ch_pointer的值*/
					setEscCursorRecord(p_tty->p_console);/*记录cursor值*/
				}
				else if (mode == 1){
					mode = 0; /*不等于0——也就是不在普通模式的情况下，按下esc全部恢复为普通模式->将mode的设置为0*/
					deleteEscInput(p_tty->p_console); /*如果在mode1，则清除输入  这个函数里会清除输入并恢复各种指针的值*/
				}
			/* Alt + F1~F12 */
			case F1:
			case F2:
			case F3:
			case F4:
			case F5:
			case F6:
			case F7:
			case F8:
			case F9:
			case F10:
			case F11:
			case F12:
				if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R))
				{
					select_console(raw_code - F1);
				}
				break;
			default:
				break;
			}
		}
}

/*======================================================================*
			      put_key
*======================================================================*/
PRIVATE void put_key(TTY* p_tty, u32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {//如果刚刚填充的空闲位置 指向的是缓冲区的最后一个地方，则下一个空闲的位置为缓冲区的开头
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


/*======================================================================*
			      tty_do_read
 *======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}


/*======================================================================*
			      tty_do_write
 *======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		out_char(p_tty->p_console, ch);
		//显示之后，将这个操作进行记录
		pushCharRecord(p_tty->p_console, ch);
	}
}


