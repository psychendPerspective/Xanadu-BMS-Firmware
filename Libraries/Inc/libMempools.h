/*
	Copyright 2019 - 2020 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC BMS firmware.

	The VESC BMS firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC BMS firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef LIBMEMPOOLS_H_
#define LIBMEMPOOLS_H_

#include "mainDataTypes.h"

// Settings
#define LIBMEMPOOLS_CONF_NUM				10

// Functions
main_config_t *libMempools_alloc_conf(void);
void libMempools_free_conf(main_config_t *conf);

int libMempools_conf_highest(void);
int libMempools_conf_allocated_num(void);

#endif /* LIBMEMPOOLS_H_ */
