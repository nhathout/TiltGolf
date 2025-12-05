#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QPainter>
#include "GameController.h"
#include "LevelData.h"

class GameView : public QWidget {
    Q_OBJECT

public:
    explicit GameView(GameController* controller, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void updateView(); // Slot to trigger repaint

private:
    GameController* controller;
    
    // Helper to convert meters to pixels
    QPointF toPixels(b2Vec2 vec);
    float toPixels(float meters);
};

#endif