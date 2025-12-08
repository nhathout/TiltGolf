#include "MainWindow.h"
#include "MenuScreen.h"
#include "GameScreen.h"
#include "LevelData.h"

// Note: LevelData.h in your current repo is the simple version where PPM is
// a compile-time constant. If you want to change pixels-per-meter, edit
// the PPM value in LevelData.h directly.

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	// Set up stack widget
	stack = new QStackedWidget(this);
	setCentralWidget(stack);

	// start unlock state: only level 1 unlocked by default
	unlockedLevels.assign(6, false);
	unlockedLevels[0] = true;
	
	// Set up screens
	menu = new MenuScreen(this);
	game = new GameScreen(this);
	stack->addWidget(menu);
	stack->addWidget(game);

	// main menu with unlocks
	menu->setUnlockStates(unlockedLevels);
	game->setTotalLevels(static_cast<int>(unlockedLevels.size()));

	// Connect screen signals to main window slots
	connect(menu, &MenuScreen::levelSelected, this, &MainWindow::startLevel);
	connect(game, &GameScreen::exitToMenu, this, &MainWindow::returnToMenu);
	connect(game, &GameScreen::levelComplete, this, &MainWindow::handleLevelComplete);

	// Open to menu in full screen
	setWindowState(Qt::WindowFullScreen);
	stack->setCurrentWidget(menu);
}

// Slot implementation: start a level (called from menu)
void MainWindow::startLevel(int levelId) {
	game->setFreePlayMode(menu->isFreePlayMode());
	game->setLevel(levelId);
	stack->setCurrentWidget(game);
}

// Slot implementation: return to main menu
void MainWindow::returnToMenu() {
	game->resetAttemptCounter();
	stack->setCurrentWidget(menu);
}

// handles unlocks
void MainWindow::handleLevelComplete(int levelId) {
	int idx = levelId - 1;
	if (idx >= 0 && idx + 1 < static_cast<int>(unlockedLevels.size())) {
		if (!unlockedLevels[idx + 1]) {
			unlockedLevels[idx + 1] = true;
			menu->setUnlockStates(unlockedLevels);
		}
	}
}