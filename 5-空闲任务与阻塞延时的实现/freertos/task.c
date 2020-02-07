#include "FreeRTOS.h"
#include "task.h"


/* ��������б� */
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

static TaskHandle_t xIdleTaskHandle      =  NULL;

TCB_t * volatile pxCurrentTCB = NULL;  //volatile:�ñ�������ԭ��ַȥ�ұ����������Ǵ����õĵ�ַȥ��

static volatile TickType_t xTickCount 				= ( TickType_t ) 0U;



/*
*************************************************************************
*                               ��������
*************************************************************************
*/

#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
									const char * const pcName,              /* �������ƣ��ַ�����ʽ */
									const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
									void * const pvParameters,              /* �����β� */
									TaskHandle_t * const pxCreatedTask,     /* ������ */
									TCB_t *pxNewTCB );

extern  TCB_t  IdleTaskTCB;
									

/*
*************************************************************************
*                               ��̬���񴴽�����
*************************************************************************
*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )//ͨ��������������Ƿ����

TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
					            const char * const pcName,           /* �������ƣ��ַ�����ʽ */
					            const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
					            void * const pvParameters,           /* �����β� */
					            StackType_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
					            TCB_t * const pxTaskBuffer )         /* ������ƿ�ָ�� */
{
	TCB_t *pxNewTCB;/* ����һ������Ŀ��ƿ� */
	TaskHandle_t xReturn;

	if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
	{		
		pxNewTCB = ( TCB_t * ) pxTaskBuffer; 
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;

		/* �����µ����� */
		prvInitialiseNewTask( pxTaskCode,        /* ������� */
                              pcName,            /* �������ƣ��ַ�����ʽ */
                              ulStackDepth,      /* ����ջ��С����λΪ�� */ 
                              pvParameters,      /* �����β� */
                              &xReturn,          /* ������ */ 
                              pxNewTCB);         /* ����ջ��ʼ��ַ */      

	}
	else
	{
		xReturn = NULL;
	}

	/* ������������������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ� */
    return xReturn;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */


//������ʵ�֣���ʼ������
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
									const char * const pcName,              /* �������ƣ��ַ�����ʽ */
									const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
									void * const pvParameters,              /* �����β� */
									TaskHandle_t * const pxCreatedTask,     /* ������ */
									TCB_t *pxNewTCB )                       /* ������ƿ�ָ�� */

{
	StackType_t *pxTopOfStack;		//������һ��ջ��ָ��
	UBaseType_t x;	
	
	/* ��ȡջ����ַ */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	//pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
	
	/* ������8�ֽڶ��� ��Ϊ��CM3��4��7�����߿��32λ��ͨ��ֻ��Ҫ�����ֽڶ��룬
		����һЩ��������64λ��������Ҫ�����ֽڶ���	*/
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x0007 ) ) );	

	/* ����������ִ洢��TCB�� */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		if( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	/* �������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

    /* ��ʼ��TCB�е�xStateListItem�ڵ� */
    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
    /* ����xStateListItem�ڵ��ӵ���� */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
    
    
    /* ��ʼ������ջ */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );   


	/* ��������ָ��(��ֵ)������ƿ� */
    if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
}


/* ��ʼ��������ص��б� */
void prvInitialiseTaskLists( void )
{
    UBaseType_t uxPriority;
    
    //ͨ��forѭ������ÿһ����Ա����������һ����ʼ��
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


static portTASK_FUNCTION( prvIdleTask, pvParameters )    //��Ч�� void prvIdleTask( void *pvParameters )
{
	/* ��ֹ�������ľ��� */
	( void ) pvParameters;
    
    for(;;)
    {
        /* ����������ʱʲô������ */
    }
}

void vTaskStartScheduler( void )
{
	
	/*======================================������������start==============================================*/     
    TCB_t *pxIdleTaskTCBBuffer = NULL;               /* ����ָ�����������ƿ� */
    StackType_t *pxIdleTaskStackBuffer = NULL;       /* ���ڿ�������ջ��ʼ��ַ */
    uint32_t ulIdleTaskStackSize;
    
    /* ��ȡ����������ڴ棺����ջ������TCB */
    vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, 
                                   &pxIdleTaskStackBuffer, 
                                   &ulIdleTaskStackSize );    
    
    xIdleTaskHandle = xTaskCreateStatic( (TaskFunction_t)prvIdleTask,              /* ������� */
					                     (char *)"IDLE",                           /* �������ƣ��ַ�����ʽ */
					                     (uint32_t)ulIdleTaskStackSize ,           /* ����ջ��С����λΪ�� */
					                     (void *) NULL,                            /* �����β� */
					                     (StackType_t *)pxIdleTaskStackBuffer,     /* ����ջ��ʼ��ַ */
					                     (TCB_t *)pxIdleTaskTCBBuffer );           /* ������ƿ� */
															 
    /* ��������ӵ������б� */ 	
	  /*��������һ��Ҫ��������ȼ������� ����ǰû�����������ʱ�Ż�ȥ���п������� */                               
    vListInsertEnd( &( pxReadyTasksLists[0] ), &( ((TCB_t *)pxIdleTaskTCBBuffer)->xStateListItem ) );
/*======================================������������end================================================*/

															 
    /* �ֶ�ָ����һ�����е����� */
    pxCurrentTCB = &Task1TCB;
    
    /* ���������� */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* �����������ɹ����򲻻᷵�أ��������������� */
    }
}

