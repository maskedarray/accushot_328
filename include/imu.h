#ifndef __IMU_H__
#define __IMU_H__
#include <Arduino.h>
#include <MPU6050.h>
#include <Wire.h>

extern MPU6050 mpu;

bool mpu_init();

#endif