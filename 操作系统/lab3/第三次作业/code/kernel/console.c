
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			      console.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	回车键: 把光标移到第一列
	换行键: 把光标前进到下一行
*/


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
#define maxEnterSize 655*2//32*1024/(25*2)   *2，存放一个初始地址和一个换行后的地址
#define maxTabSize 8192//32*1024/4=8192

/*

*/
u32 ENTER_STACK[maxEnterSize];
u32 TAB_STACK[maxTabSize];
int pointer_ENTER=-1;
int pointer_TAB=-1;
PRIVATE int isEnter(u32 position){  return position==ENTER_STACK[pointer_ENTER];}
PRIVATE void pushEnter(u32 position){  ENTER_STACK[++pointer_ENTER]=position;}
PRIVATE u32 popEnter(){  return ENTER_STACK[pointer_ENTER--];}
PRIVATE void pushTab(u32 position){  TAB_STACK[++pointer_TAB] = position;}
PRIVATE void popTab(){  TAB_STACK[pointer_TAB--]=-1;}//不要写成ENTER_STACK。。。
PRIVATE int isTab(u32 position){  return TAB_STACK[pointer_TAB]==position;}

PRIVATE void set_cursor(unsigned int position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE* p_con);

/*将操作记录压入，在ttydowrite里调用*/
PUBLIC void pushCharRecord(CONSOLE *p_con, char ch)
{
	p_con->ch[p_con->ch_pointer++]=ch;
}

/*记录进入esc模式时的cursor*/
PUBLIC void setEscCursorRecord(CONSOLE *p_con)
{
	p_con->escCursor=p_con->cursor;
}
PUBLIC void resetEscCursorRecord(CONSOLE *p_con){
	p_con->escCursor=-1;
}
/*设置进入esc的位置，在从mode0到mode1时调用*/
PUBLIC void setEscpos(CONSOLE *p_con){  p_con->escPos = p_con->ch_pointer;}
/*恢复，恢复到mode0时调用*/
PUBLIC void resetEscpos(CONSOLE *p_con) { p_con->escPos = -1; }
PUBLIC void resetCursor(CONSOLE *p_con){
	p_con->cursor=p_con->escPos;
}//未使用

PUBLIC int escInputNum(CONSOLE *p_con) //这个方法只有在mode1模式下，ctrl+z的时候才会调用，所以调用这个函数的时候escPos不会等于-1
{
	if(p_con->escPos==p_con->ch_pointer)//相等的时候表示esc模式下还没有输入
		return 0;
	else
		return p_con->ch_pointer-p_con->escPos;
}

/*======================================================================*
			   ctrlzForMode0&Mode1
 *======================================================================*/
PUBLIC void ctrlzForMode0(CONSOLE *p_con)
{
	//清屏
	disp_pos=p_con->original_addr*2;
	for(int i=0;i<SCREEN_SIZE*2;i++){//disp_pos显存指针，这里其实screensize不用*2就行，  不乘2就是清除一个屏幕内的内容
		disp_str(" ");
	}
	//恢复指针到当前console的显存开始位置	
	disp_pos = p_con->original_addr * 2;
	//cursor也要放到显存的开始位置！！！！！！！
	//因为调用out_char()的时候，实际地址  由基地址和cursor*2计算得到，要从头开始打印，就要恢复cursor
	p_con->cursor=p_con->original_addr;
	flush(p_con);
	if (p_con->ch_pointer != 0 )
	{
		/* 如果涉及到滚屏，这里可能需要判断需不需要向上滚，假定一个屏幕不会显示满屏
		   这里不添加滚屏，假定一个屏幕不会显示满*/
		for(int i=0;i<p_con->ch_pointer-1;i++)
			out_char(p_con, p_con->ch[i]); //显示p_con->ch[i]！！！！！！不是p_con->ch[ch_pointer]
		p_con->ch_pointer--;
	}
	else{
		//do nothing，因为操作数组里没有记录任何历史操作
	}
}

