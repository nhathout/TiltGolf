#include "IMU.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>

#define ACC_ADDR 0x19
#define MAG_ADDR 0x1E

#define CTRL_REG1_A 0x20
#define OUT_X_L_A   0x28
#define CRA_REG_M   0x00
#define CRB_REG_M   0x01
#define MR_REG_M    0x02
#define OUT_X_H_M   0x03

static bool writeReg(int fd, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return write(fd, buf, 2) == 2;
}

static bool readRegs(int fd, uint8_t start, uint8_t *data, int len) {
    if (write(fd, &start, 1) != 1) return false;
    return read(fd, data, len) == len;
}

bool initAccelerometer(int &fd) {
    fd = open("/dev/i2c-2", O_RDWR);
    if (fd < 0) return false;
    if (ioctl(fd, I2C_SLAVE, ACC_ADDR) < 0) return false;
    return writeReg(fd, CTRL_REG1_A, 0x27); // enable, 50Hz, all axes
}

bool initMagnetometer(int &fd) {
    if (ioctl(fd, I2C_SLAVE, MAG_ADDR) < 0) return false;
    if (!writeReg(fd, CRA_REG_M, 0x10)) return false; // 15 Hz
    if (!writeReg(fd, CRB_REG_M, 0x20)) return false; // gain
    return writeReg(fd, MR_REG_M, 0x00);             // continuous
}

bool readAccelerometer(int fd, int16_t &ax, int16_t &ay, int16_t &az) {
    uint8_t data[6];
    if (ioctl(fd, I2C_SLAVE, ACC_ADDR) < 0) return false;
    if (!readRegs(fd, OUT_X_L_A | 0x80, data, 6)) return false; // auto-increment
    ax = (int16_t)((data[1] << 8) | data[0]);
    ay = (int16_t)((data[3] << 8) | data[2]);
    az = (int16_t)((data[5] << 8) | data[4]);
    return true;
}

bool readMagnetometer(int fd, int16_t &mx, int16_t &my, int16_t &mz) {
    uint8_t data[6];
    if (ioctl(fd, I2C_SLAVE, MAG_ADDR) < 0) return false;
    if (!readRegs(fd, OUT_X_H_M, data, 6)) return false; // X,Z,Y order
    mx = (int16_t)((data[0] << 8) | data[1]);
    mz = (int16_t)((data[2] << 8) | data[3]);
    my = (int16_t)((data[4] << 8) | data[5]);
    return true;
}
