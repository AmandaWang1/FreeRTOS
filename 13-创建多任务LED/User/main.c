/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"


/**************************** ������ ********************************/
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED������ */
static TaskHandle_t LED_Task_Handle = NULL;
/* LED2������ */
static TaskHandle_t LED2_Task_Handle = NULL;


/* �������� */
static void AppTaskCreate(void); /* ���ڴ������� */
static void LED_Task(void* pvParameters); /* LED_Task����ʵ�� */
static void LED2_Task(void* pvParameters); /* LED2_Task����ʵ�� */
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
												
      
  if(pdPASS == xReturn) 
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);      
}

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
												
	if(pdPASS == xReturn)
    printf("����LED����ɹ�!\n");
												
	/* ����LED2_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED2_Task, /* ������ں��� */
                        (const char*    )"LED2_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED2_Task_Handle);/* ������ƿ�ָ�� */
												
												
  if(pdPASS == xReturn)
    printf("����LED2����ɹ�!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}


static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1(ON);
        vTaskDelay(500);   /* ��ʱ500��tick */
        printf("LED����ִ��,LED1_ON\r\n");
			
			//	printf("SRAM��̬�����ɹ�");
        
        LED1(OFF);     
        vTaskDelay(500);   /* ��ʱ500��tick */		 		
        printf("LED����ִ��,LED1_OFF\r\n");
    }
}


static void LED2_Task(void* parameter)
{	
    while (1)
    {
        LED2(ON);
        vTaskDelay(1000);   /* ��ʱ1000��tick */
        printf("LED2����ִ��,LED2_ON\r\n");
			
				//printf("SRAM��̬�����ɹ�");
        
        LED2(OFF);     
        vTaskDelay(1000);   /* ��ʱ1000��tick */		 		
        printf("LED2����ִ��,LED2_OFF\r\n");
    }
}

static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* LED��ʼ�� */
	LED_GPIO_Config();
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
}
