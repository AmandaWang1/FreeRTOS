/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle;
/* LED������ */
static TaskHandle_t LED_Task_Handle;		



/* AppTaskCreate���������ջ */
static StackType_t AppTaskCreate_Stack[128];
/* LED�����ջ */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate ������ƿ� */
static StaticTask_t AppTaskCreate_TCB;
/* LED ������ƿ� */
static StaticTask_t LED_Task_TCB;

/* �������������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;	
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;



//��������
static void AppTaskCreate(void);
static void LED_Task(void* parameter);
static void BSP_Init(void);
/****************************************/

int main(void)
{	
	BSP_Init();
	
	 /* ���� AppTaskCreate ���� */
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t	)AppTaskCreate,		//������
															(const char* 	)"AppTaskCreate",		//��������
															(uint32_t 		)128,	//�����ջ��С
															(void* 		  	)NULL,				//���ݸ��������Ĳ���
															(UBaseType_t 	)3, 	//�������ȼ�
															(StackType_t*   )AppTaskCreate_Stack,	//�����ջ
															(StaticTask_t*  )&AppTaskCreate_TCB);	//������ƿ�   
															
	if(NULL != AppTaskCreate_Handle)/* �����ɹ� */
    vTaskStartScheduler();   /* �������񣬿������� */
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL();           //�����ٽ���

  /* ����LED_Task���� */
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t	)LED_Task,		//������
															(const char* 	)"LED_Task",		//��������
															(uint32_t 		)128,					//�����ջ��С
															(void* 		  	)NULL,				//���ݸ��������Ĳ���
															(UBaseType_t 	)4, 				//�������ȼ�
															(StackType_t*   )LED_Task_Stack,	//�����ջ
															(StaticTask_t*  )&LED_Task_TCB);	//������ƿ�   
	
															
	if(NULL != LED_Task_Handle)/* �����ɹ� */
		printf("LED_Task���񴴽��ɹ�!\n");
	else
		printf("LED_Task���񴴽�ʧ��!\n");
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED����
static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1(ON);
        vTaskDelay(500);   /* ��ʱ500��tick */
        printf("LED_Task Running,LED1_ON\r\n");
        
        LED1(OFF);     
        vTaskDelay(500);   /* ��ʱ500��tick */		 		
        printf("LED_Task Running,LED1_OFF\r\n");
    }
}


/*���а����ϵĳ�ʼ�����ɷ��������������*/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	USART_Config();
	
//	LED1(ON);
//	printf("usart init is ok!\n");
//	while(1);
//  
}

	/*  ��ȡ��������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С  
			�����û��Լ����䣬���ܶ�̬����*/ 
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* ������ƿ��ڴ� */
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* �����ջ�ڴ� */
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* �����ջ��С */
}

/**
  *********************************************************************
  * @brief  ��ȡ��ʱ������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* ������ƿ��ڴ� */
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* �����ջ�ڴ� */
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
}



