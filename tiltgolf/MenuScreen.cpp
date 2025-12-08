#include "MenuScreen.h"
#include <QApplication>

// Menu screen constructor implementation
MenuScreen::MenuScreen(QWidget *parent) : QWidget(parent) {
	// Set background color
	setStyleSheet("background-color: #008000;");
	unlockedLevels.assign(6, false);
	unlockedLevels[0] = true;
	
	// Layout for title, exit button, and level select grid
	mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(20, 20, 20, 20);
	mainLayout->setSpacing(20);

	topLayout = new QHBoxLayout();	

	levelGridLayout = new QGridLayout();
	levelGridLayout->setSpacing(15);

	// Set up title
	title = new QLabel("TiltGolf!", this);
	title->setAlignment(Qt::AlignCenter);
	title->setStyleSheet("color: white; font-size: 36px; font-weight: bold;");

	// Create Free Play button (left) and Exit button (right)
	freePlayButton = new QPushButton("Free Play", this);
	freePlayButton->setFixedSize(110, 40);
	freePlayButton->setStyleSheet("background-color: #1e90ff; color: white; font-size: 18px; font-weight: bold;");
	connect(freePlayButton, &QPushButton::clicked, [this]() {
		setFreePlayMode(!freePlayMode);
	});

	exitButton = new QPushButton("Exit", this);
	exitButton->setFixedSize(100, 40);
	exitButton->setStyleSheet("background-color: red; color: white; font-size: 18px; font-weight: bold;");
	connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
	topLayout->addWidget(freePlayButton, 0, Qt::AlignLeft);
	topLayout->addStretch();
	topLayout->addWidget(exitButton, 0, Qt::AlignRight);
	
	// Set up level select grid for 6 levels
	for (int i = 0; i < 6; i++) {
		// Create button and connect it to levelSelected signal 
		QPushButton *levelButton = new QPushButton(QString("%1").arg(i+1));
		levelButtons.append(levelButton);
		connect(levelButton, &QPushButton::clicked, [this, i]() {
			emit levelSelected(i + 1); // emit 1-based level ids
		});

		// Add button to grid at position i/3, i%3 (3 column layout)
		levelGridLayout->addWidget(levelButton, i/3, i%3);
	}
	refreshLevelButtons();

	// Construct mainLayout
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(title);
	mainLayout->addLayout(levelGridLayout);
	mainLayout->addStretch();

	setLayout(mainLayout);
}

void MenuScreen::setUnlockStates(const std::vector<bool>& states) {
	unlockedLevels = states;
	if (unlockedLevels.empty()) {
		unlockedLevels.assign(levelButtons.size(), false);
	}
	// Always keep level 1 unlocked minimum
	if (!unlockedLevels.empty()) {
		unlockedLevels[0] = true;
	}
	refreshLevelButtons();
}

void MenuScreen::setFreePlayMode(bool enabled) {
	freePlayMode = enabled;
	freePlayButton->setText(enabled ? "Free Play (On)" : "Free Play");
	refreshLevelButtons();
}

void MenuScreen::refreshLevelButtons() {
	QString unlockedStyle = "background-color: yellow; color: white; font-size: 18px; font-weight: bold;";
	QString lockedStyle = "background-color: black; color: red; font-size: 18px; font-weight: bold; border: 2px solid red;";

	for (int i = 0; i < levelButtons.size(); ++i) {
		bool isUnlocked = freePlayMode;
		if (!freePlayMode) {
			if (i < static_cast<int>(unlockedLevels.size())) {
				isUnlocked = unlockedLevels[i];
			} else {
				isUnlocked = false;
			}
		}

		QPushButton *btn = levelButtons[i];
		if (isUnlocked) {
			btn->setEnabled(true);
			btn->setText(QString("%1").arg(i + 1));
			btn->setStyleSheet(unlockedStyle);
			btn->setToolTip("Play level");
		} else {
			btn->setEnabled(false);
			btn->setText("X");
			btn->setStyleSheet(lockedStyle);
			btn->setToolTip("Locked - complete previous level");
		}
	}
}