/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"//��Ϣ����ͷ�ļ�
#include "semphr.h"//�ź���ͷ�ļ�
#include "limits.h"

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  

/* ����1���¸�LED1������Ϣ������2���¸�LED2������Ϣ */

/*
	ʵ�鲽�裺
	1.��ȷʹ���ĸ�����
	2.������Ϣ��xTaskNotify()
	3.������Ϣ��xTaskNotifyWait()
	4.��������
	5.ʵ�����������߼�
*/


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED������ */
static TaskHandle_t Receive_Task_Handle = NULL;
/* LED1������ */
static TaskHandle_t Receive1_Task_Handle = NULL;
/* LED2������ */
static TaskHandle_t Send_Task_Handle = NULL;



/* �������� */
static void AppTaskCreate(void); /* ���ڴ������� */
static void Receive_Task(void* pvParameters); /* LED_Task����ʵ�� */
static void Receive1_Task(void* pvParameters); /* LED_Task����ʵ�� */
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
	
	
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Receive_Task, /* ������ں��� */
                        (const char*    )"Receive_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Receive_Task_Handle);/* ������ƿ�ָ�� */
												
	if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
	
	 /* ����LED1_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Receive1_Task, /* ������ں��� */
                        (const char*    )"Receive1_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )3,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Receive1_Task_Handle);/* ������ƿ�ָ�� */
												
	if(pdPASS == xReturn)
    printf("����LED1_Task����ɹ�!\r\n");
	
												
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
  BaseType_t xReturn = pdFALSE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  uint32_t r_num;
	/* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    xTaskNotifyWait(0x0,ULONG_MAX,&r_num,portMAX_DELAY);//�������˳��Ƿ���Ҫɾ��
		if(pdTRUE == xReturn)//Ĭ��ΪpdFALSE �����ȡ�����pdTRUE
		{
			printf("Receive_Task ����֪ͨ��ϢΪ %d \n",r_num);
		}
	}
}

static void Receive1_Task(void* parameter)
{		
   BaseType_t xReturn = pdFALSE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  uint32_t r_num;
	/* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    xTaskNotifyWait(0x0,ULONG_MAX,&r_num,portMAX_DELAY);//�������˳��Ƿ���Ҫɾ��
		if(pdTRUE == xReturn)//Ĭ��ΪpdFALSE �����ȡ�����pdTRUE
		{
			printf("Receive1_Task ����֪ͨ��ϢΪ %d \n",r_num);
		}
	}
}



static void Send_Task(void* parameter)
{	
  uint32_t send1 = 1;
	uint32_t send2 = 2;
	
	/* ������һ������ѭ�������ܷ��� */
  while (1)
  {
		//���KEY1������
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       
		{	
			xTaskNotify(Receive_Task_Handle,send1,eSetValueWithoutOverwrite);//����������������Ϣ�����ͷ�ʽ(����д��)			
		}
		
    //���KEY2������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       
		{				
			xTaskNotify(Receive1_Task_Handle,send2,eSetValueWithoutOverwrite);//����������������Ϣ�����ͷ�ʽ(����д��)				
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
