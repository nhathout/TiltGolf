#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

class GameScreen : public QWidget {
	Q_OBJECT
public:
	GameScreen(QWidget *parent = nullptr);
	void setLevel(int newevelId);
signals:
	void exitToMenu();

private slots:
	void restartLevel();
	void updateTimer();

private:
	int levelId;
	int timeElapsed;
	QVBoxLayout *mainLayout;
	QHBoxLayout *topBar;
	QLabel *levelLabel;
	QLabel *timerLabel;
	QPushButton *restartButton;
	QPushButton *exitButton;
	QTimer *timer;
};

#endif

