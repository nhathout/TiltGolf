#include "GameView.h"
#include <QPolygonF>

GameView::GameView(GameController *controller, QWidget *parent)
    : QWidget(parent), controller(controller)
{
    // Optimize for embedded
    // We will paint the entire widget each frame, so tell Qt this widget is opaque
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false); // we paint the background ourselves

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

    // --- CLEAR THE BACKGROUND FIRST (VERY IMPORTANT) ---
    // Paint the ground (green) to cover the entire widget so nothing beneath shows through.
    QColor groundColor(34, 139, 34); // same forest green as before
    painter.fillRect(rect(), groundColor);

    LevelConfig level = controller->getCurrentLevel();
    b2Vec2 ballPos = controller->getBallPos();

    // 1. Draw Water (Blue)
    painter.setBrush(QColor(0, 120, 255, 180)); // translucent blue
    painter.setPen(Qt::NoPen);
    for (const auto& water : level.water) {
        QPointF center = toPixels(water.position);
        float w = toPixels(water.size.x);
        float h = toPixels(water.size.y);
        painter.drawRect(QRectF(center.x() - w, center.y() - h, w * 2, h * 2));
    }
    for (const auto& water : level.movingWater) {
        QPointF center = toPixels(water.position);
        float w = toPixels(water.size.x);
        float h = toPixels(water.size.y);
        painter.drawRect(QRectF(center.x() - w, center.y() - h, w * 2, h * 2));
    }

    // Draw Walls (Brown) 
    painter.setBrush(QColor(139, 69, 19)); // SaddleBrown (walls)
    painter.setPen(Qt::NoPen);

    for (const auto& wall : level.walls) {
        QPointF center = toPixels(wall.position);
        float w = toPixels(wall.size.x); // Box2D uses half-width
        float h = toPixels(wall.size.y); // Box2D uses half-height

        // Draw rectangle centered at 'center'
        painter.drawRect(QRectF(center.x() - w, center.y() - h, w*2, h*2));
    }
    // 3. Draw Hole (Black)
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    
    float holePxRadius = toPixels(level.holeRadius);
    QPointF holeCenter = toPixels(level.holePos);
    
    painter.drawEllipse(holeCenter, holePxRadius, holePxRadius);

    // Draw Flag at the Hole 
    {
        // Keep the flag compact so it just accents the hole
        float poleHeight = toPixels(level.holeRadius * 1.2f);
        float poleWidth = 1.5f; // pixels
        QPointF poleTop(holeCenter.x(), holeCenter.y() - poleHeight);

        painter.setPen(QPen(Qt::black, poleWidth));
        painter.drawLine(holeCenter, poleTop);

        float flagWidth = toPixels(level.holeRadius * 0.8f);
        float flagHeight = toPixels(level.holeRadius * 0.6f);

        QPolygonF flagShape;
        flagShape << poleTop
                  << QPointF(poleTop.x() + flagWidth, poleTop.y() + flagHeight * 0.3f)
                  << QPointF(poleTop.x(), poleTop.y() + flagHeight);

        painter.setBrush(QColor(220, 30, 30)); // red flag
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(flagShape);
    }

    // Draw Start 
    painter.setBrush(QColor(0, 90, 0));
    painter.setPen(QPen(Qt::black, 1));
    QPointF startCenter = toPixels(level.ballStartPos);
    float startRadius = toPixels(0.6f);
    painter.drawEllipse(startCenter, startRadius, startRadius);

    // Draw Ball (White)
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