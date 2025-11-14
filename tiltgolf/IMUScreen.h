#ifndef IMUSCREEN_H
#define IMUSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <stdint.h>

class IMUScreen : public QWidget {
    Q_OBJECT

public:
    IMUScreen(QWidget *parent = nullptr);
    ~IMUScreen();

signals:
    void backToGameScreen();

private slots:
    void updateIMU();
    void handleBackButton();

private:
    QLabel *accLabel;
    QLabel *magLabel;
    QPushButton *backButton;
    QTimer *timer;

    int i2c_fd;

    // Helper functions for I2C
    bool writeReg(uint8_t reg, uint8_t value);
    bool readRegs(uint8_t start, uint8_t *data, int len);
    bool initAccelerometer();
    bool initMagnetometer();
    bool readAccelerometer(int16_t &ax, int16_t &ay, int16_t &az);
    bool readMagnetometer(int16_t &mx, int16_t &my, int16_t &mz);
};

#endif
