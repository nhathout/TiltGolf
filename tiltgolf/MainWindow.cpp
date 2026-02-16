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
	
	// Set up screens
	menu = new MenuScreen(this);
	game = new GameScreen(this);
	stack->addWidget(menu);
	stack->addWidget(game);

	// Connect screen signals to main window slots
	connect(menu, &MenuScreen::levelSelected, this, &MainWindow::startLevel);
	connect(game, &GameScreen::exitToMenu, this, &MainWindow::returnToMenu);

	// Open to menu in full screen
	setWindowState(Qt::WindowFullScreen);
	stack->setCurrentWidget(menu);
}

// Slot implementation: start a level (called from menu)
void MainWindow::startLevel(int levelId) {
	game->setLevel(levelId);
	stack->setCurrentWidget(game);
}

// Slot implementation: return to main menu
void MainWindow::returnToMenu() {
	stack->setCurrentWidget(menu);
}