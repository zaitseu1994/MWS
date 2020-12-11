/*
 * mbItractions.c
 *
 *  Created on: 28 авг. 2020 г.
 *      Author: User
 */
#include "stddef.h"

#include "mbItractions.h"

#include "tableModbus.h"
#include "tableCalibration.h"
#include <string.h>

struct_workTable TableDuplicate = {{{0}},0};

static int table_pozition = 0;

void MB_SEND_STATUS(TableWrite mainTable,STATUS_REG stat)
{
       mainTable->Regs.RegStatus = stat;
}

void set_end_table(int a)
{
	TableDuplicate.end_of_table = a;
}

int get_end_table()
{
	return TableDuplicate.end_of_table;
}

static void set_pozition_table()
{
  table_pozition = 0;
}

static void clear_duplicate_table()
{
  memset(&TableDuplicate,0,sizeof(TableDuplicate));
  set_pozition_table();
}

static STATUS_REG write_point_table(TableWrite mainTable)
{
	STATUS_REG stat;
	if ( table_pozition >= SIZE_TABLE_CALIBRATION )
	{
		 mainTable->Regs.RegTransfer1 = 0;
		 mainTable->Regs.RegTransfer2 = 0;
		 stat = STAT_CMD_TABLE_END;
		 set_end_table(SIZE_TABLE_CALIBRATION);
	} else
		{
		 TableDuplicate.table[table_pozition].pointDistanse = mainTable->Regs.RegTransfer1;
		 TableDuplicate.table[table_pozition].pointVolume = mainTable->Regs.RegTransfer2;
		 mainTable->Regs.RegTransfer1 = 0;
		 mainTable->Regs.RegTransfer2 = 0;
		 table_pozition++;
		 stat = STAT_CMD_TABLE_RECEIVE;
		}
	return stat;
}
static STATUS_REG send_point_table(TableWrite mainTable)
{
	STATUS_REG stat;
	if ( table_pozition >= TableDuplicate.end_of_table )
	{
		 mainTable->Regs.RegTransfer1 = 0;
		 mainTable->Regs.RegTransfer2 = 0;
		 stat = STAT_CMD_TABLE_END;
	} else
		{
		 mainTable->Regs.RegTransfer1 = TableDuplicate.table[table_pozition].pointDistanse;
		 mainTable->Regs.RegTransfer2 = TableDuplicate.table[table_pozition].pointVolume;
		 table_pozition++;
		 stat = STAT_CMD_TABLE_TRANSMIT;
		}
	return stat;
}


EVENTS MB_CHECK(TableWrite mainTable)
{
	EVENTS stat = NO_ACTION;
	if ( mainTable!=NULL )
	{
	  	switch(mainTable->Regs.RegCommand)
	  	{
	  	case modbus_no_action:
	  	{
	  		stat = NO_ACTION;
	  	}
	  	    break;
	  	case modbus_save_config:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		stat = MODBUS_SAVE_CONFIG;
	  	}
	  	    break;
	  	case modbus_update_config:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		stat = MODBUS_UPDATE_CONFIG;
	  	}
	  		break;
	  	case modbus_receive_pas:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		stat = MODBUS_RECEIVE_PAS;
	  	}
	  	    break;
	  	case modbus_cmd_table_clear:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		clear_duplicate_table();
	  		MB_SEND_STATUS(mainTable,STAT_CMD_TABLE_CLEAR);
	  	}
	  	    break;
	  	case modbus_cmd_table_receive:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		MB_SEND_STATUS(mainTable,write_point_table(mainTable));
	  	}
	  	    break;
	  	case modbus_cmd_table_pozition:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		set_pozition_table();
	  		MB_SEND_STATUS(mainTable,STAT_CMD_TABLE_POZITION);
	  	}
	  	    break;
	  	case modbus_cmd_table_transmit:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
            MB_SEND_STATUS(mainTable, send_point_table(mainTable));
	  	}
	  	    break;
	  	case modbus_cmd_table_write_end:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		set_end_table(table_pozition);
	  		MB_SEND_STATUS(mainTable,STAT_CMD_TABLE_END);
	  	}
	  	    break;
	  	case modbus_cmd_table_factory:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		stat = MODBUS_UPDATE_FACTORY;
	  	}
	  	    break;
	  	case modbus_cmd_data_connect:
	  	{
	  		MB_SEND_STATUS(mainTable,STAT_NULL);
	  		stat = MODBUS_UPDATE_DATA_CONNECT;
	  	}
	  	    break;
	  	default :
	  	{
	  		stat = NO_ACTION;
	  	}
	  		break;
	  	}
	  	mainTable->Regs.RegCommand = 0;
	}
    return stat;
}


