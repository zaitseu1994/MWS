/*
 * mbItractions.h
 *
 *  Created on: 28 авг. 2020 г.
 *      Author: Mikhail Zaitseu
 */

#ifndef INC_MBITRACTIONS_H_
#define INC_MBITRACTIONS_H_

#define	modbus_no_action           0         /*!< нет действий*/
#define modbus_save_config         1         /*!< команда сохранить пришедшие параметры настройки и тарировачную таблицу */
#define	modbus_update_config       2         /*!< команда обновить дублирующие параметры настройки и тарировочную таблицу текущими*/
#define modbus_receive_pas         3         /*!< команда  "Был получен пароль"*/
#define modbus_cmd_table_clear     4         /*!< команда   очистки дублирующей таблицы и постановки позиции текущей точки тарированной таблицы в 0*/
#define modbus_cmd_table_receive   5         /*!< команда   получения под запись точки тарировочной таблицы*/
#define modbus_cmd_table_pozition  6         /*!< команда   постановки позиции текущей точки тарированной таблицы в 0*/
#define modbus_cmd_table_transmit  7         /*!< команда   запроса сохраненой точки тарировочной таблицы*/
#define modbus_cmd_table_write_end 8         /*!< команда   окончания записи тарировочной таблицы*/
#define modbus_cmd_table_factory   9         /*!< команда   сохранить параметры (serial, type,app ...)*/
#define modbus_cmd_data_connect    10        /*!< команда   принять время соединения*/
//===================================================================================

#include "tableModbus.h"
#include "tableEvents.h"

//===================================================================================

typedef enum
{
  STAT_NULL,
  STAT_CONFIG_SAVE = 1,                      /*!< подтверждение сохранения пришедших параметров настройки и тарировочной таблицы*/
  STAT_CONFIG_UPDATE,                        /*!< подтверждение обновления дублирующих параметров настройки и тарировочной таблицы*/
  STAT_PAS_RECEIVE,                          /*!< подтверждение принятия пароля*/
  STAT_CMD_TABLE_CLEAR,                      /*!< подтверждение очистки дублирующей тарировочной таблицы и установки позиции текущей точки тарировочной таблицы в 0*/
  STAT_CMD_TABLE_RECEIVE,                    /*!< подтверждение записи точки тарировочной таблицы */
  STAT_CMD_TABLE_POZITION,                   /*!< подтверждение установки позиции текущей точки тарировочной таблицы в 0*/
  STAT_CMD_TABLE_TRANSMIT,                   /*!< подтверждение выставления в регистры передачи текущей точки тарировочной таблицы*/
  STAT_CMD_TABLE_END,                        /*!< окончание тарировочной таблицы*/
  STAT_CMD_SAVE_FACTORY,                     /*!< подтверждение сохранения параметров (serial, type,app ...)*/
  STAT_CMD_DATA_CONNECT                      /*!< подтверждение  принятия время соединения*/
} STATUS_REG;

//===================================================================================

EVENTS MB_CHECK(TableWrite mainTable);
void MB_SEND_STATUS(TableWrite mainTable,STATUS_REG stat);
void set_end_table(int a);
int get_end_table();


#endif /* INC_MBITRACTIONS_H_ */
