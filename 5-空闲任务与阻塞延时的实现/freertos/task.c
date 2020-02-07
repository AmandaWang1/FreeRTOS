#include "FreeRTOS.h"
#include "task.h"


/* 任务就绪列表 */
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

static TaskHandle_t xIdleTaskHandle      =  NULL;

TCB_t * volatile pxCurrentTCB = NULL;  //volatile:让编译器从原地址去找变量，而不是从引用的地址去找

static volatile TickType_t xTickCount 				= ( TickType_t ) 0U;



/*
*************************************************************************
*                               函数声明
*************************************************************************
*/

#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* 任务入口 */
									const char * const pcName,              /* 任务名称，字符串形式 */
									const uint32_t ulStackDepth,            /* 任务栈大小，单位为字 */
									void * const pvParameters,              /* 任务形参 */
									TaskHandle_t * const pxCreatedTask,     /* 任务句柄 */
									TCB_t *pxNewTCB );

extern  TCB_t  IdleTaskTCB;
									

/*
*************************************************************************
*                               静态任务创建函数
*************************************************************************
*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )//通过这个宏来决定是否编译

TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* 任务入口 */
					            const char * const pcName,           /* 任务名称，字符串形式 */
					            const uint32_t ulStackDepth,         /* 任务栈大小，单位为字 */
					            void * const pvParameters,           /* 任务形参 */
					            StackType_t * const puxStackBuffer,  /* 任务栈起始地址 */
					            TCB_t * const pxTaskBuffer )         /* 任务控制块指针 */
{
	TCB_t *pxNewTCB;/* 定义一个任务的控制块 */
	TaskHandle_t xReturn;

	if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
	{		
		pxNewTCB = ( TCB_t * ) pxTaskBuffer; 
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;

		/* 创建新的任务 */
		prvInitialiseNewTask( pxTaskCode,        /* 任务入口 */
                              pcName,            /* 任务名称，字符串形式 */
                              ulStackDepth,      /* 任务栈大小，单位为字 */ 
                              pvParameters,      /* 任务形参 */
                              &xReturn,          /* 任务句柄 */ 
                              pxNewTCB);         /* 任务栈起始地址 */      

	}
	else
	{
		xReturn = NULL;
	}

	/* 返回任务句柄，如果任务创建成功，此时xReturn应该指向任务控制块 */
    return xReturn;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */


//任务块的实现，初始化任务
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* 任务入口 */
									const char * const pcName,              /* 任务名称，字符串形式 */
									const uint32_t ulStackDepth,            /* 任务栈大小，单位为字 */
									void * const pvParameters,              /* 任务形参 */
									TaskHandle_t * const pxCreatedTask,     /* 任务句柄 */
									TCB_t *pxNewTCB )                       /* 任务控制块指针 */

{
	StackType_t *pxTopOfStack;		//定义了一个栈顶指针
	UBaseType_t x;	
	
	/* 获取栈顶地址 */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	//pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
	
	/* 向下做8字节对齐 因为在CM3，4，7中总线宽度32位，通常只需要做四字节对齐，
		但是一些浮点数是64位，所以需要做八字节对齐	*/
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x0007 ) ) );	

	/* 将任务的名字存储在TCB中 */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		if( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	/* 任务名字的长度不能超过configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

    /* 初始化TCB中的xStateListItem节点 */
    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
    /* 设置xStateListItem节点的拥有者 */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
    
    
    /* 初始化任务栈 */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );   


	/* 让任务句柄指向(赋值)任务控制块 */
    if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
}


/* 初始化任务相关的列表 */
void prvInitialiseTaskLists( void )
{
    UBaseType_t uxPriority;
    
    //通过for循环，将每一个成员变量都进行一个初始化
  for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) configMAX_PRIORITIES; uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}
}


extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
extern void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
																					StackType_t **ppxIdleTaskStackBuffer, 
																					uint32_t *pulIdleTaskStackSize );


static portTASK_FUNCTION( prvIdleTask, pvParameters )    //等效于 void prvIdleTask( void *pvParameters )
{
	/* 防止编译器的警告 */
	( void ) pvParameters;
    
    for(;;)
    {
        /* 空闲任务暂时什么都不做 */
    }
}

