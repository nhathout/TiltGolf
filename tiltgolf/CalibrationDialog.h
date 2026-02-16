#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;

class CalibrationDialog : public QDialog {
    Q_OBJECT
public:
    explicit CalibrationDialog(QWidget *parent = nullptr);
    ~CalibrationDialog() override = default;

signals:
    void saveClicked();
    void cancelClicked();

private slots:
    void onSave();
    void onCancel();

private:
    QLabel *msgLabel;
    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif
