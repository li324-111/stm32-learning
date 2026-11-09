#include "stm32f10x.h"

void Serial_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /*
     * 打开GPIOA和USART1时钟
     * PA9、PA10属于GPIOA，USART1负责串口收发
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

    /*
     * PA9 = USART1_TX
     * 复用推挽输出：由USART1控制引脚电平
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*
     * PA10 = USART1_RX
     * 上拉输入：接收USB-TTL发来的电平
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*
     * 配置USART1通信格式：9600、8位、无校验、1停止位
     */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    /* 把配置表写入USART1寄存器 */
    USART_Init(USART1, &USART_InitStructure);

    /* 启动USART1 */
    USART_Cmd(USART1, ENABLE);
}

	void Serial_SendByte(uint8_t Byte)
	{
		/* 把一个字节写入USART1发送数据寄存器 */
		USART_SendData(USART1, Byte);

		/*
		 * 等待发送数据寄存器变空。
		 * TXE = Transmit data register Empty
		 */
		while (
			USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET
		);
	}
	void Serial_SendString(char *String)
{
    uint16_t i;

    for (i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte(String[i]);
    }
}
	uint8_t Serial_ReceiveByte(void)
{
    while (
        USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET
    );

    return USART_ReceiveData(USART1);
}
