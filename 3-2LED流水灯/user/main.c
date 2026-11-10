#include "stm32f10x.h"
#include "Timer.h"
#include "Serial.h"
#include <string.h>

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

	/* 外接流水灯任务的数据 */
	uint16_t LED_Number = 0;
	uint32_t LED_LastTime = 0;
	
	/*串口数据*/
	uint32_t Serial_LastTime = 0;
	char CommandBuffer[32];
	uint8_t CommandIndex = 0;
	uint8_t CommandReady = 0;
	uint32_t CommandLastTime = 0;


	while (1)
	{
//		/* 任务1：每100ms移动一次外接流水灯 */
//		if (Timer2_Count - LED_LastTime >= 100)
//		{
//			LED_LastTime = Timer2_Count;

//			GPIO_Write(GPIOA, ~(0x0001 << LED_Number));

//			LED_Number++;

//			if (LED_Number >= 8)
//			{
//				LED_Number = 0;
//			}
//		}

	
//		/* 任务2：每1000ms发送一次字符串 */
//		if (Timer2_Count - Serial_LastTime >= 1000)
//		{
//			Serial_LastTime = Timer2_Count;

//			Serial_SendString("Hello STM32!\r\n");
//		}
//		/*任务3：接收一字节*/
//		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
//		{
//			uint8_t ReceiveData;

//			ReceiveData = USART_ReceiveData(USART1);
//			
//		if (ReceiveData == '1')
//		{
//			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//			Serial_SendString("LED ON\r\n");
//			Serial_SendByte(ReceiveData);
//			Serial_SendString("\r\n");
//		}			
//		
//		else if (ReceiveData == '0')
//		{
//			GPIO_SetBits(GPIOC, GPIO_Pin_13);
//			Serial_SendString("LED OFF\r\n");
//			
//			Serial_SendByte(ReceiveData);
//			Serial_SendString("\r\n");
//		}
//		else if (ReceiveData == 's')
//		{
//			Serial_SendString("PC13 state command received\r\n");
//		}
//		else if (ReceiveData == 'h')
//		{
//			Serial_SendString("1: LED ON\r\n");
//			Serial_SendString("0: LED OFF\r\n");
//			Serial_SendString("s: status\r\n");
//			Serial_SendString("h: help\r\n");
//		}
//		}
		
			/* 任务4：接收一整句话 */
			if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
			{
				uint8_t ReceiveData;

				ReceiveData = USART_ReceiveData(USART1);

				/* 回显普通字符 */
				if (ReceiveData != '\r' && ReceiveData != '\n')
				{
					Serial_SendByte(ReceiveData);
				}

				if (ReceiveData == '\r' || ReceiveData == '\n')
				{
					if (CommandIndex > 0)
					{
						CommandBuffer[CommandIndex] = '\0';
						CommandReady = 1;
					}
				}
				else
				{
					if (CommandIndex < 31)
					{
						CommandBuffer[CommandIndex] = ReceiveData;
						CommandIndex++;

						/* 记录最后一个字符到达的时间 */
						CommandLastTime = Timer2_Count;
					}
					else
					{
						CommandIndex = 0;
						Serial_SendString("\r\nCommand too long\r\n");
					}
				}
			}
			/* 如果50ms没有收到新字符，认为命令已经结束 */
			if (CommandIndex > 0 &&
				CommandReady == 0 &&
				Timer2_Count - CommandLastTime >= 50)
			{
				CommandBuffer[CommandIndex] = '\0';
				CommandReady = 1;
			}
			/* 任务5：处理已经接收完整的命令 */
			if (CommandReady == 1)
			{
				if (strcmp(CommandBuffer, "led on") == 0)
			{
				/* PC13输出低电平，板载LED亮 */
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				Serial_SendString("LED ON\r\n");
			}
			else if (strcmp(CommandBuffer, "led off") == 0)
			{
				/* PC13输出高电平，板载LED灭 */
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				Serial_SendString("LED OFF\r\n");
			}
				else if (strcmp(CommandBuffer, "help") == 0)
				{
					Serial_SendString("led on\r\n");
					Serial_SendString("led off\r\n");
					Serial_SendString("help\r\n");
				}
				else
				{
					Serial_SendString("Unknown command\r\n");
				}

				CommandReady = 0;
				CommandIndex = 0;
			}
	}
}
