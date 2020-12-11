/*
 * WorkParam.h
 *
 *  Created on: 1 сент. 2020 г.
 *      Author: User
 */

#ifndef INC_WORKPARAM_H_
#define INC_WORKPARAM_H_

#include "tableEvents.h"
#include "tableModbus.h"
#include "tableCalibration.h"

#define SIZE_FLASH_CRC         1
#define MCU_SIZE_FLASH_PAGE    2048

#define FLASH_ADR_TABLE_READ   (0x0803D800)
#define FLASH_ADR_TABLE_WRITE  (FLASH_ADR_TABLE_READ+MCU_SIZE_FLASH_PAGE)
#define FLASH_ADR_TABLE_CALIBR (FLASH_ADR_TABLE_WRITE+MCU_SIZE_FLASH_PAGE)

#define FLASH_COUNT_PAGE ((FLASH_ADR_TABLE_CALIBR - FLASH_ADR_TABLE_READ) / MCU_SIZE_FLASH_PAGE)

WORKSTATUS ReadWorkParamFromFlash(uint32_t ADR,uint8_t* data,uint16_t size);
WORKSTATUS WriteWorkParamToFlash(uint32_t ADR,uint8_t* data,uint16_t size);

WORKSTATUS SetInitialTableRead(TableRead tableRead);
WORKSTATUS SetInitialTableWrite(TableWrite tableWrite);

WORKSTATUS CopyReadTable(TableRead Source,TableRead destination);
WORKSTATUS CopyWriteTable(TableWrite Source,TableWrite destination);
WORKSTATUS CopyCalibrTable(struct_workTable* Source,struct_workTable* destination,uint16_t size);

float get_volume_interpolation(float X);
void setup_end_work_table(int end);
int  get_end_work_table();
void accept_param();

#endif /* INC_WORKPARAM_H_ */
