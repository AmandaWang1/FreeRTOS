#include "stm32f10x.h"
#include "bsp_led.h"


void Delay (uint32_t count)
{
	for(;count!=0;count--);
}


int main (void)
{
	led ();
	
	while(1)
	{
		GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
		Delay(0xfffff);
		GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
		Delay(0xfffff);
	}
//	while(1)
//	{
//		LED(ON);
//		delay();
//		LED(OFF);
//		delay();
//	}
}

