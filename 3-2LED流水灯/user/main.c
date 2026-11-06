#include "stm32f10x.h"
#include "Timer.h"
#include "Serial.h"

extern volatile uint32_t Timer2_Count;
int main(void)
{
	/* 同时给GPIOA和GPIOC提供时钟 */
	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
		ENABLE
	);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* 配置PA0~PA7，控制8个外接LED */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0 |
		GPIO_Pin_1 |
		GPIO_Pin_2 |
		GPIO_Pin_3 |
		GPIO_Pin_4 |
		GPIO_Pin_5 |
		GPIO_Pin_6 |
		GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 配置PC13，控制开发板自带LED */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PC13先输出高电平，让板载LED初始熄灭 */
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	Timer2_Init();
	Serial_Init();
	Serial_SendByte('A');

	/* 外接流水灯任务的数据 */
	uint16_t LED_Number = 0;
	uint32_t LED_LastTime = 0;

	/* 板载LED任务的数据 */
	uint32_t BoardLED_LastTime = 0;
	uint8_t BoardLED_State = 0;

	while (1)
	{
		/* 任务1：每100ms移动一次外接流水灯 */
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

		/* 任务2：每500ms改变一次板载LED状态 */
		if (Timer2_Count - BoardLED_LastTime >= 500)
		{
			BoardLED_LastTime = Timer2_Count;

			if (BoardLED_State == 0)
			{
				/* PC13输出低电平，LED亮 */
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				BoardLED_State = 1;
			}
			else
			{
				/* PC13输出高电平，LED灭 */
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				BoardLED_State = 0;
			}
		}
	}
}
