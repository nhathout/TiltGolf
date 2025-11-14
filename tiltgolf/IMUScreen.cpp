#include "IMUScreen.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>

using namespace std;

// I2C addresses
#define ACC_ADDR  0x19
#define MAG_ADDR  0x1E

// Accelerometer registers
#define CTRL_REG1_A 0x20
#define OUT_X_L_A   0x28

// Magnetometer registers
#define CRA_REG_M   0x00
#define CRB_REG_M   0x01
#define MR_REG_M    0x02
#define OUT_X_H_M   0x03

IMUScreen::IMUScreen(QWidget *parent) : QWidget(parent), i2c_fd(-1)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    accLabel = new QLabel("Accel: X=0 Y=0 Z=0");
    magLabel = new QLabel("Mag: X=0 Y=0 Z=0");
    backButton = new QPushButton("Back");

    layout->addWidget(accLabel);
    layout->addWidget(magLabel);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, this, &IMUScreen::handleBackButton);

    // Open I2C bus
    i2c_fd = open("/dev/i2c-2", O_RDWR);
    if (i2c_fd < 0) {
        accLabel->setText("Failed to open I2C bus");
        magLabel->setText("");
        return;
    }

    if (!initAccelerometer() || !initMagnetometer()) {
        accLabel->setText("IMU disconnected!");
        magLabel->setText("");
        ::close(i2c_fd);
        i2c_fd = -1;
        return;
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &IMUScreen::updateIMU);
    timer->start(200); // update 5 times per second
}

IMUScreen::~IMUScreen()
{
    if (i2c_fd >= 0) ::close(i2c_fd);
}

// ----------------------
// Back button
// ----------------------
void IMUScreen::handleBackButton()
{
    timer->stop();
    emit backToGameScreen();
    this->close();
}

// ----------------------
// I2C helper functions
// ----------------------
bool IMUScreen::writeReg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return write(i2c_fd, buf, 2) == 2;
}

bool IMUScreen::readRegs(uint8_t start, uint8_t *data, int len)
{
    if (write(i2c_fd, &start, 1) != 1) return false;
    return read(i2c_fd, data, len) == len;
}

bool IMUScreen::initAccelerometer()
{
    if (ioctl(i2c_fd, I2C_SLAVE, ACC_ADDR) < 0) return false;
    return writeReg(CTRL_REG1_A, 0x27); // enable, 50Hz, all axes
}

bool IMUScreen::initMagnetometer()
{
    if (ioctl(i2c_fd, I2C_SLAVE, MAG_ADDR) < 0) return false;
    if (!writeReg(CRA_REG_M, 0x10)) return false;  // 15 Hz
    if (!writeReg(CRB_REG_M, 0x20)) return false;  // gain
    return writeReg(MR_REG_M, 0x00);              // continuous
}

bool IMUScreen::readAccelerometer(int16_t &ax, int16_t &ay, int16_t &az)
{
    uint8_t data[6];
    if (ioctl(i2c_fd, I2C_SLAVE, ACC_ADDR) < 0) return false;
    if (!readRegs(OUT_X_L_A | 0x80, data, 6)) return false;
    ax = (int16_t)((data[1] << 8) | data[0]);
    ay = (int16_t)((data[3] << 8) | data[2]);
    az = (int16_t)((data[5] << 8) | data[4]);
    return true;
}

bool IMUScreen::readMagnetometer(int16_t &mx, int16_t &my, int16_t &mz)
{
    uint8_t data[6];
    if (ioctl(i2c_fd, I2C_SLAVE, MAG_ADDR) < 0) return false;
    if (!readRegs(OUT_X_H_M, data, 6)) return false;
    mx = (int16_t)((data[0] << 8) | data[1]);
    mz = (int16_t)((data[2] << 8) | data[3]);
    my = (int16_t)((data[4] << 8) | data[5]);
    return true;
}

// ----------------------
// Update function called by timer
// ----------------------
void IMUScreen::updateIMU()
{
    if (i2c_fd < 0) return;

    int16_t ax, ay, az;
    int16_t mx, my, mz;

    bool acc_ok = readAccelerometer(ax, ay, az);
    bool mag_ok = readMagnetometer(mx, my, mz);

    if (!acc_ok || !mag_ok) {
        accLabel->setText("IMU disconnected!");
        magLabel->setText("");
        return;
    }

    accLabel->setText(QString("Accel: X=%1 Y=%2 Z=%3").arg(ax).arg(ay).arg(az));
    magLabel->setText(QString("Mag: X=%1 Y=%2 Z=%3").arg(mx).arg(my).arg(mz));
}
