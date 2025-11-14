#include "GameScreen.h"
#include "IMUScreen.h"
#include <QString>

// Game screen constructor implementation
GameScreen::GameScreen(QWidget *parent) : QWidget(parent) {
	// Allocate the layouts, labels, and buttons
	mainLayout = new QVBoxLayout(this);
	topBar = new QHBoxLayout();
	levelLabel = new QLabel(QString("Level %1").arg(0));
	timeElapsed= 0;
	timerLabel = new QLabel(QString("%1").arg(timeElapsed));
	restartButton = new QPushButton("Restart");
	exitButton = new QPushButton("Exit");
	imuButton = new QPushButton("IMU Debug");	//skanda: New IMU Debug button

	// Construct top bar
	topBar->addWidget(levelLabel);
	topBar->addStretch();
	topBar->addWidget(timerLabel);
        topBar->addStretch();
	topBar->addWidget(restartButton);
	topBar->addWidget(exitButton);
	topBar->addWidget(imuButton);	//skanda: Add IMU Debug button to top bar

	mainLayout->addLayout(topBar);
	setLayout(mainLayout);

	connect(restartButton, &QPushButton::clicked, this, &GameScreen::restartLevel);
	connect(exitButton, &QPushButton::clicked, this, &GameScreen::exitToMenu);
	connect(imuButton, &QPushButton::clicked, this, &GameScreen::openIMUScreen);


	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &GameScreen::updateTimer);
	timer->start(1000);

}

// Level restart slot implementation
void GameScreen::restartLevel() {
	timeElapsed = 0;
}

// Timer update slot implementation
void GameScreen::updateTimer() {
	timeElapsed++;
	timerLabel->setText(QString("%1 s").arg(timeElapsed));
}

void GameScreen::setLevel(int newLevelId) {
	levelId = newLevelId;
}

void GameScreen::openIMUScreen() {
    if (imuScreen) return; // already open

    imuScreen = new IMUScreen();
    imuScreen->setAttribute(Qt::WA_DeleteOnClose); // auto cleanup

    // Connect back button
    connect(imuScreen, &IMUScreen::backToGameScreen, this, [=]() {
        imuScreen = nullptr; // forget the pointer
        // GameScreen is already visible, just close IMU screen
    });

    // Optional: if you want to hide the game screen while IMU screen is open:
    // this->hide();
    imuScreen->showFullScreen();
}