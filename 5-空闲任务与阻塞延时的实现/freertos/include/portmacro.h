#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"


/* 数据类型重定义 */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif


/* 中断控制状态寄存器：0xe000ed04
 * Bit 28 PENDSVSET: PendSV 悬起位
 */
#define portNVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )

#define portSY_FULL_READ_WRITE		( 15 )

#define portYIELD()																\
{																				\
	/* 触发PendSV，产生上下文切换 */								                \
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;								\
	__dsb( portSY_FULL_READ_WRITE );											\
	__isb( portSY_FULL_READ_WRITE );											\
}

/* 临界区管理 */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portDISABLE_INTERRUPTS()				vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()					vPortSetBASEPRI( 0 )

#define portENTER_CRITICAL()					vPortEnterCritical()
#define portEXIT_CRITICAL()						vPortExitCritical()

#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortRaiseBASEPRI()   //凡是看到 FROM_ISR 结尾的函数\宏定义 它都是在中断中使用的
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortSetBASEPRI(x)


#define portINLINE __inline

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE __forceinline
#endif

static portFORCE_INLINE void vPortRaiseBASEPRI( void )  //不带返回值，不能嵌套，不能在中断中使用
{
	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		msr basepri, ulNewBASEPRI     //11   当中断优先级的信号大于11不能被相应，小于11则可以被响应
		dsb
		isb
	}
}


static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )  //可以嵌套（可以在中断中使用）
{
uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		mrs ulReturn, basepri      //原先 basepri 的值保存在返回值中
		msr basepri, ulNewBASEPRI  //再设置basepri 的值
		dsb
		isb
	}

	return ulReturn;
}

static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
	__asm
	{
		/* Barrier instructions are not used as this function is only used to
		lower the BASEPRI value. */
		msr basepri, ulBASEPRI
	}
}

/**
	关中断
	1、ulPortRaiseBASEPRI          //设置的是11   返回的是0     5 6 7 8 会响应

	2、ulPortRaiseBASEPRI          //设置的是5    返回的是11		5 6 7 8 不会响应


	开中断
	3.vPortSetBASEPRI(11)          //此时 basepri = 11					5 6 7 8 会响应

	4.vPortSetBASEPRI(0)  				 // 所有都会被响应

**/


#endif /* PORTMACRO_H */
