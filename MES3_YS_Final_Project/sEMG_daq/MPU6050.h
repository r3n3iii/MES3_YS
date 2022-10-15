/*
 * MP6050.h
 *
 *  Created on: Oct 14, 2022
 *      Author: Rene
 */
#ifndef  __MP6050_H_
#define  __MP6050_H_

#include<stdint.h>

void MPU6050_Reset();
void MPU6050_GetData(int16_t accelerometer[3], int16_t gyro[3], int16_t *temp);


void MPU6050_I2C_init(void);

#endif


