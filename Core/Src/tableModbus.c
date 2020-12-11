/*
 * tableModbus.c
 *
 *  Created on: Aug 26, 2020
 *      Author: User
 */
#include "tableModbus.h"
#include <stddef.h>

TableRead table_read_create_new(uint16_t* start_adr, uint32_t size)
{
	  if(size == sizeof(union_tableRegsRead))
	  {
		  return (TableRead)start_adr;
	  } else
	  {
		  return NULL;
	  }
};

TableWrite table_write_create_new(uint16_t* start_adr, uint32_t size)
{
	  if(size == sizeof(uinon_tableRegsWrite))
	  {
		  return (TableWrite)start_adr;
	  } else
	  {
		  return NULL;
	  }
}
