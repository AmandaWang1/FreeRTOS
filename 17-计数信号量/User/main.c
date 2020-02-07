/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"//��Ϣ����ͷ�ļ�
#include "semphr.h"//�ź���ͷ�ļ�

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


/*
	ʵ�鲽�裺
	1.�����ź������
	2.����һ�������ź��� xSemaphoreCreateCounting()
	3.���������¾ͻ�ȡһ���ź��� xSemaphoreTake()
	4.���������¾��ͷ�һ���ź��� xSemaphoreGive()
*/


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� 
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED������ */
static TaskHandle_t Receive_Task_Handle = NULL;
/* LED2������ */
static TaskHandle_t Send_Task_Handle = NULL;
/* �����ź������ */
SemaphoreHandle_t CountSem_Handle =NULL;

#define  QUEUE_LEN    4   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  QUEUE_SIZE   4   /* ������ÿ����Ϣ��С���ֽڣ� */


/* �������� */
static void AppTaskCreate(void); /* ���ڴ������� */
static void Receive_Task(void* pvParameters); /* LED_Task����ʵ�� */
static void Send_Task(void* pvParameters); /* LED2_Task����ʵ�� */
static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */


int main(void)
{	
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	BSP_Init();
	
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
												
	/* �������ֵ��ȷ������������� */           
  if(pdPASS == xReturn) 
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
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
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
	
	//���ͣ��������ʼ��5����λ���ܹ�5����λ//�����ź������
	CountSem_Handle = xSemaphoreCreateCounting(5,5);//�������������ź���ֵ����ʼ�����ź���ֵ//�ǵð�config��ʹ�ü����ź����ĺ��
	
  if(NULL != CountSem_Handle)
    printf("����BinarySem�ɹ�!\r\n");
	
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Receive_Task, /* ������ں��� */
                        (const char*    )"Receive_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Receive_Task_Handle);/* ������ƿ�ָ�� */
												
	if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
												
	/* ����LED2_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Send_Task, /* ������ں��� */
                        (const char*    )"Send_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Send_Task_Handle);/* ������ƿ�ָ�� */
												
												
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}


static void Receive_Task(void* parameter)
{		
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    //���KEY1������
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       
		{
			/* ��ȡһ�������ź��� */
      xReturn = xSemaphoreTake(CountSem_Handle,	/* �����ź������ */
                             0); 	/* �ȴ�ʱ�䣺0 */
			if ( pdTRUE == xReturn ) 
				printf( "KEY1�����£��ɹ����뵽ͣ��λ��\n" );
			else
				printf( "KEY1�����£�������˼������ͣ����������\n" );							
		}
		vTaskDelay(20);     //ÿ20msɨ��һ��		
  }
}


static void Send_Task(void* parameter)
{	
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    //���KEY2������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       
		{
			/* ��ȡһ�������ź��� */
      xReturn = xSemaphoreGive(CountSem_Handle);//���������ź���                  
			if ( pdTRUE == xReturn ) 
				printf( "KEY2�����£��ͷ�1��ͣ��λ��\n" );
			else
				printf( "KEY2�����£������޳�λ�����ͷţ�\n" );							
		}
		vTaskDelay(20);     //ÿ20msɨ��һ��	
  } 
}

static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* LED��ʼ�� */
	LED_GPIO_Config();
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	/* ������ʼ�� */
	Key_GPIO_Config();
	
}
