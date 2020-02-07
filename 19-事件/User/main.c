/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"//消息队列头文件
#include "semphr.h"//信号量头文件
#include "event_groups.h "//事件组

/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


/*
	实验步骤：
	1.定义事件句柄，通过宏定义设置事件掩码位
	2.创建事件——xEventGroupCreate()
	3.实现等待事件任务——xEventGroupWaitBits()
	4.实现设置事件任务——xEventGroupSetBits()
*/


/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED任务句柄 */
static TaskHandle_t Receive_Task_Handle = NULL;
/* LED2任务句柄 */
static TaskHandle_t Send_Task_Handle = NULL;
/* 定义事件句柄 */
static EventGroupHandle_t Event_Handle = NULL;

#define EVENT1 (0X01 << 1)
#define EVENT2 (0X01 << 2)

/* 函数声明 */
static void AppTaskCreate(void); /* 用于创建任务 */
static void Receive_Task(void* pvParameters); /* LED_Task任务实现 */
static void Send_Task(void* pvParameters); /* LED2_Task任务实现 */
static void BSP_Init(void);/* 用于初始化板载相关资源 */


int main(void)
{	
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	BSP_Init();
	
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
												
	/* 如果返回值正确，启动任务调度 */           
  if(pdPASS == xReturn) 
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
	
	Event_Handle = xEventGroupCreate();//创建一个事件
	
  if(NULL != Event_Handle)
    printf("创建Event_Handle成功!\r\n");
	
  /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Receive_Task, /* 任务入口函数 */
                        (const char*    )"Receive_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Receive_Task_Handle);/* 任务控制块指针 */
												
	if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\r\n");
												
	/* 创建LED2_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Send_Task, /* 任务入口函数 */
                        (const char*    )"Send_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Send_Task_Handle);/* 任务控制块指针 */
												
												
  if(pdPASS == xReturn)
    printf("创建KEY_Task任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}


static void Receive_Task(void* parameter)
{		
  EventBits_t xReturn = pdTRUE;/* 定义一个返回值保存事件 */
  
  while (1)
  {
		//通过返回值判断事件是否已经发生
		xReturn = xEventGroupWaitBits(Event_Handle,EVENT1|EVENT2,pdTRUE,pdTRUE,portMAX_DELAY);//参数：事件句柄，等待事件的位,是否退出事件清除，等待所有的位，等待一个事件的所有时间
 
		if((xReturn & (EVENT1|EVENT2)) == (EVENT1|EVENT2) )//确定已经发生
		{
			printf("KEY1与KEY2都按下\n");
			LED1_TOGGLE;
		}
		else
			printf("事件错误\n");
	}
}


static void Send_Task(void* parameter)
{	
  /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
    //如果KEY1被单击
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       
		{
			//设置一个事件	
			xEventGroupSetBits(Event_Handle,EVENT1);
			printf("KEY1被单击\n");
		}
		
		//如果KEY2被单击
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       
		{
			//设置一个事件	
			xEventGroupSetBits(Event_Handle,EVENT2);
			printf("KEY2被单击\n");
		}
		vTaskDelay(20);     //每20ms扫描一次	
  } 
}

static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* LED初始化 */
	LED_GPIO_Config();
	
	/* 串口初始化 */
	USART_Config();
	
	/* 按键初始化 */
	Key_GPIO_Config();
	
}
