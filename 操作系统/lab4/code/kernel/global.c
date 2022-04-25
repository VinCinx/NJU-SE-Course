
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"

#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PUBLIC	PROCESS			proc_table[NR_TASKS];//进程数组
PUBLIC PROCESS *pre_proc = proc_table;

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

//下面的会出错
// PUBLIC TASK task_table[NR_TASKS] = {
//     {Reader_kind1, STACK_SIZE_TESTA, "A"},
//     {Reader_kind1, STACK_SIZE_TESTB, "B"},
//     {Reader_kind1, STACK_SIZE_TESTC, "C"},
//     {Writer_kind1, STACK_SIZE_TESTD, "D"},
//     {Writer_kind1, STACK_SIZE_TESTE, "E"},
//     {F, STACK_SIZE_TESTF, "F"}};


//NR_TASKS改为3不会出错
// PUBLIC TASK task_table[NR_TASKS] = {{TestA, STACK_SIZE_TESTA, "TestA"},
//                                     {TestB, STACK_SIZE_TESTB, "TestB"},
//                                     {Reader_kind1, STACK_SIZE_TESTC, "TestC"}};


/* 写者优先 */
PUBLIC TASK task_table[NR_TASKS] = {{Reader_kind1, STACK_SIZE_TESTA, "A"},
                                    {Reader_kind1, STACK_SIZE_TESTB, "B"},
                                    {Reader_kind1, STACK_SIZE_TESTC, "C"},
                                    {Writer_kind1, STACK_SIZE_TESTD, "D"},
                                    {Writer_kind1, STACK_SIZE_TESTE, "E"},
                                    {F, STACK_SIZE_TESTF, "F"}};

/* 读者优先，只需要改变读和写的方法都为kind2的，并且将写进程放到前面，先执行写就行了 */
/* main里的ticks赋值要改一下 */
/* main里的r_w属性要改一下 */
// PUBLIC TASK task_table[NR_TASKS] = {{Writer_kind2, STACK_SIZE_TESTD, "D"},
//                                     {Writer_kind2, STACK_SIZE_TESTE, "E"},
//                                     {Reader_kind2, STACK_SIZE_TESTA, "A"},
//                                     {Reader_kind2, STACK_SIZE_TESTB, "B"},
//                                     {Reader_kind2, STACK_SIZE_TESTC, "C"},
//                                     {F, STACK_SIZE_TESTF, "F"}};

/* 读写公平，解决部分的饿死问题 */
// PUBLIC TASK task_table[NR_TASKS] = {{Reader_kind3, STACK_SIZE_TESTA, "A"},
//                                     {Reader_kind3, STACK_SIZE_TESTB, "B"},
//                                     {Reader_kind3, STACK_SIZE_TESTC, "C"},
//                                     {Writer_kind3, STACK_SIZE_TESTD, "D"},
//                                     {Writer_kind3, STACK_SIZE_TESTE, "E"},
//                                     {F, STACK_SIZE_TESTF, "F"}};

PUBLIC irq_handler irq_table[NR_IRQ];

PUBLIC system_call sys_call_table[NR_SYS_CALL] = {sys_get_ticks,
                                                  sys_sleep,
                                                  sys_print,
                                                  sys_P,
                                                  sys_V};


//用于调试的全局变量
PUBLIC int global0=0;
//一些用于输出的                                                  
PUBLIC char READ_START[] = " read start! ";
PUBLIC char WRITE_START[] = " write start! ";
PUBLIC char READING[] = " reading...   ";
PUBLIC char WRITING[] = " writing...   " ;
PUBLIC char READ_FINISH[] = " READ FINISHED ";
PUBLIC char WRITE_FINISH[] = " WRITE FINISHED ";
PUBLIC char New_Line[] = "\n";
PUBLIC int reader_count;
PUBLIC int writer_count;
PUBLIC char RW_STATUS;

PUBLIC int reading_count;

PUBLIC Semaphore readerNum_lock;//可以同时读的文件数的信号量
PUBLIC Semaphore write_lock;//写锁，共享资源信号量
PUBLIC Semaphore readerCount_signal;
PUBLIC Semaphore writerCount_signal;
PUBLIC Semaphore readingCount_signal;//在main里使用这个变量暂时会出问题
PUBLIC Semaphore RW_lock;//读写争抢资源

PUBLIC Semaphore FAIR_lock;



PUBLIC int ready_queue[NR_TASKS];
PUBLIC int readyQueue_pointer;
PUBLIC void readyQueue_push(int x){ready_queue[readyQueue_pointer++] = x;}
PUBLIC int readyQueue_pop(){
    if (readyQueue_pointer > 0){
        int taskNum=ready_queue[0];
        for(int i=0;i<readyQueue_pointer-1;i++){
            ready_queue[i]=ready_queue[i+1];
        }
        readyQueue_pointer--;
        return taskNum;
    }
    return -1;
}
/*todo在global.h里添加，main里初始化*/
PUBLIC void readyQueue_remove(int taskNum){
    int temp=-1;//存放taskNum的下标
    for(int i=0;i<readyQueue_pointer;i++){
        if(taskNum==ready_queue[i]){
            temp=i;
            break;
        }
    }
    for(int i=temp;i<readyQueue_pointer-1;i++){
        ready_queue[i] = ready_queue[i + 1];
    }
    readyQueue_pointer--;
}
