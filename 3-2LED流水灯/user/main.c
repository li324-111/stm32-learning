#include "stm32f10x.h"
#include "Timer.h"

extern volatile uint32_t Timer2_Count;
int main(void)
{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
	
	Timer2_Init();
	uint16_t LED_Number = 0;
	uint32_t LED_LastTime = 0;

	while (1)
	{
		if (Timer2_Count - LED_LastTime >= 100)
		{
			LED_LastTime = Timer2_Count;

			GPIO_Write(GPIOA, ~(0x0001 << LED_Number));

			LED_Number++;

			if (LED_Number >= 8)
			{
				LED_Number = 0;
			}
		}
	}
}
