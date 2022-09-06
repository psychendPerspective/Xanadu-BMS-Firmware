/*
	Copyright 2017 - 2018 Danny Bokma	  danny@diebie.nl
	Copyright 2019 - 2020 Kevin Dionne	kevin.dionne@ennoid.me
  	Copyright 2022        Vishal Bhat   vishal.bhat09@gmail.com

	This file is part of the Xanadu BMS firmware.

	The Xanadu BMS firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Xanadu BMS firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef EKF_H
#define EKF_H

#include <math.h>

typedef struct {

	float phi_r;
	float theta_r;

	float P[2][2];

	float Q[2];
	float R[3];

} EKF;

void EKF_Init(EKF *ekf, float P[2], float Q[2], float R[3]);

void EKF_Predict(EKF *ekf, float p_rps, float q_rps, float r_rps, float sampleTime_s);

void EKF_Update(EKF *ekf, float ax_mps2, float ay_mps2, float az_mps2);

#endif