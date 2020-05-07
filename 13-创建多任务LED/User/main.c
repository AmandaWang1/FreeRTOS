/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"


/**************************** 任务句柄 ********************************/
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;
/* LED2任务句柄 */
static TaskHandle_t LED2_Task_Handle = NULL;


/* 函数声明 */
static void AppTaskCreate(void); /* 用于创建任务 */
static void LED_Task(void* pvParameters); /* LED_Task任务实现 */
static void LED2_Task(void* pvParameters); /* LED2_Task任务实现 */
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
												
      
  if(pdPASS == xReturn) 
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);      
}

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
  
  /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* 任务入口函数 */
                        (const char*    )"LED_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED_Task_Handle);/* 任务控制块指针 */
												
	if(pdPASS == xReturn)
    printf("创建LED任务成功!\n");
												
	/* 创建LED2_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED2_Task, /* 任务入口函数 */
                        (const char*    )"LED2_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED2_Task_Handle);/* 任务控制块指针 */
												
												
  if(pdPASS == xReturn)
    printf("创建LED2任务成功!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}


static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1(ON);
        vTaskDelay(500);   /* 延时500个tick */
        printf("LED正在执行,LED1_ON\r\n");
			
			//	printf("SRAM动态创建成功");
        
        LED1(OFF);     
        vTaskDelay(500);   /* 延时500个tick */		 		
        printf("LED正在执行,LED1_OFF\r\n");
    }
}


static void LED2_Task(void* parameter)
{	
    while (1)
    {
        LED2(ON);
        vTaskDelay(1000);   /* 延时1000个tick */
        printf("LED2正在执行,LED2_ON\r\n");
			
				//printf("SRAM动态创建成功");
        
        LED2(OFF);     
        vTaskDelay(1000);   /* 延时1000个tick */		 		
        printf("LED2正在执行,LED2_OFF\r\n");
    }
}

static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* LED初始化 */
	LED_GPIO_Config();
	
	/* 串口初始化 */
	USART_Config();
	
}
