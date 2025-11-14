// IMU.h
#ifndef IMU_H
#define IMU_H

#include <stdint.h>

bool initAccelerometer(int &fd);
bool initMagnetometer(int &fd);
bool readAccelerometer(int fd, int16_t &ax, int16_t &ay, int16_t &az);
bool readMagnetometer(int fd, int16_t &mx, int16_t &my, int16_t &mz);

#endif
