#include "CalibrationDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen> // <-- required to use QScreen (was causing the incomplete type error)

CalibrationDialog::CalibrationDialog(QWidget *parent)
    : QDialog(parent)
{
    // Make the dialog compact and without big window decorations
    setWindowFlags(windowFlags() | Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    // Size chosen to fit a 480x272 screen comfortably
    const int W = 460;
    const int H = 110;
    setFixedSize(W, H);

    // Message label — concise so it doesn't wrap too much
    msgLabel = new QLabel("Calibration preview active.\nHold the board in the neutral position and observe the ball.\nSave to accept or Cancel to abort.", this);
    msgLabel->setWordWrap(true);
    msgLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    msgLabel->setMargin(6);

    // Buttons — large enough for touch
    saveButton = new QPushButton("Save", this);
    cancelButton = new QPushButton("Cancel", this);
    saveButton->setMinimumHeight(36);
    cancelButton->setMinimumHeight(36);
    saveButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Layout
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(saveButton);
    btnLayout->addWidget(cancelButton);

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addWidget(msgLabel, 1);
    main->addLayout(btnLayout);

    setLayout(main);

    // Position near bottom-center of parent (or screen if no parent)
    if (parent) {
        QPoint globalParent = parent->mapToGlobal(QPoint(0,0));
        int px = globalParent.x() + (parent->width() - width()) / 2;
        int py = globalParent.y() + parent->height() - height() - 8; // 8px margin from bottom
        move(px, py);
    } else {
        // center-bottom on primary screen (guard against null pointer)
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeom = screen->geometry();
            int px = screenGeom.x() + (screenGeom.width() - width()) / 2;
            int py = screenGeom.y() + screenGeom.height() - height() - 8;
            move(px, py);
        }
    }

    // Keep it non-modal so user can interact with game and tilt while preview is active
    setModal(false);

    connect(saveButton, &QPushButton::clicked, this, &CalibrationDialog::onSave);
    connect(cancelButton, &QPushButton::clicked, this, &CalibrationDialog::onCancel);

    // Let the dialog delete itself on close
    setAttribute(Qt::WA_DeleteOnClose);
}

void CalibrationDialog::onSave() {
    emit saveClicked();
    close();
}

void CalibrationDialog::onCancel() {
    emit cancelClicked();
    close();
}
