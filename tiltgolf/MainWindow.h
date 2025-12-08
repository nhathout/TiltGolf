#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "MenuScreen.h"
#include "GameScreen.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <vector>

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent= nullptr);

private:
	QStackedWidget *stack;
	MenuScreen *menu;
	GameScreen *game;
	std::vector<bool> unlockedLevels;

private slots:
	void startLevel(int levelId);
	void returnToMenu();
	void handleLevelComplete(int levelId);
};

#endif
