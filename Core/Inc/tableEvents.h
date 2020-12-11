/*
 * tableEvents.h
 *
 *  Created on: 28 авг. 2020 г.
 *      Author: User
 */

#ifndef TABLEEVENTS_H_
#define TABLEEVENTS_H_

#define SIZE_TABLE_CALIBRATION       300 // при изменении значения проверять вместимость таблицы в страницы флеш памяти(либо изменния механизам сохранения)

typedef enum
{
	NO_ACTION,                     /*!< нет действий*/
    MODBUS_SAVE_CONFIG,            /*!< команда сохранить пришедшие параметры настройки и тарировачную таблицу */
	MODBUS_UPDATE_CONFIG,          /*!< команда обновить дублирующие параметры настройки и тарировочную таблицу текущими*/
    MODBUS_RECEIVE_PAS,            /*!< команда  "Был получен пароль"*/
	MODBUS_UPDATE_FACTORY,         /*!< команда   сохранить параметры (serial, type,app ...)*/
	MODBUS_UPDATE_DATA_CONNECT     /*!< команда   обновить дату соединения*/
} EVENTS;

typedef enum
{
   OK,
   ERR
} WORKSTATUS;

#endif /* TABLEEVENTS_H_ */
