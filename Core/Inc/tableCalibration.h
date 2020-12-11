/*
 * tableCalibratiion.h
 *
 *  Created on: 28 авг. 2020 г.
 *      Author: Mikhail Zaitseu
 */

#ifndef INC_TABLECALIBRATION_H_
#define INC_TABLECALIBRATION_H_

#include <stdint.h>
#include "tableEvents.h"

#pragma pack(push, 1)
typedef struct struct_pointTableCalibration
{
               uint16_t pointDistanse;
               float pointVolume;
}struct_pointTableCalibration;
#pragma pack(pop)

typedef struct_pointTableCalibration TableCalibration;

typedef struct struct_workTable
{
	TableCalibration table[SIZE_TABLE_CALIBRATION];
	int end_of_table;
}struct_workTable;

void sorting_table_calibration(TableCalibration* table,uint16_t sizeTable);

#endif /* INC_TABLECALIBRATION_H_ */
