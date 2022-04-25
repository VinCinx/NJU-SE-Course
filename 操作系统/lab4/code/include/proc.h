
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* 相关寄存器的值 */
typedef struct s_stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;		/* ┓						│			*/
	u32	fs;		/* ┃						│			*/
	u32	es;		/* ┃						│			*/
	u32	ds;		/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()				│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;		/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;


/* 我们所说的进程表结构体 */
/* 恢复一个进程的时候，esp指向这个结构体的开始（栈pop的时候是由低到高），通过一系列的pop指令将寄存器的值（上次停止执行这个进程的时候保存的寄存器的值）弹出————回复进程的执行现场 */
typedef struct s_proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */

        int ticks;                 /* remained ticks */
        int priority;

	u32 pid;                   /* process id passed in from MM */
	char p_name[16];           /* name of the process */

	int time_slice_needed;
	int sleep_start_tick;//调用sleep函数不分配时间片的时刻
	int wakeup_tick;//被唤醒的时刻
	int state;//暂定用来记录进程的状态  00 01, 10 11   0 1, 2 3  *多余的
	int blocked;//是否被阻塞
	int finished;//是否已经完成
	char r_w_type;//记录该进程的读写类型

}PROCESS;

typedef struct s_task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
}TASK;

/* Number of tasks */
#define NR_TASKS	6



/* stacks of tasks */
#define STACK_SIZE_TESTA	0x8000
#define STACK_SIZE_TESTB	0x8000
#define STACK_SIZE_TESTC	0x8000
#define STACK_SIZE_TESTD 	0x8000
#define STACK_SIZE_TESTE 	0x8000
#define STACK_SIZE_TESTF 	0x8000

#define STACK_SIZE_TOTAL (STACK_SIZE_TESTA + \
						  STACK_SIZE_TESTB + \
						  STACK_SIZE_TESTC + \
						  STACK_SIZE_TESTD + \
						  STACK_SIZE_TESTE + \
						  STACK_SIZE_TESTF)
