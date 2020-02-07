/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"  


/*
	实验步骤：
	1.定义软件定时器句柄
	2.实现回调函数 void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);
	3.创建定时器——xTimerCreate()
	4.启动定时器——xTimerStart()
*/


/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;

/* 软件定时器句柄 */
static xTimerHandle Swtmr1_Handle = NULL;
static xTimerHandle Swtmr2_Handle = NULL;

static uint32_t TmrCb_Count1 = 0;//记录软件定时器1回调函数执行次数
static uint32_t TmrCb_Count2 = 0;//记录软件定时器2回调函数执行次数

/* 函数声明 */
static void AppTaskCreate(void); /* 用于创建任务 */
static void BSP_Init(void);/* 用于初始化板载相关资源 */
void Swtmr1_Callback(TimerHandle_t xTimer);
void Swtmr2_Callback(TimerHandle_t xTimer);


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
	
															//自动重装在的软件定时器，周期,是否自动重装载,ID,回调函数
	Swtmr1_Handle = xTimerCreate("AutoReloadTimer",1000,pdTRUE,(void*)1,Swtmr1_Callback);
	xTimerStart(Swtmr1_Handle,0);//开启
	
	Swtmr2_Handle = xTimerCreate("OneShotTimer",5000,pdFALSE,(void*)2,Swtmr2_Callback);
	xTimerStart(Swtmr2_Handle,0);
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

//回调函数
void Swtmr1_Callback(TimerHandle_t xTimer)
{
	TickType_t tick_num1;
	TmrCb_Count1++;//每回调一次加一
	tick_num1 = xTaskGetTickCount();//获取滴答定时器的计数值
	//LED1_TOGGLE;
	printf("swtmr1_Callback函数执行 %d 次\n",TmrCb_Count1);
	printf("滴答定时器数值 = %d\n",tick_num1);
	
}

void Swtmr2_Callback(TimerHandle_t xTimer)
{
	TickType_t tick_num2;
	TmrCb_Count2++;//每回调一次加一
	tick_num2 = xTaskGetTickCount();//获取滴答定时器的计数值
	//LED2_TOGGLE;
	printf("swtmr2_Callback函数执行 %d 次\n",TmrCb_Count2);
	printf("滴答定时器数值 = %d\n",tick_num2);
	
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
