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

IMU::IMU() : i2c_fd(-1), mx(0), my(0), mz(0) {}

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
        return false;
    }

    // Configure Registers
    
    // CRA_REG_M: Data Rate
    if (!writeReg(CRA_REG_M, 0x10)) return false; 

    // CRB_REG_M: Gain
    if (!writeReg(CRB_REG_M, 0x20)) return false; 

    // MR_REG_M: Mode
    if (!writeReg(MR_REG_M, 0x00)) return false;

    return true;
}

void IMU::update() {
    if (i2c_fd < 0) return;

    // Ensure we are talking to the Magnetometer 
    ioctl(i2c_fd, I2C_SLAVE, MAG_ADDR);

    uint8_t data[6];
    
    // Read 6 bytes starting from OUT_X_H_M
    if (readRegs(OUT_X_H_M, data, 6)) {
        // Registers are ordered X, Z, Y in the memory map
        // Data is Big Endian (High byte, Low byte)
        
        mx = (int16_t)((data[0] << 8) | data[1]); // X
        mz = (int16_t)((data[2] << 8) | data[3]); // Z
        my = (int16_t)((data[4] << 8) | data[5]); // Y
    }
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