/*错误的清屏方式：
   for(int i=p_con->escPos;i<SCREEN_SIZE*2;i++){
		disp_str(" ");
	}
	
	因为escPos和cursor在输入里有tab或者回车键的时候是不相等的*/
PUBLIC void ctrlzForMode1(CONSOLE *p_con)
{
	/*整体的操作：清除esc模式下的n步操作，然后将前n-1步操作再做一次*/
	//清除esc模式下的输入
	disp_pos = p_con->original_addr * 2 + p_con->escCursor * 2; //这里original_addr是0，escPos以word为单位   disp_pos以byte为单位，而且是显存指针，不需要加上base基地址
	for(int i=p_con->escCursor;i<SCREEN_SIZE*2;i++){
		disp_str(" ");
	}
	disp_pos = p_con->original_addr * 2 + p_con->escCursor * 2;
	//将光标移动到esc开始的第一个字符处
	p_con->cursor=p_con->escCursor;
	flush(p_con);
	//将前n-1个操作再做一次		
	int num=escInputNum(p_con);
	if(num!=0){
		for(int i=p_con->escPos;i<p_con->ch_pointer-1;i++)//只有一个输入时，ch_pointer-1==escPos，不会进入这个循环
			out_char(p_con, p_con->ch[i]);
		p_con->ch_pointer--;//！！！！！！！！！！！！1不能漏！！！！！ 回退一个操作就要从操作数组里删除一个操作，就是将指针向前移动一位
	}else{
		//do nothing  在esc模式下还没有输入，不处理
	}
}
/*======================================================================*
			   deleteEscInput
 *======================================================================*/
PUBLIC void deleteEscInput(CONSOLE *p_con)
{
	disp_pos = p_con->original_addr * 2 + p_con->escCursor * 2; //这里original_addr是0，escPos以word为单位   disp_pos以byte为单位，而且是显存指针，不需要加上base基地址
	for (int i = p_con->escCursor; i < SCREEN_SIZE * 2; i++)
		disp_str(" ");
	disp_pos = p_con->original_addr * 2 + p_con->escCursor * 2;
	//恢复光标的位置
	p_con->cursor = p_con->escCursor;
	flush(p_con);
	resetEscCursorRecord(p_con);
	resetEscpos(p_con);
}
/*======================================================================*
			   escSearch
 *======================================================================*/
