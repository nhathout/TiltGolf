#include "GameScreen.h"
#include "GameController.h"
#include "GameView.h"
#include "CalibrationDialog.h"
#include <QString>
#include <QMessageBox>
#include <memory>

GameScreen::GameScreen(QWidget *parent) : QWidget(parent) {
    // 1. Create Controller
    controller = new GameController(this);

    // 2. Create View (pass controller to it)
    gameView = new GameView(controller, this);
    
    // 3. Setup Layout
    mainLayout = new QVBoxLayout(this);
    topBar = new QHBoxLayout();
    
    levelId = 1;
    levelLabel = new QLabel(QString("Level %1").arg(1));
    timeElapsed = 0;
    timerLabel = new QLabel(QString("Time: %1").arg(timeElapsed));
    
    restartButton = new QPushButton("Restart");
    exitButton = new QPushButton("Exit");
    calibrateButton = new QPushButton("Calibrate"); // new

    // Top Bar Widgets
    topBar->addWidget(levelLabel);
    topBar->addStretch();
    topBar->addWidget(timerLabel);
    topBar->addStretch();
    topBar->addWidget(calibrateButton); // add calibrate button to top bar
    topBar->addWidget(restartButton);
    topBar->addWidget(exitButton);

    mainLayout->addLayout(topBar);
    
    // Add the Game View (Physics rendering) to the center
    mainLayout->addWidget(gameView, 1); // 1 = stretch factor

    setLayout(mainLayout);

    // 4. Connect Signals
    connect(restartButton, &QPushButton::clicked, this, &GameScreen::restartLevel);

    // Exit: pause the running controller before switching screens so the game loop stops
    connect(exitButton, &QPushButton::clicked, [this]()
            {
        // Pause game loop so physics stops running in the background
        if (controller) controller->pauseGame();
        emit exitToMenu(); });

    // Connect Controller Win Signal
    connect(controller, &GameController::gameWon, this, &GameScreen::handleLevelComplete);

    // Calibrate button: start preview, show compact non-modal dialog with Save/Cancel
    connect(calibrateButton, &QPushButton::clicked, [this]()
            {
        // Start a live preview (temp bias applied immediately)
        // IMPORTANT: pass false so the ball is NOT forcibly reset/spawned when starting preview
        controller->startCalibrationPreview(false);

        // Create compact non-modal dialog that lets user save or cancel
        CalibrationDialog *dlg = new CalibrationDialog(this);

        // Use shared flag so we can detect if Save was clicked
        auto saved = std::make_shared<bool>(false);

        // When Save is tapped, commit preview and mark saved=true
        connect(dlg, &CalibrationDialog::saveClicked, [this, saved, dlg]() {
            controller->acceptCalibrationPreview();
            *saved = true;
            // Close the dialog (will trigger destroyed/finished signals)
            dlg->close();
        });

        // When Cancel is tapped, cancel preview immediately
        connect(dlg, &CalibrationDialog::cancelClicked, [this, saved, dlg]() {
            controller->cancelCalibrationPreview();
            *saved = false;
            dlg->close();
        });

        // If the dialog is destroyed/closed by other means and Save wasn't clicked,
        // cancel the preview. If it was saved, we've already committed.
        connect(dlg, &CalibrationDialog::destroyed, [this, saved]() {
            if (!(*saved)) {
                controller->cancelCalibrationPreview();
            }
        });

        // Show the compact dialog non-modally
        dlg->show(); });

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
    emit levelComplete(levelId);
    restartLevel(); // Or advance to next level
}