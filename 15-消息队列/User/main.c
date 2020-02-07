/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"//��Ϣ����ͷ�ļ�

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


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
/* LED2������ */
static TaskHandle_t Send_Task_Handle = NULL;

QueueHandle_t Test_Queue;//�������о��


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
	
	//������Ϣ����
	Test_Queue = xQueueCreate(QUEUE_LEN,QUEUE_SIZE); //�������������һ�����г��ȣ�һ�����д�С	//��������֮ǰ�ȴ�������
  
  if(NULL != Test_Queue)
    printf("����Test_Queue��Ϣ���гɹ�!\r\n");
	
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
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
  uint32_t r_queue;	/* ����һ��������Ϣ�ı��� */
  while (1)
  {
    xReturn = xQueueReceive( Test_Queue,    /* ��Ϣ���еľ�� */
                             &r_queue,      /* ���͵���Ϣ���� */
                             portMAX_DELAY); /* �ȴ�ʱ�� һֱ�� */
    if(pdTRUE == xReturn)
      printf("���ν��յ���������%d\n\n",r_queue);
    else
      printf("���ݽ��ճ���,�������0x%lx\n",xReturn);
  }
}


static void Send_Task(void* parameter)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  uint32_t send_data1 = 1;
  uint32_t send_data2 = 2;
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {/* K1 ������ */
      printf("������Ϣsend_data1��\n");
      xReturn = xQueueSend( Test_Queue, /* ��Ϣ���еľ�� */
                            &send_data1,/* ���͵���Ϣ���� */
                            0 );        /* �ȴ�ʱ�� 0 */
      if(pdPASS == xReturn)
        printf("��Ϣsend_data1���ͳɹ�!\n\n");
    } 
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )
    {/* K2 ������ */
      printf("������Ϣsend_data2��\n");
      xReturn = xQueueSend( Test_Queue, /* ��Ϣ���еľ�� */
                            &send_data2,/* ���͵���Ϣ���� */
                            0 );        /* �ȴ�ʱ�� 0 */
      if(pdPASS == xReturn)
        printf("��Ϣsend_data2���ͳɹ�!\n\n");
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
