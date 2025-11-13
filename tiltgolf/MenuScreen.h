#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MenuScreen: public QWidget {
	Q_OBJECT
public:
	MenuScreen(QWidget *parent = nullptr);

signals:
	void levelSelected(int levelId);

private:
	QVBoxLayout *mainLayout;
	QHBoxLayout *topLayout;
	QGridLayout *levelGridLayout;
	QLabel *title;
	QPushButton *exitButton;
};

#endif
