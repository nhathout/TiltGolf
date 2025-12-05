#include "GameView.h"
#include <algorithm>

GameView::GameView(GameController *controller, QWidget *parent)
    : QWidget(parent), controller(controller)
{
    // Optimize for embedded
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("background-color: #228B22;"); // Forest Green
    
    // Listen to controller updates
    connect(controller, &GameController::gameStateUpdated, this, &GameView::updateView);
}

void GameView::updateView() {
    // Schedule a redraw
    update(); 
}

void GameView::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    LevelConfig level = controller->getCurrentLevel();
    b2Vec2 ballPos = controller->getBallPos();

    // --- 1. Draw Hole (Black) ---
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    
    float holePxRadius = toPixels(level.holeRadius);
    QPointF holeCenter = toPixels(level.holePos);
    
    painter.drawEllipse(holeCenter, holePxRadius, holePxRadius);

    // --- 2. Draw Walls (Brown/Dark Green) ---
    painter.setBrush(QColor(139, 69, 19)); // SaddleBrown
    
    for (const auto& wall : level.walls) {
        QPointF center = toPixels(wall.position);
        float w = toPixels(wall.size.x); // Box2D uses half-width
        float h = toPixels(wall.size.y); // Box2D uses half-height
        
        // DrawRect takes top-left, width, height. 
        // Our center is center, w/h are half-extents.
        painter.drawRect(QRectF(center.x() - w, center.y() - h, w*2, h*2));
    }

    // --- 3. Draw Ball (White) ---
    painter.setBrush(Qt::white);
    painter.setPen(Qt::black);
    
    QPointF ballCenter = toPixels(ballPos);
    float ballRadius = toPixels(0.5f); // 0.5m radius from PhysicsEngine
    
    painter.drawEllipse(ballCenter, ballRadius, ballRadius);
}

// Convert a world-space vector (meters) to screen pixels using compile-time PPM
QPointF GameView::toPixels(const b2Vec2 &vec) const
{
    return QPointF(vec.x * PPM, vec.y * PPM);
}

// Convert a scalar in meters to pixels
float GameView::toPixels(float meters) const
{
    return meters * PPM;
}