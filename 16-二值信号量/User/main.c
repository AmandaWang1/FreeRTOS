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
	2.����һ����ֵ�ź��� xSemaphoreCreateBinary()
	3.�������л�ȡ�ź��� xSemaphoreTake()
	4.���������¾��ͷ�һ���ź��� xSemaphoreGive()
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
static TaskHandle_t TAKE_Task_Handle = NULL;
/* LED2������ */
static TaskHandle_t GIVE_Task_Handle = NULL;
/* ��ֵ�ź������ */
SemaphoreHandle_t BinarySem_Handle =NULL;

#define  QUEUE_LEN    4   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  QUEUE_SIZE   4   /* ������ÿ����Ϣ��С���ֽڣ� */


/* �������� */
static void AppTaskCreate(void); /* ���ڴ������� */
static void TAKE_Task(void* pvParameters); /* LED_Task����ʵ�� */
static void GIVE_Task(void* pvParameters); /* LED2_Task����ʵ�� */
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
	
	BinarySem_Handle = xSemaphoreCreateBinary();//���ô�����������ǻ᷵��һ����ֵ�ź����ľ��//������ֵ�ź���
	
	
  if(NULL != BinarySem_Handle)
    printf("����BinarySem�ɹ�!\r\n");
	
  /* ����TAKE_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )TAKE_Task, /* ������ں��� */
                        (const char*    )"TAKE_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&TAKE_Task_Handle);/* ������ƿ�ָ�� */
												
	if(pdPASS == xReturn)
    printf("����TAKE_Task����ɹ�!\r\n");
												
	/* ����LED2_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )GIVE_Task, /* ������ں��� */
                        (const char*    )"GIVE_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&GIVE_Task_Handle);/* ������ƿ�ָ�� */
												
												
  if(pdPASS == xReturn)
    printf("����GIVE_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}


static void TAKE_Task(void* parameter)
{		
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
  while (1)
  {
  xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);//��������ֵ�ź������������ʱ�� //��ȡ�ź���		
		
    if(pdTRUE == xReturn)
      printf("��ȡ�ź����ɹ�\n");
    else
      printf("��ȡ�ź���ʧ��\n");
  }
}


static void GIVE_Task(void* parameter)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {/* K1 ������ */
      xReturn = xSemaphoreGive(BinarySem_Handle);//���������ĸ��ź�������
      if(pdPASS == xReturn)
        printf("�����ź����ɹ�!\n\n");
    } 
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )
    {/* K2 ������ */
       xReturn = xSemaphoreGive(BinarySem_Handle);//���������ĸ��ź�������
      if(pdPASS == xReturn)
        printf("�����ź����ɹ�!\n\n");
    }
    vTaskDelay(20);/* ��ʱ20��tick */
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
