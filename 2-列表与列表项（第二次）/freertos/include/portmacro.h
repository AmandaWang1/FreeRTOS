#ifndef _PORTMACRO_H
#define _PORTMACRO_H

#include "stdint.h"
#include "stddef.h"


/* 数据类型重定义 */
#define			 portCHAR       	 char
#define			 portFLOAT    		 float
#define 		 portDOUBLE 			 double
#define 		 portLONG    			 long
#define 		 portSHORT  	   	 short
#define	 		 portSTACK_TYPE 	 uint32_t
#define 		 portBASE_TYPE 		 long
	
typedef 		 portSTACK_TYPE    StackType_t;
typedef			 long					   	 BaseType_t;
typedef  unsigned  long  UBaseType_t;


/* TickType_t具体是16位还是32位， *
 *由 configUSE_16_BIT_TICKS 决定  *
 *这个宏在 FreeRTOSConfig.h 中定义*/
#if ( configUSE_16_BIT_TICKS == 1 )
typedef    uint16_t    TickType_t;
#define portMAX_DELAY  ( TickType_t )   0xffff
#else 
typedef    uint32_t    TickType_t;
#define portMAX_DELAY ( TickType_t )  0xffffffffUL
#endif


#endif /* _PORTMACRO_H */


