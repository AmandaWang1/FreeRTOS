#ifndef _LED_H
#define _LED_H          //��ֹ�ظ�����

#include "stm32f10x.h"

//��Ӳ����صĶ���ɺ�

#define LED1_GPIO_PIN                  GPIO_Pin_1
#define LED1_GPIO_PORT                 GPIOA
#define LED1_GPIO_CLK                  RCC_APB2Periph_GPIOA


//#define    ON      1
//#define    OFF     0

//#define   LED(a)  if(a) GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN); else 	GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);

void led (void);

#endif /*_LED_H*/

