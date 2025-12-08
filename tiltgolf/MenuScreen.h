#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <vector>

class MenuScreen: public QWidget {
	Q_OBJECT
public:
	MenuScreen(QWidget *parent = nullptr);
	void setUnlockStates(const std::vector<bool>& states);
	bool isFreePlayMode() const { return freePlayMode; }
	int levelCount() const { return levelButtons.size(); }

signals:
	void levelSelected(int levelId);

private:
	QVBoxLayout *mainLayout;
	QHBoxLayout *topLayout;
	QGridLayout *levelGridLayout;
	QLabel *title;
	QPushButton *exitButton;
	QPushButton *freePlayButton;
	QVector<QPushButton*> levelButtons;
	std::vector<bool> unlockedLevels;
	bool freePlayMode = false;

	void refreshLevelButtons();
	void setFreePlayMode(bool enabled);
};

#endif