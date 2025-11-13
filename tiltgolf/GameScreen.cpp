#include "GameScreen.h"
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

	// Construct top bar
	topBar->addWidget(levelLabel);
	topBar->addStretch();
	topBar->addWidget(timerLabel);
        topBar->addStretch();
	topBar->addWidget(restartButton);
	topBar->addWidget(exitButton);

	mainLayout->addLayout(topBar);
	setLayout(mainLayout);

	connect(restartButton, &QPushButton::clicked, this, &GameScreen::restartLevel);
	connect(exitButton, &QPushButton::clicked, this, &GameScreen::exitToMenu);

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
