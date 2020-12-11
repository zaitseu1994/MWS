/*
 * a111_measure.c
 *
 *  Created on: 9 дек. 2020 г.
 *      Author: User
 */
#include "a111_measure.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_hal_integration.h"
#include "acc_hal_definitions.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_envelope.h"
#include "acc_version.h"

#include "tableModbus.h"

#include "stm32l4xx_hal.h"

static const acc_hal_t *hal;
static acc_service_handle_t handle;
static acc_service_envelope_metadata_t envelope_metadata = { 0 };
static bool radarIsActive = false;


static void a111_set_settings(acc_service_configuration_t envelope_configuration,TableWrite TableWrite);


bool radar_activate_rss()
{
	hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		radarIsActive  = false;
	}
	return radarIsActive;
}

bool radar_start_with_settings(TableWrite TableWrite)
{

	acc_service_configuration_t envelope_configuration = acc_service_envelope_configuration_create();

	if (envelope_configuration == NULL)
	{
		acc_rss_deactivate();
		radarIsActive  = false;
		return radarIsActive;
	}

	a111_set_settings(envelope_configuration,TableWrite);

	if( handle!=NULL )
	{
		acc_service_deactivate(handle);
		acc_service_destroy(&handle);
		HAL_Delay(1500);
	}

	handle = acc_service_create(envelope_configuration);

	if (handle == NULL)
	{
		acc_service_envelope_configuration_destroy(&envelope_configuration);
		acc_rss_deactivate();
		radarIsActive  = false;
		return radarIsActive;
	}
	acc_service_envelope_configuration_destroy(&envelope_configuration);

	acc_service_envelope_get_metadata(handle, &envelope_metadata);


	if (!acc_service_activate(handle))
	{
		acc_service_destroy(&handle);
		acc_rss_deactivate();
		radarIsActive  = false;
		return radarIsActive;
	}

	radarIsActive  = true;
	return radarIsActive;
}

void a111_set_settings(acc_service_configuration_t envelope_configuration,TableWrite TableWrite)
{

	acc_service_requested_start_set(envelope_configuration, TableWrite->Regs.StartOfMeasure);
	acc_service_requested_length_set(envelope_configuration, TableWrite->Regs.LenghtOfMeasure);

	if ( TableWrite->Regs.RepetitionMode  )
	{
		acc_service_repetition_mode_on_demand_set(envelope_configuration);
	}else
	{
		acc_service_repetition_mode_streaming_set(envelope_configuration,2);
	}
	acc_service_power_save_mode_set(envelope_configuration,TableWrite->Regs.PowerSaveMode);
	acc_service_receiver_gain_set(envelope_configuration,TableWrite->Regs.ReceiverGain);
	acc_service_tx_disable_set(envelope_configuration,TableWrite->Regs.TXDisable);
	acc_service_hw_accelerated_average_samples_set(envelope_configuration,TableWrite->Regs.HWAAS);
	acc_service_asynchronous_measurement_set(envelope_configuration,TableWrite->Regs.AsynchMeasure);
	acc_service_profile_set(envelope_configuration,TableWrite->Regs.Profile+1);
	acc_service_maximize_signal_attenuation_set(envelope_configuration,TableWrite->Regs.MaximizeSignal);

}

uint16_t radar_get_measure()
{
	uint16_t distance = 0;

	bool                               success    = true;
	const int                          iterations = COUNT_STEP_MEASURE;
	//uint16_t                           data[envelope_metadata.data_length];
	uint16_t * data ;
	acc_service_envelope_result_info_t result_info;

	if( radarIsActive )
	{
		float summ_distance = 0;
		uint16_t size = envelope_metadata.data_length;


		for (int i = 0; i < iterations; i++)
		{
			//success = acc_service_envelope_get_next(handle, data,size, &result_info);
			success = acc_service_envelope_get_next_by_reference(handle, &data, &result_info);
			if (!success)
			{

				break;
			}

			uint16_t max = 0;
			int peak_idx = 0;
			for ( int i = 0 ; i < size ; i ++)
			{
			     if ( data [ i ] > max )
					{
					  max = data [ i ];
					  peak_idx = i ;
					}
			}
			float dist = envelope_metadata.start_m + peak_idx *envelope_metadata.step_length_m ;
			summ_distance+=dist;
		}
		distance = (uint16_t)((summ_distance/iterations)*1000);
	}
	return distance;
}
