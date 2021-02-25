/*
 * serialProtocols.c
 *
 *  Created on: 21 янв. 2021 г.
 *      Author: User
 */

#include "stdbool.h"
#include "serialProtocols.h"


static uint8_t llc_answ_buf[MAX_LEN_BUF_ANSWER];

bool LLC_EXECUTE = false;

static llc_commands LLCCMD = CMD_NULL;

static struct llc_data
{
	uint8_t adress;
	int8_t *temperature;
	uint16_t *level;
	uint16_t *frequency;
	uint32_t *serialnum;
	uint16_t *typedevice;
	uint32_t *volume;
	uint8_t *error;
}llc_data;

uint8_t dallas_crc_table[256] =		// dallas crc lookup table
{
	0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
	157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
	35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
	190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
	70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
	219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
	101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
	248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
	140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
	17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
	175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
	50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
	202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
	87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
	233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
	116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};


void serialProtocolsSetAdress(uint8_t adr)
{
	llc_data.adress = adr;
}

void serialProtocolsInit( TableWrite duplicate_config_write,TableRead duplicate_config_read )
{
	llc_data.volume = &duplicate_config_write->Regs.CurrentVolume; // обьем остался в том же поле
	llc_data.frequency = &duplicate_config_write->Regs.CurrentDistanse;// вместо текущей частоты вписывается текущее расстояние
	llc_data.serialnum = &duplicate_config_read->Regs.SerialNum;//серийный номер, используется 3 байта (кроме старшего)
	llc_data.typedevice = &duplicate_config_read->Regs.TypeDevice;//тип устройства, используется 1 байт(младший)
	llc_data.level = (uint16_t*)&duplicate_config_write->Regs.CurrentVolume;
}

void serialProtocolsTransmit(UCHAR ** pucFrame, USHORT * pusLength )
{
	USHORT len = 0;

	if(LLCCMD!=CMD_NULL)
	{
	  llc_answ_buf[0] = LLC_ANSWTBYTE;
	  llc_answ_buf[1] = llc_data.adress;
      switch(LLCCMD)
      {
      case CMD_06h:
    	  llc_answ_buf[2] = LLC_CMD06;
    	  llc_answ_buf[3] = 25;// температура
    	  llc_answ_buf[4] = (uint8_t)((*llc_data.level/10));
    	  llc_answ_buf[5] = (uint8_t)((*llc_data.level/10) >> 8);
    	  llc_answ_buf[6] = (uint8_t)(*llc_data.frequency);
    	  llc_answ_buf[7] = (uint8_t)(*llc_data.frequency >> 8);
//    	  llc_answ_buf[4] = 12;
//    	  llc_answ_buf[5] = 3;
//    	  llc_answ_buf[6] = 20;
//    	  llc_answ_buf[7] = 3;
    	  len = 8;
    	  llc_answ_buf[8] = serialProtocolCrcGet(llc_answ_buf,len);
    	  *pusLength = 9;
    	  break;
      case CMD_21h:
    	  llc_answ_buf[2] = LLC_CMD21;
          llc_answ_buf[3] = (uint8_t)(*llc_data.serialnum);
          llc_answ_buf[4] = (uint8_t)(*llc_data.serialnum >> 8);
          llc_answ_buf[5] = (uint8_t)(*llc_data.serialnum >> 16);
          llc_answ_buf[6] = (uint8_t)(*llc_data.typedevice);
          len = 7;
    	  llc_answ_buf[7] = serialProtocolCrcGet(llc_answ_buf,len);
    	  *pusLength = 8;
    	  break;
      case CMD_24h:
    	  llc_answ_buf[2] = LLC_CMD24;
          llc_answ_buf[3] = *llc_data.temperature;
          llc_answ_buf[4] = (uint8_t)(*llc_data.volume);
          llc_answ_buf[5] = (uint8_t)(*llc_data.volume >>8);
          llc_answ_buf[6] = (uint8_t)(*llc_data.volume >>16);
          llc_answ_buf[7] = (uint8_t)(*llc_data.volume >> 24);
          llc_answ_buf[8] = 0x00;
          len = 9;
    	  llc_answ_buf[9] = serialProtocolCrcGet(llc_answ_buf,len);
    	  *pusLength = 10;
    	  break;
      case CMD_ERROR:
      default:
    	  llc_answ_buf[2] = LLC_CRC_ERR;
          len = 3;
    	  llc_answ_buf[3] = serialProtocolCrcGet(llc_answ_buf,len);
    	  *pusLength = 4;
    	  break;
      }
	}
	LLCCMD = CMD_NULL;
	*pucFrame = (UCHAR *) &llc_answ_buf[0];
}

uint8_t dallasCRC(uint8_t variable,uint8_t CRC_current)
{
	CRC_current = dallas_crc_table[CRC_current^variable];
	return CRC_current;
}

bool serialProtocolCrcCalculate(UCHAR ** pucFrame, USHORT * pusLength)
{
#ifdef WORK
    bool stat = false;
#endif
#ifndef WORK
    bool stat = true;
#endif
	uint8_t CRC_llc=0;
	for(uint8_t number=0;number<(*pusLength)-1;number++){		//calculate CRC
		CRC_llc=dallasCRC((*pucFrame)[number],CRC_llc);
	}
	if(CRC_llc==(*pucFrame)[(*pusLength)-1])
    stat = true;
    return stat;
}

uint8_t serialProtocolCrcGet(uint8_t* buf, uint8_t len)
{
	uint8_t CRC_llc=0;
    for( int i=0;i<len;i++)
    {
    	CRC_llc = dallasCRC(buf[i],CRC_llc);
    }
    return CRC_llc;
}

bool serialProtocolsReceive( UCHAR ** pucFrame, USHORT * pusLength )
{
	bool stat = false;
	if( (*pucFrame)[0] ==  LLC_STARTBYTE )
	{
		if( (*pucFrame)[1] == llc_data.adress)
		{
			if ( serialProtocolCrcCalculate(pucFrame,pusLength))
			{
	             stat = true;
	             switch((*pucFrame)[2])
	             {
	             case LLC_CMD06:
	            	 LLCCMD = CMD_06h;
	            	 break;
	             case LLC_CMD21:
	            	 LLCCMD = CMD_21h;
	            	 break;
	             case LLC_CMD24:
	            	 LLCCMD = CMD_24h;
	            	 break;
	             default:
	            	 LLCCMD = CMD_ERROR;
	            	 stat = false;
	            	 break;
	             }
			}else
			{
				LLCCMD = CMD_ERROR;
				stat = false;
			}
		}
	}
	return stat;
}



