/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"//消息队列头文件
#include "semphr.h"//信号量头文件

/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


/*
	实验步骤：
	1.定义信号量句柄
	2.创建一个二值信号量 xSemaphoreCreateBinary()
	3.在任务中获取信号量 xSemaphoreTake()
	4.当按键按下就释放一个信号量 xSemaphoreGive()
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
static TaskHandle_t TAKE_Task_Handle = NULL;
/* LED2任务句柄 */
static TaskHandle_t GIVE_Task_Handle = NULL;
/* 二值信号量句柄 */
SemaphoreHandle_t BinarySem_Handle =NULL;

#define  QUEUE_LEN    4   /* 队列的长度，最大可包含多少个消息 */
#define  QUEUE_SIZE   4   /* 队列中每个消息大小（字节） */


/* 函数声明 */
static void AppTaskCreate(void); /* 用于创建任务 */
static void TAKE_Task(void* pvParameters); /* LED_Task任务实现 */
static void GIVE_Task(void* pvParameters); /* LED2_Task任务实现 */
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
	
	BinarySem_Handle = xSemaphoreCreateBinary();//不用传入参数，但是会返回一个二值信号量的句柄//创建二值信号量
	
	
  if(NULL != BinarySem_Handle)
    printf("创建BinarySem成功!\r\n");
	
  /* 创建TAKE_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )TAKE_Task, /* 任务入口函数 */
                        (const char*    )"TAKE_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&TAKE_Task_Handle);/* 任务控制块指针 */
												
	if(pdPASS == xReturn)
    printf("创建TAKE_Task任务成功!\r\n");
												
	/* 创建LED2_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )GIVE_Task, /* 任务入口函数 */
                        (const char*    )"GIVE_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&GIVE_Task_Handle);/* 任务控制块指针 */
												
												
  if(pdPASS == xReturn)
    printf("创建GIVE_Task任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}


static void TAKE_Task(void* parameter)
{		
  BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
  while (1)
  {
  xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);//参数：二值信号量句柄，阻塞时间 //获取信号量		
		
    if(pdTRUE == xReturn)
      printf("获取信号量成功\n");
    else
      printf("获取信号量失败\n");
  }
}


static void GIVE_Task(void* parameter)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {/* K1 被按下 */
      xReturn = xSemaphoreGive(BinarySem_Handle);//参数：向哪个信号量发送
      if(pdPASS == xReturn)
        printf("发送信号量成功!\n\n");
    } 
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )
    {/* K2 被按下 */
       xReturn = xSemaphoreGive(BinarySem_Handle);//参数：向哪个信号量发送
      if(pdPASS == xReturn)
        printf("发送信号量成功!\n\n");
    }
    vTaskDelay(20);/* 延时20个tick */
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
