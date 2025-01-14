/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"//消息队列头文件
#include "semphr.h"//信号量头文件
#include "limits.h"

/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


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
/* LED1任务句柄 */
static TaskHandle_t Receive1_Task_Handle = NULL;
/* LED2任务句柄 */
static TaskHandle_t Send_Task_Handle = NULL;



/* 函数声明 */
static void AppTaskCreate(void); /* 用于创建任务 */
static void Receive_Task(void* pvParameters); /* LED_Task任务实现 */
static void Receive1_Task(void* pvParameters); /* LED_Task任务实现 */
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
	uint32_t take_num = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
  /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
    //如果KEY1被单击
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       
		{
      /* uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ); 
       * xClearCountOnExit：pdTRUE 在退出函数的时候任务任务通知值清零，类似二值信号量
       * pdFALSE 在退出函数ulTaskNotifyTakeO的时候任务通知值减一，类似计数型信号量。
       */
      //获取任务通知 ,没获取到则不等待
      take_num=ulTaskNotifyTake(pdFALSE,0);//
      if(take_num > 0)
        printf( "KEY1被按下，成功申请到停车位，当前车位为 %d \n", take_num - 1);
			else
        printf( "KEY1被按下，车位已经没有了，请按KEY2释放车位\n" );  
		}
		vTaskDelay(20);     //每20ms扫描一次		
  }
}


static void Send_Task(void* parameter)
{	
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
    //如果KEY2被单击
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       
		{
      /* 原型:BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify ); */
			/* 释放一个任务通知 */
      xTaskNotifyGive(Send_Task_Handle);//发送任务通知
      /* 此函数只会返回pdPASS */
			if ( pdPASS == xReturn ) 
				printf( "KEY2被按下，释放1个停车位。\n" );
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
