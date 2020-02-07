
#include "FreeRTOS.h"
#include "task.h"


/************* 定义全局变量的标志 ************/
portCHAR flag1;
portCHAR flag2;


/********* 定义任务栈（一个全局的数组）******/
#define TASK1_STACK_SIZE                    20
StackType_t Task1Stack[TASK1_STACK_SIZE];           

#define TASK2_STACK_SIZE                    20
StackType_t Task2Stack[TASK2_STACK_SIZE];

/* 定义任务控制块 */
TCB_t Task1TCB;
TCB_t Task2TCB;

/* 定义任务句柄 */
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;



/************* 函数声明 *************/
void delay(uint32_t count);
void Task1_Entry(void *p_arg);
void Task2_Entry(void *p_arg);

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];


int main (void)
{
	
	prvInitialiseTaskLists();
	
	//任务1
	Task1_Handle = xTaskCreateStatic(Task1_Entry,
								     "Task1",
								     TASK1_STACK_SIZE,
								     NULL,
								     Task1Stack,
								     &Task1TCB);
	
	//核心函数是 vListInsert,将任务控制块的列表项插入就绪列表中
	vListInsert(&pxReadyTasksLists[1],&Task1TCB.xStateListItem);
	
	
	//任务2
	Task2_Handle = xTaskCreateStatic(Task2_Entry,
								     "Task2",
								     TASK2_STACK_SIZE,
								     NULL,
								     Task2Stack,
								     &Task2TCB);	
				
	vListInsert(&pxReadyTasksLists[2],&Task2TCB.xStateListItem);
	
	vTaskStartScheduler();
	
	for(;;)
	{
		/*什么都不做*/
	}
}

/* 延时函数 */
void delay(uint32_t count)
{
	for(;count!=0;count--);
}


/* 定义任务函数 */
void Task1_Entry(void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		taskYIELD();  //相当于调用了portmacro.h中的portYIELD()，并且触发了一个PendSV
	}
}


void Task2_Entry(void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		taskYIELD();
	}
}



