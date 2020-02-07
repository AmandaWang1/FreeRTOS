#ifndef __LED_H
#define	__LED_H


#include "stm32f10x.h"


/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */

#define LED1_GPIO_PORT    	GPIOD			              /* GPIO端口 */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define LED1_GPIO_PIN				GPIO_Pin_3			        /* 连接到SCL时钟线的GPIO */

#define LED2_GPIO_PORT    	GPIOE			              /* GPIO端口 */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define LED2_GPIO_PIN				GPIO_Pin_5			        /* 连接到SCL时钟线的GPIO */


/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* 使用标准的固件库控制IO*/
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN)
//					
//#define LED2(a)	if (a)	\
//					GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);\
//					else		\
//					GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN)


///* 定义控制IO的宏 */
//#define LED1_TOGGLE		 digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
//#define LED1_OFF		   digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
//#define LED1_ON			   digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)
					

void LED_GPIO_Config(void);

#endif /* __LED_H */
