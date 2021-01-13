#include "port.h"
#include "mb.h"
#include "mbport.h"

#include "stm32l476xx.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
UART_HandleTypeDef *uart;
static uint8_t singlechar;

/* ----------------------- User defenitions ---------------------------------*/
#define RS485_RTS_LOW	HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_RESET)
#define RS485_RTS_HIGH 	HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_SET)

#define RS485_DIR_OUT   RS485_DIR_PORT->BSRR = (uint32_t)RS485_DIR_Pin
#define RS485_DIR_IN    RS485_DIR_PORT->BRR = (uint32_t)RS485_DIR_Pin

//HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_Pin, GPIO_PIN_SET)
//HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_Pin, GPIO_PIN_RESET)
/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit( void *dHUART, ULONG ulBaudRate, void *dHTIM )
{
	uart = (UART_HandleTypeDef *)dHUART;

	return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
		//RS485_RTS_LOW;
		RS485_DIR_IN;
		HAL_UART_Receive_IT(uart, &singlechar, 1);
	}	
	else
	{
		HAL_UART_AbortReceive_IT(uart);
	}

	if(xTxEnable)
	{
		//RS485_RTS_HIGH;
		RS485_DIR_OUT;
		pxMBFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(uart);
	}
}

void vMBPortClose(void)
{
	HAL_UART_AbortReceive_IT(uart);
	HAL_UART_AbortTransmit_IT(uart);
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	//RS485_DIR_OUT;
	HAL_UART_Transmit_IT(uart, (uint8_t*)&ucByte, 1);
	//RS485_DIR_IN;
	return TRUE;
}

BOOL xMBPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	//RS485_DIR_OUT;
	HAL_UART_Transmit_IT(uart, (uint8_t *)ucByte, usSize);
	//RS485_DIR_IN;
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar);
	return TRUE;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == uart->Instance)
	{
		pxMBFrameCBByteReceived();
		HAL_UART_Receive_IT(uart, &singlechar, 1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == uart->Instance)
	{
		pxMBFrameCBTransmitterEmpty();
	}
}

#endif