PUBLIC void escSearch(CONSOLE *p_con)
{	//计算esc后面，在屏幕上显示出来的有几个字符  abaaba
	//screen_inputNum错误的计算方法：  int screen_inputNum=escInputNum(p_con); a[backspace] 结果应该是0，但是escInputNum(p_con)是记录操作的，会记录两个操作
	int screen_inputNum=p_con->cursor-p_con->escCursor;
	if(screen_inputNum==0)
		return;
	int beginPos=p_con->original_addr*2;
	while(beginPos<p_con->original_addr*2+p_con->escCursor*2){
		int found=1;
		int pos=beginPos;//从i开始的移动指针

		if (p_con->escCursor - (beginPos / 2 - p_con->original_addr) < screen_inputNum){
			found = 0;
		}else{
			for (int j = p_con->original_addr * 2 + p_con->escCursor * 2; j < p_con->original_addr * 2 + p_con->cursor * 2; j += 2){
				if (*((u8 *)(V_MEM_BASE + pos)) == *((u8 *)(V_MEM_BASE + j))){
					//特殊处理tab
					if ( *((u8 *)(V_MEM_BASE + pos)) == ' ')
					{
						//不能用简单的颜色相等进行匹配，因为代码里空格在不同模式下的前景颜色是不一样的，也就是色彩值不相等
						if (*((u8 *)(V_MEM_BASE + pos + 1)) == GREEN)//被查找文档是tab的情况，要查找的也要是tab才能算找到了
						{
							if (*((u8 *)(V_MEM_BASE + j + 1)) == GREEN)
								pos += 2;
							else{
								found=0;
								break;
							}
						}
						else{//被查找的文档是普通的空格的情况，后面的文档也要是普通的空格才能匹配
							if (*((u8 *)(V_MEM_BASE + j + 1)) != GREEN)
								pos += 2;
							else{
								found = 0;
								break;
							}
						}
					}else{
						pos+=2;
					}
				}
				else{
					found = 0;
					break;
				}		
			}
		}
		/*
		  找到了就要标红
		  而且下一次开始找的位置不是直接将cursor向后移动一个位置，而要移动到这个找到的内容的后面一格
		  也就是不能从被找到内容的内部开始找，被找到的内容已经不可用了*/
		if(found==1){
			for(int j=beginPos;j<pos;j+=2){
				if(*(u8*)(V_MEM_BASE + j ) != ' ')
					*(u8*)(V_MEM_BASE + j + 1) = RED;
				else{//是空格，判断是普通空格还是tab
					if (*(u8 *)(V_MEM_BASE + j + 1)==GREEN)//是tab,用绿色显示
						*(u8 *)(V_MEM_BASE + j + 1) = BACKGROUNDRED1; //从BACKGROUNDRED复制的时候不要只改名字不改后面的颜色参数
					else//是普通空格，用红色显示
						*(u8 *)(V_MEM_BASE + j + 1) = BACKGROUNDRED;
				}
			}
			beginPos+=screen_inputNum*2;
		}else
			beginPos+=2;//没找到就将下一个位置作为起始位置开始找
	}
}
/*======================================================================*
			   resetSearchedColor恢复被查找到的字体的颜色，注意，tab的颜色不能恢复到普通的颜色
 *======================================================================*/
PUBLIC void resetSearchedColor(CONSOLE *p_con)
{
	for(int i=0;i<p_con->escCursor;i++){
		if (*(u8 *)(V_MEM_BASE + i * 2 + 1) == BACKGROUNDRED1 || *(u8 *)(V_MEM_BASE + i * 2 + 1)==GREEN)//tab被查找了恢复为绿色，没有被查找，依然赋值为绿色
			*(u8*)(V_MEM_BASE + i*2 + 1)=GREEN;
		else
			*(u8*)(V_MEM_BASE + i*2 + 1) = DEFAULT_CHAR_COLOR;
	}
}

/*======================================================================*
			   init_screen
 *======================================================================*/
