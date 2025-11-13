#include "MenuScreen.h"
#include <QApplication>

// Menu screen constructor implementation
MenuScreen::MenuScreen(QWidget *parent) : QWidget(parent) {
	// Set background color
	setStyleSheet("background-color: #008000;");
	
	// Layout for title, exit button, and level select grid
	mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(20, 20, 20, 20);
	mainLayout->setSpacing(20);

	topLayout = new QHBoxLayout();
	topLayout->addStretch();

	levelGridLayout = new QGridLayout();
	levelGridLayout->setSpacing(15);

	// Set up title
	title = new QLabel("TiltGolf!", this);
	title->setAlignment(Qt::AlignCenter);
	title->setStyleSheet("color: white; font-size: 36px; font-weight: bold;");

	// Create exit button and connect it to application quit function
	QPushButton *exitButton = new QPushButton("Exit", this);
	exitButton->setFixedSize(100, 40);
	exitButton->setStyleSheet("background-color: red; color: white; font-size: 18px; font-weight: bold;");
	connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
	topLayout->addWidget(exitButton, 0, Qt::AlignRight);
	
	// Set up level select grid for 6 levels
	for (int i = 0; i < 6; i++) {
		// Create button and connect it to levelSelected signal 
		QPushButton *levelButton = new QPushButton(QString("%1").arg(i+1));
		levelButton->setStyleSheet("background-color: yellow; color: white; font-size: 18px; font-weight: bold;");
		connect(levelButton, &QPushButton::clicked, [this, i]() {
			emit levelSelected(i);
		});

		// Add button to grid at position i/3, i%3 (3 column layout)
		levelGridLayout->addWidget(levelButton, i/3, i%3);
	}

	// Construct mainLayout
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(title);
	mainLayout->addLayout(levelGridLayout);
	mainLayout->addStretch();

	setLayout(mainLayout);
}