void vTaskStartScheduler( void )
{
	
	/*======================================创建空闲任务start==============================================*/     
    TCB_t *pxIdleTaskTCBBuffer = NULL;               /* 用于指向空闲任务控制块 */
    StackType_t *pxIdleTaskStackBuffer = NULL;       /* 用于空闲任务栈起始地址 */
    uint32_t ulIdleTaskStackSize;
    
    /* 获取空闲任务的内存：任务栈和任务TCB */
    vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, 
                                   &pxIdleTaskStackBuffer, 
                                   &ulIdleTaskStackSize );    
    
    xIdleTaskHandle = xTaskCreateStatic( (TaskFunction_t)prvIdleTask,              /* 任务入口 */
					                     (char *)"IDLE",                           /* 任务名称，字符串形式 */
					                     (uint32_t)ulIdleTaskStackSize ,           /* 任务栈大小，单位为字 */
					                     (void *) NULL,                            /* 任务形参 */
					                     (StackType_t *)pxIdleTaskStackBuffer,     /* 任务栈起始地址 */
					                     (TCB_t *)pxIdleTaskTCBBuffer );           /* 任务控制块 */
															 
    /* 将任务添加到就绪列表 */ 	
	  /*空闲任务一定要是最低优先级的任务 ，当前没有任务可运行时才会去运行空闲任务 */                               
    vListInsertEnd( &( pxReadyTasksLists[0] ), &( ((TCB_t *)pxIdleTaskTCBBuffer)->xStateListItem ) );
/*======================================创建空闲任务end================================================*/

															 
    /* 手动指定第一个运行的任务 */
    pxCurrentTCB = &Task1TCB;
    
    /* 启动调度器 */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* 调度器启动成功，则不会返回，即不会来到这里 */
    }
}

//void vTaskSwitchContext( void )
//{    
//    /* 两个任务轮流切换 */
//	/* 判断当前任务控制块是否为Task1，
//		如果是，切换到Task2 ，否则切换为Task1 */
//    if( pxCurrentTCB == &Task1TCB )   
//    {
//        pxCurrentTCB = &Task2TCB;
//    }
//    else
//    {
//        pxCurrentTCB = &Task1TCB;
//    }
//}


void vTaskDelay( const TickType_t xTicksToDelay )
{
    TCB_t *pxTCB = NULL;
    
    /* 获取当前任务的TCB */
    pxTCB = pxCurrentTCB;
    
    /* 设置延时时间 */
    pxTCB->xTicksToDelay = xTicksToDelay;
    
    /* 任务切换 */
    taskYIELD();
}


void vTaskSwitchContext( void )
{
	/* 如果当前线程是空闲线程，那么就去尝试执行线程1或者线程2，
       看看他们的延时时间是否结束，如果线程的延时时间均没有到期，
       那就返回继续执行空闲线程 */
	if( pxCurrentTCB == &IdleTaskTCB )  		//如果当前任务控制块指向一个空闲任务
	{
		if(Task1TCB.xTicksToDelay == 0)   		//判断任务1延时是否到了
		{            
            pxCurrentTCB =&Task1TCB;  		//如果到了，当前任务切换为任务1
		}
		else if(Task2TCB.xTicksToDelay == 0)  //否则判断任务2延时是否到了
		{
            pxCurrentTCB =&Task2TCB;      //如果到了，当前任务切换为任务2
		}
		else																	//如果两个任务延时均未达到，直接返回
		{
			return;		/* 线程延时均没有到期则返回，继续执行空闲线程 */
		} 
	}
	else			//如果当前任务不是空闲任务
	{
		/*如果当前线程是线程1或者线程2的话，检查下另外一个线程,如果另外的线程不在延时中，就切换到该线程
        否则，判断下当前线程是否应该进入延时状态，如果是的话，就切换到空闲线程。否则就不进行任何切换 */
		if(pxCurrentTCB == &Task1TCB)									//如果当前任务是任务1
		{
			if(Task2TCB.xTicksToDelay == 0)							//检查任务2是否到期
			{
                pxCurrentTCB =&Task2TCB;					//如果到期切换为任务2
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)		//否则判断当前任务是否要进入延时状态
			{
                pxCurrentTCB = &IdleTaskTCB;			//如果是，切换为空闲任务
			}
			else   //否则就继续执行，不进行切换
			{
				return;		/* 返回，不进行切换，因为两个线程都处于延时中 */
			}
		}
		else if(pxCurrentTCB == &Task2TCB)				//任务2 同理任务1
		{
			if(Task1TCB.xTicksToDelay == 0)
			{
                pxCurrentTCB =&Task1TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)
			{
                pxCurrentTCB = &IdleTaskTCB;
			}
			else 
			{
				return;		/* 返回，不进行切换，因为两个线程都处于延时中 */
			}
		}
	}
}


void xTaskIncrementTick( void )
{
    TCB_t *pxTCB = NULL;
    BaseType_t i = 0;
    
    /* 更新系统时基计数器xTickCount，xTickCount是一个在port.c中定义的全局变量 */
    const TickType_t xConstTickCount = xTickCount + 1;
    xTickCount = xConstTickCount;

    
    /* 扫描就绪列表中所有线程的xTicksToDelay，如果不为0，则减1 */
	for(i=0; i<configMAX_PRIORITIES; i++)
	{
		//从就绪列表中找到一个任务控制块
    pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &pxReadyTasksLists[i] ) );
		if(pxTCB->xTicksToDelay > 0)
		{
			pxTCB->xTicksToDelay --;
		}
	}
    
    /* 任务切换 */
    portYIELD();
}