//void vTaskSwitchContext( void )
//{    
//    /* �������������л� */
//	/* �жϵ�ǰ������ƿ��Ƿ�ΪTask1��
//		����ǣ��л���Task2 �������л�ΪTask1 */
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
    
    /* ��ȡ��ǰ�����TCB */
    pxTCB = pxCurrentTCB;
    
    /* ������ʱʱ�� */
    pxTCB->xTicksToDelay = xTicksToDelay;
    
    /* �����л� */
    taskYIELD();
}


void vTaskSwitchContext( void )
{
	/* �����ǰ�߳��ǿ����̣߳���ô��ȥ����ִ���߳�1�����߳�2��
       �������ǵ���ʱʱ���Ƿ����������̵߳���ʱʱ���û�е��ڣ�
       �Ǿͷ��ؼ���ִ�п����߳� */
	if( pxCurrentTCB == &IdleTaskTCB )  		//�����ǰ������ƿ�ָ��һ����������
	{
		if(Task1TCB.xTicksToDelay == 0)   		//�ж�����1��ʱ�Ƿ���
		{            
            pxCurrentTCB =&Task1TCB;  		//������ˣ���ǰ�����л�Ϊ����1
		}
		else if(Task2TCB.xTicksToDelay == 0)  //�����ж�����2��ʱ�Ƿ���
		{
            pxCurrentTCB =&Task2TCB;      //������ˣ���ǰ�����л�Ϊ����2
		}
		else																	//�������������ʱ��δ�ﵽ��ֱ�ӷ���
		{
			return;		/* �߳���ʱ��û�е����򷵻أ�����ִ�п����߳� */
		} 
	}
	else			//�����ǰ�����ǿ�������
	{
		/*�����ǰ�߳����߳�1�����߳�2�Ļ������������һ���߳�,���������̲߳�����ʱ�У����л������߳�
        �����ж��µ�ǰ�߳��Ƿ�Ӧ�ý�����ʱ״̬������ǵĻ������л��������̡߳�����Ͳ������κ��л� */
		if(pxCurrentTCB == &Task1TCB)									//�����ǰ����������1
		{
			if(Task2TCB.xTicksToDelay == 0)							//�������2�Ƿ���
			{
                pxCurrentTCB =&Task2TCB;					//��������л�Ϊ����2
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)		//�����жϵ�ǰ�����Ƿ�Ҫ������ʱ״̬
			{
                pxCurrentTCB = &IdleTaskTCB;			//����ǣ��л�Ϊ��������
			}
			else   //����ͼ���ִ�У��������л�
			{
				return;		/* ���أ��������л�����Ϊ�����̶߳�������ʱ�� */
			}
		}
		else if(pxCurrentTCB == &Task2TCB)				//����2 ͬ������1
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
				return;		/* ���أ��������л�����Ϊ�����̶߳�������ʱ�� */
			}
		}
	}
}


void xTaskIncrementTick( void )
{
    TCB_t *pxTCB = NULL;
    BaseType_t i = 0;
    
    /* ����ϵͳʱ��������xTickCount��xTickCount��һ����port.c�ж����ȫ�ֱ��� */
    const TickType_t xConstTickCount = xTickCount + 1;
    xTickCount = xConstTickCount;

    
    /* ɨ������б��������̵߳�xTicksToDelay�������Ϊ0�����1 */
	for(i=0; i<configMAX_PRIORITIES; i++)
	{
		//�Ӿ����б����ҵ�һ��������ƿ�
    pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &pxReadyTasksLists[i] ) );
		if(pxTCB->xTicksToDelay > 0)
		{
			pxTCB->xTicksToDelay --;
		}
	}
    
    /* �����л� */
    portYIELD();
}




