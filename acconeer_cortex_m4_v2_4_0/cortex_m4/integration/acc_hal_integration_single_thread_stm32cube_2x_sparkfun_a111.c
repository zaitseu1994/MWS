// Copyright (c) Acconeer AB, 2020
// All rights reserved

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

#include "acc_definitions.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"


/* spi handle */
extern SPI_HandleTypeDef A111_SPI_HANDLE;

/**
 * @brief The number of sensors available on the board
 */
#define SENSOR_COUNT 2

/**
 * @brief Size of SPI transfer buffer
 */
#ifndef A111_SPI_MAX_TRANSFER_SIZE
#define A111_SPI_MAX_TRANSFER_SIZE 65535
#endif

/**
 * @brief The reference frequency used by this board
 *
 * This assumes 26 MHz on the Sparkfun A111 Board
 */
#define ACC_BOARD_REF_FREQ 26000000


/**
 * @brief Format for the RSS logging
 *
 */

#define LOG_FORMAT "%02u:%02u:%02u.%03u (%c) (%s): %s\n"


static inline void disable_interrupts(void)
{
	__asm volatile( "cpsid i" ::: "memory" );
}


static inline void enable_interrupts(void)
{
	__asm volatile( "cpsie i" ::: "memory" );
	__asm volatile ("isb");
}


//----------------------------------------
// Implementation of RSS HAL handlers
//----------------------------------------


static void acc_hal_integration_sensor_transfer(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_size)
{
	uint32_t     sensor_cs_pin;
	GPIO_TypeDef *sensor_cs_port;

	if ((sensor_id == 0) || (sensor_id > SENSOR_COUNT))
	{
		return;
	}

	switch (sensor_id)
	{
		case 1:
			sensor_cs_pin  = A111_ID1_SPI_SS_N_Pin;
			sensor_cs_port = A111_ID1_SPI_SS_N_GPIO_Port;
			break;
		case 2:
			sensor_cs_pin  = A111_ID2_SPI_SS_N_Pin;
			sensor_cs_port = A111_ID2_SPI_SS_N_GPIO_Port;
			break;
		default:
			return;
	}

	const uint32_t SPI_TRANSMIT_RECEIVE_TIMEOUT = 5000;

	HAL_GPIO_WritePin(sensor_cs_port, sensor_cs_pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&A111_SPI_HANDLE, buffer, buffer, buffer_size, SPI_TRANSMIT_RECEIVE_TIMEOUT);
	HAL_GPIO_WritePin(sensor_cs_port, sensor_cs_pin, GPIO_PIN_SET);
}