PUBLIC void init_screen(TTY* p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;	/* 显存总大小 (in WORD)  因为两个byte，也就是一个word用于输出一个char*/

	int con_v_mem_size                   = v_mem_size / NR_CONSOLES;
	p_tty->p_console->original_addr      = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit        = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	p_tty->p_console->escPos=-1;//设为-1，默认不在esc模式
	p_tty->p_console->ch_pointer=0;
	p_tty->p_console->escCursor = -1; //设为-1，默认不在esc模式 与escPos在有tab的时候不相等，要单独记录

	/* 默认光标位置在最开始处 
	   添加的：
	   对于console0来说cursor就是0
	   但是对于别的console，cursor就是一个以word为单位的不为0的绝对地址了
	   （这个指的是在显存中的绝对地址，在物理内存中还是相对地址，再加上一个定义好的常量base才是物理地址）
	   */
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	if (nr_tty == 0) {
		/* 第一个控制台沿用原来的光标位置 */
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	else {
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}

	mode=0;//
	set_cursor(p_tty->p_console->cursor);
}


/*======================================================================*
			   is_current_console
*======================================================================*/
PUBLIC int is_current_console(CONSOLE* p_con)
{
	return (p_con == &console_table[nr_current_console]);
}


/*======================================================================*
			   out_char
 *======================================================================*/
PUBLIC void out_char(CONSOLE* p_con, char ch)
{
	//p_vmem是物理地址
	u8* p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);

	switch(ch) {
	case '\n':
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - SCREEN_WIDTH) {//先判断能不能换行，能换行的前提是换行后，新行的结尾不能超过分配的内存块
			pushEnter(p_con->cursor);//存放回车之前的地址
			p_con->cursor = p_con->original_addr + SCREEN_WIDTH * ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1);
			pushEnter(p_con->cursor);//用于退格时判断，要删除的是不是回车
		}
		break;
	case '\b':
		if(isTab(p_con->cursor)){
			popTab();
			p_con->cursor-=4;
			
			int count=0;
			while(count++<4){
				*(p_vmem - 2*count) = ' ';
				*(p_vmem - 2*count+1) = DEFAULT_CHAR_COLOR;
			}
		}else if(isEnter(p_con->cursor)){
			popEnter();
			u32 recoverPos=popEnter();
			while(p_con->cursor!=recoverPos){
				p_vmem-=2;
				*(p_vmem) = ' ';
				*(p_vmem+1) = DEFAULT_CHAR_COLOR;
				p_con->cursor--;//这个递减不能放在while括号里，如果放在里面，两者相等时还会减去1
			}
		}else{
			if (p_con->cursor > p_con->original_addr) {
				p_con->cursor--;
				//两个char的数据   打印出一个字符
				*(p_vmem-2) = ' ';
				*(p_vmem-1) = DEFAULT_CHAR_COLOR;
			}
		}
		break;
	case '\t':
		if (p_con->cursor <= p_con->original_addr + p_con->v_mem_limit - 4){//即当前位置<=limit指向的位置-4
			// int count = 0;
			// while (count++ < 4)
			// {
			// 	out_char(p_con, ' ');
			// }
			for (int i = 0; i < 4; ++i)
			{ 
				*p_vmem++ = ' ';
				*p_vmem++ = GREEN; // 为了在查找时与4个空格区分   这样设置后无论什么模式tab的前景颜色都是绿色，不过没关系因为tab不需要显示任何字符，只有设置背景色时才会显示出来
			}
			p_con->cursor+=4;
			pushTab(p_con->cursor);
			break;
		}
		break;
	default:
		if (p_con->cursor <
		    p_con->original_addr + p_con->v_mem_limit - 1) {
			*p_vmem++ = ch;
			if(mode==0){
				*p_vmem++ = DEFAULT_CHAR_COLOR;
			}else if(mode==1){
				*p_vmem++ = RED;
			}
			p_con->cursor++;
		}
		break;
	}

	while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE) {
		scroll_screen(p_con, SCR_DN);
	}

	flush(p_con);
}

/*======================================================================*
                           flush
*======================================================================*/
PRIVATE void flush(CONSOLE *p_con) /*这个函数负责将光标显示出来，并且从current_start_addr的地方开始显示输入的内容*/
{
        set_cursor(p_con->cursor);
        set_video_start_addr(p_con->current_start_addr);
}

/*======================================================================*
			    set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

/*======================================================================*
			  set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}



/*======================================================================*
			   select_console
 *======================================================================*/
PUBLIC void select_console(int nr_console)	/* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES)) {
		return;
	}

	nr_current_console = nr_console;

	set_cursor(console_table[nr_console].cursor);
	set_video_start_addr(console_table[nr_console].current_start_addr);
}

/*======================================================================*
			   scroll_screen
 *----------------------------------------------------------------------*
 滚屏.
 *----------------------------------------------------------------------*
 direction:
	SCR_UP	: 向上滚屏
	SCR_DN	: 向下滚屏
	其它	: 不做处理
 *======================================================================*/
PUBLIC void scroll_screen(CONSOLE* p_con, int direction)
{
	if (direction == SCR_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			//按照代码的逻辑可知，只要这个if判断成立，current_start_addr肯定是够减去一个SCREEN_WIDTH的
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN) {
		if (p_con->current_start_addr + SCREEN_SIZE <
		    p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else{
	}

	set_video_start_addr(p_con->current_start_addr);
	set_cursor(p_con->cursor);
}

