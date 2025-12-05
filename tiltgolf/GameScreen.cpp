#include "GameScreen.h"
#include "IMUScreen.h"
#include <QString>
#include <QMessageBox>

GameScreen::GameScreen(QWidget *parent) : QWidget(parent) {
    // 1. Create Controller
    controller = new GameController(this);

    // 2. Create View (pass controller to it)
    gameView = new GameView(controller, this);
    
    // 3. Setup Layout
    mainLayout = new QVBoxLayout(this);
    topBar = new QHBoxLayout();
    
    levelLabel = new QLabel(QString("Level %1").arg(1));
    timeElapsed = 0;
    timerLabel = new QLabel(QString("Time: %1").arg(timeElapsed));
    
    restartButton = new QPushButton("Restart");
    exitButton = new QPushButton("Exit");

    // Top Bar Widgets
    topBar->addWidget(levelLabel);
    topBar->addStretch();
    topBar->addWidget(timerLabel);
    topBar->addStretch();
    topBar->addWidget(restartButton);
    topBar->addWidget(exitButton);

    mainLayout->addLayout(topBar);
    
    // Add the Game View (Physics rendering) to the center
    mainLayout->addWidget(gameView, 1); // 1 = stretch factor

    setLayout(mainLayout);

    // 4. Connect Signals
    connect(restartButton, &QPushButton::clicked, this, &GameScreen::restartLevel);
    connect(exitButton, &QPushButton::clicked, this, &GameScreen::exitToMenu);
    
    // Connect Controller Win Signal
    connect(controller, &GameController::gameWon, this, &GameScreen::handleLevelComplete);

    // Game Timer (for UI clock, not physics)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameScreen::updateTimer);
    
    // Start Level 1 immediately (or wait for setLevel)
    controller->loadLevel(1);
    timer->start(1000);
}

void GameScreen::restartLevel() {
    timeElapsed = 0;
    timerLabel->setText("Time: 0");
    controller->resetGame();
}

void GameScreen::updateTimer() {
    timeElapsed++;
    timerLabel->setText(QString("Time: %1").arg(timeElapsed));
}

void GameScreen::setLevel(int newLevelId) {
    levelId = newLevelId;
    levelLabel->setText(QString("Level %1").arg(levelId));
    controller->loadLevel(levelId);
    timeElapsed = 0;
}

void GameScreen::handleLevelComplete() {
    QMessageBox::information(this, "Winner!", "You sank the ball!");
    restartLevel(); // Or advance to next level
}