static void acc_hal_integration_sensor_power_on(acc_sensor_id_t sensor_id)
{
	if ((sensor_id == 0) || (sensor_id > SENSOR_COUNT))
	{
		return;
	}

	switch (sensor_id)
	{
		case 1:
			HAL_GPIO_WritePin(A111_ID1_SPI_SS_N_GPIO_Port, A111_ID1_SPI_SS_N_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(A111_ID1_ENABLE_GPIO_Port, A111_ID1_ENABLE_Pin, GPIO_PIN_SET);
			break;
		case 2:
			HAL_GPIO_WritePin(A111_ID2_SPI_SS_N_GPIO_Port, A111_ID2_SPI_SS_N_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(A111_ID2_ENABLE_GPIO_Port, A111_ID2_ENABLE_Pin, GPIO_PIN_SET);
			break;
		default:
			break;
	}

	// Wait 2 ms to make sure that the sensor crystal have time to stabilize
	HAL_Delay(2);
}


static void acc_hal_integration_sensor_power_off(acc_sensor_id_t sensor_id)
{
	if ((sensor_id == 0) || (sensor_id > SENSOR_COUNT))
	{
		return;
	}

	switch (sensor_id)
	{
		case 1:
			HAL_GPIO_WritePin(A111_ID1_ENABLE_GPIO_Port, A111_ID1_ENABLE_Pin, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(A111_ID2_ENABLE_GPIO_Port, A111_ID2_ENABLE_Pin, GPIO_PIN_RESET);
			break;
		default:
			break;
	}

	// Wait after power off to leave the sensor in a known state
	// in case the application intends to enable the sensor directly
	HAL_Delay(2);
}


static bool acc_hal_integration_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms)
{
	uint32_t     sensor_interrupt_pin;
	GPIO_TypeDef *sensor_interrupt_port;

	if ((sensor_id == 0) || (sensor_id > SENSOR_COUNT))
	{
		return false;
	}

	switch (sensor_id)
	{
		case 1:
			sensor_interrupt_pin  = A111_ID1_SENSOR_INTERRUPT_Pin;
			sensor_interrupt_port = A111_ID1_SENSOR_INTERRUPT_GPIO_Port;
			break;
		case 2:
			sensor_interrupt_pin  = A111_ID2_SENSOR_INTERRUPT_Pin;
			sensor_interrupt_port = A111_ID2_SENSOR_INTERRUPT_GPIO_Port;
			break;
		default:
			return false;
	}

	const uint32_t wait_begin_ms = HAL_GetTick();
	while ((HAL_GPIO_ReadPin(sensor_interrupt_port, sensor_interrupt_pin) != GPIO_PIN_SET) &&
	       (HAL_GetTick() - wait_begin_ms < timeout_ms))
	{
		// Wait for the GPIO interrupt
		disable_interrupts();
		// Check again so that IRQ did not occur
		if (HAL_GPIO_ReadPin(sensor_interrupt_port, sensor_interrupt_pin) != GPIO_PIN_SET)
		{
			__WFI();
		}
		// Enable interrupts again to allow pending interrupt to be handled
		enable_interrupts();
	}

	return HAL_GPIO_ReadPin(sensor_interrupt_port, sensor_interrupt_pin) == GPIO_PIN_SET;
}

static float acc_hal_integration_get_reference_frequency(void)
{
	return ACC_BOARD_REF_FREQ;
}


static uint32_t acc_hal_integration_get_current_time(void)
{
	return HAL_GetTick();
}


static void acc_hal_integration_log(acc_log_level_t level, const char *module, const char *buffer)
{
	uint32_t time_ms = acc_hal_integration_get_current_time();
	char     level_ch;

	unsigned int timestamp    = time_ms;
	unsigned int hours        = timestamp / 1000 / 60 / 60;
	unsigned int minutes      = timestamp / 1000 / 60 % 60;
	unsigned int seconds      = timestamp / 1000 % 60;
	unsigned int milliseconds = timestamp % 1000;

	level_ch = (level <= ACC_LOG_LEVEL_DEBUG) ? "EWIVD"[level] : '?';

	printf(LOG_FORMAT, hours, minutes, seconds, milliseconds, level_ch, module, buffer);
}


static const acc_hal_t hal =
{
	.properties.sensor_count          = SENSOR_COUNT,
	.properties.max_spi_transfer_size = A111_SPI_MAX_TRANSFER_SIZE,

	.sensor_device.power_on                = acc_hal_integration_sensor_power_on,
	.sensor_device.power_off               = acc_hal_integration_sensor_power_off,
	.sensor_device.wait_for_interrupt      = acc_hal_integration_wait_for_sensor_interrupt,
	.sensor_device.transfer                = acc_hal_integration_sensor_transfer,
	.sensor_device.get_reference_frequency = acc_hal_integration_get_reference_frequency,

	.os.mem_alloc = malloc,
	.os.mem_free  = free,
	.os.gettime   = acc_hal_integration_get_current_time,

	.log.log_level = ACC_LOG_LEVEL_INFO,
	.log.log       = acc_hal_integration_log
};


const acc_hal_t *acc_hal_integration_get_implementation(void)
{
	return &hal;
}
