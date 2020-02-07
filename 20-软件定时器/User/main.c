/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


/*
	ʵ�鲽�裺
	1.���������ʱ�����
	2.ʵ�ֻص����� void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);
	3.������ʱ������xTimerCreate()
	4.������ʱ������xTimerStart()
*/


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;

/* �����ʱ����� */
static xTimerHandle Swtmr1_Handle = NULL;
static xTimerHandle Swtmr2_Handle = NULL;

static uint32_t TmrCb_Count1 = 0;//��¼�����ʱ��1�ص�����ִ�д���
static uint32_t TmrCb_Count2 = 0;//��¼�����ʱ��2�ص�����ִ�д���

/* �������� */
static void AppTaskCreate(void); /* ���ڴ������� */
static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */
void Swtmr1_Callback(TimerHandle_t xTimer);
void Swtmr2_Callback(TimerHandle_t xTimer);


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
	
															//�Զ���װ�ڵ������ʱ��������,�Ƿ��Զ���װ��,ID,�ص�����
	Swtmr1_Handle = xTimerCreate("AutoReloadTimer",1000,pdTRUE,(void*)1,Swtmr1_Callback);
	xTimerStart(Swtmr1_Handle,0);//����
	
	Swtmr2_Handle = xTimerCreate("OneShotTimer",5000,pdFALSE,(void*)2,Swtmr2_Callback);
	xTimerStart(Swtmr2_Handle,0);
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�ص�����
void Swtmr1_Callback(TimerHandle_t xTimer)
{
	TickType_t tick_num1;
	TmrCb_Count1++;//ÿ�ص�һ�μ�һ
	tick_num1 = xTaskGetTickCount();//��ȡ�δ�ʱ���ļ���ֵ
	//LED1_TOGGLE;
	printf("swtmr1_Callback����ִ�� %d ��\n",TmrCb_Count1);
	printf("�δ�ʱ����ֵ = %d\n",tick_num1);
	
}

void Swtmr2_Callback(TimerHandle_t xTimer)
{
	TickType_t tick_num2;
	TmrCb_Count2++;//ÿ�ص�һ�μ�һ
	tick_num2 = xTaskGetTickCount();//��ȡ�δ�ʱ���ļ���ֵ
	//LED2_TOGGLE;
	printf("swtmr2_Callback����ִ�� %d ��\n",TmrCb_Count2);
	printf("�δ�ʱ����ֵ = %d\n",tick_num2);
	
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
