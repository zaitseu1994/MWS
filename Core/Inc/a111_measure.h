/*
 * a111_measure.h
 *
 *  Created on: 9 дек. 2020 г.
 *      Author: User
 */

#ifndef INC_A111_MEASURE_H_
#define INC_A111_MEASURE_H_

#include <stdbool.h>
#include "tableModbus.h"

#define COUNT_STEP_MEASURE 1

bool radar_activate_rss();
bool radar_start_with_settings(TableWrite TableWrite);
uint16_t radar_get_measure();

#endif /* INC_A111_MEASURE_H_ */
