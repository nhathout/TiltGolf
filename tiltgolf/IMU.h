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

    // Getters for the calibrated magnetic data (raw - saved_bias - temp_bias)
    int16_t getX() const;
    int16_t getY() const;
    int16_t getZ() const;

    // Simple immediate calibration (in-memory only)
    // Set the current raw reading as the saved bias so that current pose becomes "zero".
    bool calibrateNow();

    // Temporary-bias (preview) API: set a temp bias from the current reading,
    // clear it, or commit the temp bias into the saved bias.
    void setTempBiasFromCurrentReading(); // capture temp bias = current raw reading
    void clearTempBias();                 // discard temp bias
    bool commitTempBiasToSaved();         // make temp bias permanent (saved_bias += temp), clear temp

private:
    int i2c_fd;
    int16_t mx, my, mz;

    // Per-axis hard-iron saved bias (in-memory only)
    int32_t bias_x;
    int32_t bias_y;
    int32_t bias_z;

    // Temporary bias used during preview (not persisted)
    int32_t temp_bias_x;
    int32_t temp_bias_y;
    int32_t temp_bias_z;

    // Helper functions
    bool writeReg(uint8_t reg, uint8_t value);
    bool readRegs(uint8_t start, uint8_t *data, int len);
};

#endif