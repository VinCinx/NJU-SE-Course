
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;
EXTERN	u8		gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	u32		k_reenter;

EXTERN	TSS		tss;
EXTERN	PROCESS*	p_proc_ready;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern  TASK            task_table[];
extern	irq_handler	irq_table[];

// 定义一个信号量的结构体，维护一个信号量val，以及一个等待队列
typedef struct semaphore
{
    int value;
    PROCESS *queue[NR_TASKS];
}Semaphore;

extern char READ_START[];
extern char WRITE_START[];
extern char READING[];
extern char WRITING[];
extern char READ_FINISH[];
extern char WRITE_FINISH[];
extern char New_Line[];
extern int reader_count;
extern int writer_count;
extern char RW_STATUS;
extern int reading_count;

extern Semaphore readerNum_lock;
extern Semaphore write_lock;
extern Semaphore readerCount_signal;
extern Semaphore writerCount_signal;
extern Semaphore externl;
extern Semaphore RW_lock;

extern Semaphore FAIR_lock;

extern int ready_queue[NR_TASKS];
extern int readyQueue_pointer;
extern void readyQueue_push(int x);
extern int readyQueue_pop();
extern void readyQueue_remove(int taskNum);

PUBLIC PROCESS *pre_proc;

extern int global0;
