/*
 * WorkParam.c
 *
 *  Created on: 1 сент. 2020 г.
 *      Author: User
 */
#include "main.h"

#include <stddef.h>
#include <string.h>

#include "workParam.h"
#include "tableEvents.h"
#include "tableCalibration.h"

#include <stdlib.h>
#include "math.h"

#define LINEAR_TYPE_APPROXIMATION 0
#define LAGRANJ_TYPE_APPROXIMATION 1

#define TYPE_AVERAGE_SUM 1
#define TYPE_AVERAGE_EXP 0

#define FILTER_COEF 0.7
#define MAX_NUM_SUM 40

//===================================================================================
/**
  sTable - таблица для расчета crc.
*/
static const unsigned char sTable[256] =
{
0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

//===================================================================================

union_tableRegsRead un_tableRead = {0};
uinon_tableRegsWrite un_tableWrite = {0};
TableRead work_config_read = &un_tableRead;
TableWrite work_config_write = &un_tableWrite;

struct_workTable TableWorkParam = {{{0}},0};

float (*volume_approximation)(float X) = NULL;
uint16_t (*filtred_val)(uint16_t X) = NULL;

static uint16_t filtred_distanse = 0;
volatile float Coef_EXP = 0.9;
//===================================================================================

uint32_t maPrime2dHash (unsigned char *str, uint32_t len)
{
  unsigned int hash = 0, i;
  unsigned int rotate = 2;
  unsigned int seed = 0x1A4E41U;
  for (i = 0; i != len; i++, str++)
  {
	   hash += sTable[(*str + i) & 255];
	   hash = ( hash << (32 - rotate) ) | (hash >> rotate);
	   hash = ( hash + i ) * seed;
  }
  return (hash + len) * seed;
}

//===================================================================================

float Lagranj (float X)
{
    int n  = TableWorkParam.end_of_table;
    float L, l;
    int i, j;
    L = 0;
    if( (X < TableWorkParam.table[n-1].pointDistanse)  &&
 	    (X > TableWorkParam.table[0].pointDistanse)
    )
    for (i = 0; i < n; ++i)
    {
        l = 1;

        for (j = 0; j < n; ++j)
            if (i != j)
                l *= (X - TableWorkParam.table[j].pointDistanse) /
				(TableWorkParam.table[i].pointDistanse - TableWorkParam.table[j].pointDistanse);
        L += TableWorkParam.table[i].pointVolume * l;
    }
    return L;
}

//===================================================================================

float Linear (float X)
{
   float x0 = 0,x1 = 0,y0 = 0,y1 = 0;
   float Fx = 0;
   if( (X < TableWorkParam.table[TableWorkParam.end_of_table-1].pointDistanse)  &&
	   (X > TableWorkParam.table[0].pointDistanse)
   )
   if(TableWorkParam.end_of_table >=2)
   {
      for(int i=TableWorkParam.end_of_table-1;i>=1;i--)
      {
    	  float summA = abs(X-(float)TableWorkParam.table[i].pointDistanse);
    	  float summB = abs(X - (float)TableWorkParam.table[i-1].pointDistanse);
		  float mainDis = abs((float)TableWorkParam.table[i-1].pointDistanse - (float)TableWorkParam.table[i].pointDistanse);
          if ( mainDis == summA + summB )
          {
                 x0 =  (float)TableWorkParam.table[i-1].pointDistanse;
                 y0 =  (float)TableWorkParam.table[i-1].pointVolume;
                 x1 =  (float)TableWorkParam.table[i].pointDistanse;
                 y1 =  (float)TableWorkParam.table[i].pointVolume;
              break;
          }
      }
      if(x0 && y0 && x1 && y1)
      {
         Fx = y0+(y1-y0)/(x1-x0)*(X-x0);
      }
   }
   return Fx;
}

//===================================================================================

uint16_t runing_averageSUM(uint16_t newVal) {
  static int t = 0;
  static int vals[MAX_NUM_SUM];
  static int average = 0;
  uint8_t num = (un_tableWrite.Regs.IntervalAverag*2);
  if(num == 0)
  num = 1;
  if (++t >= num) t = 0; // перемотка t
  average -= vals[t];         // вычитаем старое
  average += newVal;          // прибавляем новое
  vals[t] = newVal;           // запоминаем в массив
  return (uint16_t)((float)average / num);
}

//===================================================================================

uint16_t runing_averageEXP(uint16_t X)
{
  if( ( ((filtred_distanse - X) < 0) && (X/filtred_distanse > 1.5))
	 ||
	 ( ((filtred_distanse - X) > 0) && (filtred_distanse/X > 1.5) )
  ) // значение расстояния стало резко больше или меньше чем в 1,5 раза
  {
	 filtred_distanse = X * 0.9 + filtred_distanse * (0.1);
  }else
  {
	filtred_distanse = X * Coef_EXP + filtred_distanse * (1 - Coef_EXP);
  }
  return filtred_distanse;
}

uint16_t get_filtred_distanse(uint16_t X)
{
	if( filtred_val )
	return filtred_val(X);
	else
	return 0;
}

float get_volume_interpolation(float X)
{
	if ( volume_approximation )
	return volume_approximation(X);
	else
	return 0;
}

//===================================================================================

void accept_param()
{
	 if ( un_tableWrite.Regs.TypeApproxim == LINEAR_TYPE_APPROXIMATION )
	 volume_approximation = Linear;
	 else
     volume_approximation  = Lagranj;

	 if(  un_tableWrite.Regs.TypeAverag == TYPE_AVERAGE_SUM )
	 {
		 filtred_val = runing_averageSUM;
	 }
	 else
		 filtred_val = runing_averageEXP;
	 filtred_distanse = 0;

	 un_tableRead.Regs.timechange = un_tableWrite.Regs.timechange;
	 un_tableRead.Regs.idchange = un_tableWrite.Regs.idchange;
	 un_tableRead.Regs.idset = un_tableWrite.Regs.idset;

	 if( un_tableWrite.Regs.IntervalAverag!=0 )
	 Coef_EXP = ((float)1/un_tableWrite.Regs.IntervalAverag);

	 HAL_GPIO_WritePin(CAN_RES_Port, CAN_RES_Pin, un_tableWrite.Regs.Reslift & 0x0f00 ? GPIO_PIN_SET:GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(CAN_POW_Port, CAN_POW_Pin, un_tableWrite.Regs.Reslift & 0xf000 ? GPIO_PIN_SET:GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(RS485_RES_Port, RS485_RES_Pin, un_tableWrite.Regs.Reslift & 0x000f ? GPIO_PIN_SET:GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(RS485_POW_Port, RS485_POW_Pin, un_tableWrite.Regs.Reslift & 0x00f0 ? GPIO_PIN_SET:GPIO_PIN_RESET);

}

//===================================================================================

void setup_end_work_table(int end)
{
	TableWorkParam.end_of_table = end;
}

//===================================================================================

int get_end_work_table()
{
	return TableWorkParam.end_of_table;
}

//===================================================================================

static void FLASH_initialize()
 {
  HAL_FLASH_Unlock(); // разблокировать флеш
  uint32_t error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);
  error |= (FLASH->ECCR & FLASH_FLAG_ECCD);
  if ( error != 0u)
  {
	__HAL_FLASH_CLEAR_FLAG(error);
  }
  HAL_FLASH_Lock(); // заблокировать флеш
 }

uint8_t Flash_Ready(void)
{
	   return !(FLASH->SR & FLASH_SR_BSY);
}


uint8_t Flash_ErasePage(uint32_t address,uint8_t numb)
{
  uint8_t page = (int)(address - 0x08000000)/MCU_SIZE_FLASH_PAGE;
  uint32_t page_error = 0;
  static FLASH_EraseInitTypeDef EraseInitStruct;     // структура для очистки флеша
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // постраничная очистка, FLASH_TYPEERASE_MASSERASE - очистка всего флеша
  EraseInitStruct.Page = page - 64;                  // адрес страницы
  EraseInitStruct.NbPages = numb;                    // кол-во страниц для стирания
  EraseInitStruct.Banks = FLASH_BANK_2;              // FLASH_BANK_1 - банк №2, FLASH_BANK_BOTH - оба банка
  FLASH_initialize();
  HAL_FLASH_Unlock();                                // разблокировать флеш
  if ( HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK )
  {
	   return 0;
  }
  while ( !Flash_Ready() )
  {

  }
  HAL_FLASH_Lock(); // заблокировать флеш
  return 1;
}

uint8_t Flash_WritePage(uint32_t address, uint8_t* data, int size)
{
  int i=0;
  uint64_t* word=(uint64_t*)data;
  HAL_FLASH_Unlock(); // разблокировать флеш
  for(i=0;i<(int)(size+8-1)/8;i++) // записываем двойными словами количество size в байтах
  {
	if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *word ) != HAL_OK )
	{
		 HAL_FLASH_GetError();
		 return 0;
	}
	word++;
	address+=8;
}
while ( !Flash_Ready() )
{

}
 HAL_FLASH_Lock(); // заблокировать флеш
 return 1;
}

uint8_t Flash_ReadBuf(uint32_t address, uint8_t* data, int size)
{
  int poz=0;
  uint8_t* uf;
  volatile  uint32_t val = 0;
  while (poz <= size -1 )
   {
   val = (*(uint32_t*) address);
   uf = (uint8_t*)&val;

   for ( int i=0;i<4;i++ )
   {
	if ( poz <= size-1 )
	{
		 data[poz] = uf[i];
		 poz++;
	} else {
	  break;
	}
   }
   address+=4;
  }
  return 1;
}

//===================================================================================

WORKSTATUS ReadWorkParamFromFlash(uint32_t ADR,uint8_t* data,uint16_t size)
{
	uint32_t crc = 0;
	Flash_ReadBuf(ADR,data,size);
	Flash_ReadBuf(ADR+size,(uint8_t*)&crc,4);
	if(crc!=maPrime2dHash(data,size))
	return ERR;
	else
	return OK;
}

//===================================================================================

WORKSTATUS WriteWorkParamToFlash(uint32_t ADR,uint8_t* data,uint16_t size)
{
	uint32_t crc = maPrime2dHash(data,size);
	Flash_ErasePage(ADR,(int)((size+MCU_SIZE_FLASH_PAGE-1)/MCU_SIZE_FLASH_PAGE));
	uint8_t datacrc[size+4];
	memcpy(datacrc,data,size);
	memcpy(&datacrc[size],(uint8_t*)&crc,4);
	if (!Flash_WritePage(ADR,datacrc,size+4))
	return ERR;
	else
	return OK;
}

//===================================================================================

WORKSTATUS SetInitialTableRead(TableRead tableRead)
{
	tableRead->Regs.timeconnect = 0;
	tableRead->Regs.timechange = 0;
	tableRead->Regs.idchange = 0;
	tableRead->Regs.timedefault = 0;
	tableRead->Regs.iddefault = 0;
	tableRead->Regs.LogError = 1;
	tableRead->Regs.SerialNum = 10000;
	tableRead->Regs.TypeDevice = 4;
	tableRead->Regs.VerApp = 2;
	return OK;
}

//===================================================================================

WORKSTATUS SetInitialTableWrite(TableWrite tableWrite)
{
	tableWrite->Regs.AsynchMeasure = 0;
	tableWrite->Regs.DownSampFactor = 1;
	tableWrite->Regs.HWAAS = 20;
	tableWrite->Regs.LenghtOfMeasure = 1.2;
	tableWrite->Regs.MaximizeSignal = 0;
	tableWrite->Regs.NoiseLevel = 1;
	tableWrite->Regs.PowerSaveMode = 3;
	tableWrite->Regs.Profile = 1;//  0 это профиль 1
	tableWrite->Regs.ReceiverGain = 0.7;
	tableWrite->Regs.RepetitionMode = 1;
	tableWrite->Regs.RunningAverage = 0.7;
	tableWrite->Regs.SensorId = 1;
	tableWrite->Regs.StartOfMeasure = 0.2;
	tableWrite->Regs.TXDisable = 0;
	tableWrite->Regs.TypeApproxim = 1;
	tableWrite->Regs.TypeAverag = 1;

	tableWrite->Regs.CurrentDistanse = 0;
	tableWrite->Regs.CurrentVolume = 0;
	tableWrite->Regs.AdrModbus = 10;
	tableWrite->Regs.IntervalAverag  = 5;
	tableWrite->Regs.Password = 0;
    tableWrite->Regs.Reslift = 0;

	tableWrite->Regs.RegCommand = 0;
	tableWrite->Regs.RegStatus = 0;
	tableWrite->Regs.RegTransfer1 = 0;
	tableWrite->Regs.RegTransfer2 = 0;

	tableWrite->Regs.timeconnect = 0;
	tableWrite->Regs.timechange = 0;
	tableWrite->Regs.idchange = 0;
	tableWrite->Regs.timedefault = 0;
	tableWrite->Regs.iddefault = 0;
	tableWrite->Regs.SerialNum = 10000;
	tableWrite->Regs.TypeDevice = 4;
	tableWrite->Regs.VerApp = 2;
	return OK;
}

//===================================================================================

WORKSTATUS CopyReadTable(TableRead Source,TableRead destination)
{
  WORKSTATUS st = ERR;
  if ( Source!=NULL && destination!=NULL )
  {
       memcpy((uint8_t*)destination,(uint8_t*)Source,sizeof(union_tableRegsRead));
       st = OK;
  }
  return st;
}

//===================================================================================

WORKSTATUS CopyWriteTable(TableWrite Source,TableWrite destination)
{
	  WORKSTATUS st = ERR;
	  if ( Source!=NULL && destination!=NULL )
	  {
	       memcpy((uint8_t*)destination,(uint8_t*)Source,sizeof(uinon_tableRegsWrite));
	       st = OK;
	  }
	  return st;
}

//===================================================================================

WORKSTATUS CopyCalibrTable(struct_workTable* Source,struct_workTable* destination,uint16_t size)
{
	  WORKSTATUS st = ERR;
	  if ( Source!=NULL && destination!=NULL )
	  {
	       memcpy((uint8_t*)destination,(uint8_t*)Source,size);
	       st = OK;
	  }
	  return st;
}
