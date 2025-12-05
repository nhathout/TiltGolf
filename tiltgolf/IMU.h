#ifndef IMU_H
#define IMU_H

#include <stdint.h>

class IMU {
public:
    IMU();
    ~IMU();

    // Initialize the I2C connection and configure the Magnetometer
    bool begin();

    // Reads the latest data from the sensor.
    void update();

    // Getters for the raw magnetic data
    int16_t getX() const { return mx; }
    int16_t getY() const { return my; }
    int16_t getZ() const { return mz; }

private:
    int i2c_fd;
    int16_t mx, my, mz;

    // Helper functions
    bool writeReg(uint8_t reg, uint8_t value);
    bool readRegs(uint8_t start, uint8_t *data, int len);
};

#endif