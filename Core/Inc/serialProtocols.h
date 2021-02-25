/*
 * serialProtocols.h
 *
 *  Created on: 21 янв. 2021 г.
 *      Author: User
 */

#ifndef INC_SERIALPROTOCOLS_H_
#define INC_SERIALPROTOCOLS_H_

#define MAX_LEN_BUF_ANSWER 15

#define LLC_STARTBYTE 0x31
#define LLC_ANSWTBYTE 0x3E

#define LLC_CMD06 0x06
#define LLC_CMD21 0x21
#define LLC_CMD24 0x24

#define LLC_CRC_ERR 0xFF

#include "port.h"
#include "tableModbus.h"
#include "main.h"

typedef enum llc_commands
{
	CMD_NULL,
	CMD_ERROR,
	CMD_06h,
	CMD_21h,
	CMD_24h
}llc_commands;

void serialProtocolsSetAdress(uint8_t adr);
void serialProtocolsInit( TableWrite duplicate_config_write,TableRead duplicate_config_read );

bool serialProtocolsReceive( UCHAR ** pucFrame, USHORT * pusLength );
void serialProtocolsTransmit(UCHAR ** pucFrame, USHORT * pusLength );

uint8_t serialProtocolCrcGet(uint8_t* buf, uint8_t len);

#endif /* INC_SERIALPROTOCOLS_H_ */
