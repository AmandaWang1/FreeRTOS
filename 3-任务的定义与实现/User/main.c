
#include "FreeRTOS.h"
#include "task.h"


/************* ����ȫ�ֱ����ı�־ ************/
portCHAR flag1;
portCHAR flag2;


/********* ��������ջ��һ��ȫ�ֵ����飩******/
#define TASK1_STACK_SIZE                    20
StackType_t Task1Stack[TASK1_STACK_SIZE];           

#define TASK2_STACK_SIZE                    20
StackType_t Task2Stack[TASK2_STACK_SIZE];

/* ����������ƿ� */
TCB_t Task1TCB;
TCB_t Task2TCB;

/* ���������� */
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;



/************* �������� *************/
void delay(uint32_t count);
void Task1_Entry(void *p_arg);
void Task2_Entry(void *p_arg);

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];


int main (void)
{
	
	prvInitialiseTaskLists();
	
	//����1
	Task1_Handle = xTaskCreateStatic(Task1_Entry,
								     "Task1",
								     TASK1_STACK_SIZE,
								     NULL,
								     Task1Stack,
								     &Task1TCB);
	
	//���ĺ����� vListInsert,��������ƿ���б����������б���
	vListInsert(&pxReadyTasksLists[1],&Task1TCB.xStateListItem);
	
	
	//����2
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
		/*ʲô������*/
	}
}

/* ��ʱ���� */
void delay(uint32_t count)
{
	for(;count!=0;count--);
}


/* ���������� */
void Task1_Entry(void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		taskYIELD();  //�൱�ڵ�����portmacro.h�е�portYIELD()�����Ҵ�����һ��PendSV
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



