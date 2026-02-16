#include "IMU.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>

// I2C Configuration
#define I2C_DEVICE "/dev/i2c-2"
#define MAG_ADDR   0x1E

// Magnetometer Registers
#define CRA_REG_M  0x00 // Config A (Rate)
#define CRB_REG_M  0x01 // Config B (Gain)
#define MR_REG_M   0x02 // Mode
#define OUT_X_H_M  0x03 // Data Start

IMU::IMU() : i2c_fd(-1), mx(0), my(0), mz(0),
             bias_x(0), bias_y(0), bias_z(0),
             temp_bias_x(0), temp_bias_y(0), temp_bias_z(0) {}

IMU::~IMU() {
    if (i2c_fd >= 0) {
        close(i2c_fd);
    }
}

bool IMU::begin() {
    // Open I2C device
    i2c_fd = open(I2C_DEVICE, O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "IMU Error: Failed to open " << I2C_DEVICE << std::endl;
        return false;
    }

    // Select Magnetometer Address
    if (ioctl(i2c_fd, I2C_SLAVE, MAG_ADDR) < 0) {
        std::cerr << "IMU Error: Failed to acquire bus access/talk to slave" << std::endl;
        close(i2c_fd);
        i2c_fd = -1;
        return false;
    }

    // Configure Registers
    if (!writeReg(CRA_REG_M, 0x10))
        return false; // data rate
    if (!writeReg(CRB_REG_M, 0x20))
        return false; // gain
    if (!writeReg(MR_REG_M, 0x00))
        return false; // continuous mode

    // start with zero biases (in-memory only)
    bias_x = bias_y = bias_z = 0;
    temp_bias_x = temp_bias_y = temp_bias_z = 0;

    return true;
}

void IMU::update() {
    if (i2c_fd < 0) return;

    // Ensure we are talking to the Magnetometer 
    ioctl(i2c_fd, I2C_SLAVE, MAG_ADDR);

    uint8_t data[6];

    // Read 6 bytes starting from OUT_X_H_M
    if (readRegs(OUT_X_H_M, data, 6)) {
        // Registers are ordered X, Z, Y in the memory map for many HMC sensors
        // Data is Big Endian (High byte, Low byte)
        mx = (int16_t)((data[0] << 8) | data[1]); // X
        mz = (int16_t)((data[2] << 8) | data[3]); // Z
        my = (int16_t)((data[4] << 8) | data[5]); // Y
    }
}

int16_t IMU::getX() const
{
    int32_t val = static_cast<int32_t>(mx) - bias_x - temp_bias_x;
    return static_cast<int16_t>(val);
}

int16_t IMU::getY() const
{
    int32_t val = static_cast<int32_t>(my) - bias_y - temp_bias_y;
    return static_cast<int16_t>(val);
}

int16_t IMU::getZ() const
{
    int32_t val = static_cast<int32_t>(mz) - bias_z - temp_bias_z;
    return static_cast<int16_t>(val);
}

bool IMU::calibrateNow()
{
    // Read one fresh sample and treat it as the saved bias (in-memory only)
    update();

    bias_x = static_cast<int32_t>(mx);
    bias_y = static_cast<int32_t>(my);
    bias_z = static_cast<int32_t>(mz);

    std::cout << "IMU: Calibrated (in-memory) bias set to current reading: "
              << bias_x << ", " << bias_y << ", " << bias_z << std::endl;

    // Clear any temp bias (we made the saved bias equal to current raw)
    temp_bias_x = temp_bias_y = temp_bias_z = 0;

    return true;
}

void IMU::setTempBiasFromCurrentReading()
{
    // Capture a fresh sample and use it as temporary bias for preview
    update();
    temp_bias_x = static_cast<int32_t>(mx);
    temp_bias_y = static_cast<int32_t>(my);
    temp_bias_z = static_cast<int32_t>(mz);

    std::cout << "IMU: Temp bias set for preview: "
              << temp_bias_x << ", " << temp_bias_y << ", " << temp_bias_z << std::endl;
}

void IMU::clearTempBias()
{
    temp_bias_x = temp_bias_y = temp_bias_z = 0;
    std::cout << "IMU: Temp bias cleared." << std::endl;
}

bool IMU::commitTempBiasToSaved()
{
    // Make the temp bias permanent by adding it to the saved bias, then clear temp.
    bias_x += temp_bias_x;
    bias_y += temp_bias_y;
    bias_z += temp_bias_z;

    temp_bias_x = temp_bias_y = temp_bias_z = 0;

    std::cout << "IMU: Committed temp bias to saved bias: "
              << bias_x << ", " << bias_y << ", " << bias_z << std::endl;
    return true;
}

bool IMU::writeReg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return write(i2c_fd, buf, 2) == 2;
}

bool IMU::readRegs(uint8_t start, uint8_t *data, int len) {
    // Write register address we want to start reading from
    if (write(i2c_fd, &start, 1) != 1) return false;
    // Read the data back
    return read(i2c_fd, data, len) == len;
}