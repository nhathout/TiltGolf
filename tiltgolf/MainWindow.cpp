#include "MainWindow.h"
#include "MenuScreen.h"
#include "GameScreen.h"

// Main window constructor implementation
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

// Start level slot implementation
void MainWindow::startLevel(int levelId) {
	game->setLevel(levelId);
	stack->setCurrentWidget(game);
}

// Return to menu slot implmentation
void MainWindow::returnToMenu() {
	stack->setCurrentWidget(menu);
}
