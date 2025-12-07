#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <vector>
#include <algorithm>
#include <cmath>
#include "box2d/box2d.h"

// Conversion factor: 15 pixels = 1 meter (compile-time constant)
const float PPM = 15.0f;

struct WallDef {
    b2Vec2 position; // Center position in meters
    b2Vec2 size;     // Half-width and Half-height in meters
};

struct MovingWaterDef {
    b2Vec2 basePosition; // nominal center position (meters)
    b2Vec2 position;     // current center position (meters), updated each step
    b2Vec2 size;         // Half-width and Half-height in meters
    float amplitude;     // vertical oscillation amplitude (meters)
    float speed;         // oscillation speed (radians per second)
    float phase;         // current phase (radians)
    float direction;     // 1.0 or -1.0 to invert motion
};

struct LevelConfig {
    int id;
    b2Vec2 ballStartPos;
    b2Vec2 holePos;
    float holeRadius;
    std::vector<WallDef> walls;
    std::vector<WallDef> water;
    std::vector<MovingWaterDef> movingWater;
    float width;  // World width in meters
    float height; // World height in meters
};

class LevelData {
public:
    static LevelConfig getLevel(int id) {
        LevelConfig level;
        level.id = id;

        // Define World Size (meters)
        level.width = 30.0f;
        level.height = 15.0f;

        // Default hole size (meters)
        level.holeRadius = 1.0f; // 1 meter radius

        // Wall thickness (half-size in meters).
        float wallThick = 0.5f;

        // Clear any existing geometry (safety)
        level.walls.clear();
        level.water.clear();
        level.movingWater.clear();

        // --- Boundary walls (placed fully inside the world) ---
        // Top wall: center at y = wallThick, spans full width
        level.walls.push_back({b2Vec2(level.width * 0.5f, wallThick), b2Vec2(level.width * 0.5f, wallThick)});

        // Bottom wall: center at y = height - wallThick, spans full width
        level.walls.push_back({b2Vec2(level.width * 0.5f, level.height - wallThick), b2Vec2(level.width * 0.5f, wallThick)});

        // Left wall: center at x = wallThick, spans full height
        level.walls.push_back({b2Vec2(wallThick, level.height * 0.5f), b2Vec2(wallThick, level.height * 0.5f)});

        // Right wall: center at x = width - wallThick, spans full height
        level.walls.push_back({b2Vec2(level.width - wallThick, level.height * 0.5f), b2Vec2(wallThick, level.height * 0.5f)});

        // --- Per-level placement ---
        if (id == 2)
        {
            // Level 2: top-right start, single horizontal bar near the top, large water region bottom-left, hole bottom-right.
            float margin = 1.5f;
            level.ballStartPos.Set(level.width - wallThick - margin, wallThick + margin); // top-right
            level.holePos.Set(level.width - wallThick - margin, level.height - wallThick - margin); // bottom-right

            // Horizontal bar spanning to the right wall
            float barHalfThickness = 0.25f;
            float leftGap = 4.0f; // leave some gap on the left
            float barHalfWidth = (level.width - wallThick - leftGap) * 0.5f;
            float barCenterX = level.width - wallThick - barHalfWidth; // touching right wall
            float barY = level.height * 0.70f;
            level.walls.push_back({b2Vec2(barCenterX, barY), b2Vec2(barHalfWidth, barHalfThickness)});

            // Water: shorter in height (~40% of screen), bottom-left region
            float waterHalfW = level.width * 0.32f;        // ~64% width coverage
            float waterHalfH = level.height * 0.20f;       // 40% total height
            float waterCenterX = waterHalfW + wallThick;   // from left border inward
            float waterCenterY = level.height - wallThick - waterHalfH - 0.5f; // small margin from bottom
            level.water.push_back({b2Vec2(waterCenterX, waterCenterY), b2Vec2(waterHalfW, waterHalfH)});

            return level;
        }
        if (id == 3)
        {
            // Level 3: two moving water blocks oscillating opposite, start top-left, hole bottom-right.
            float margin = 1.2f;
            level.ballStartPos.Set(wallThick + margin, wallThick + margin); // near top-left
            level.holePos.Set(level.width - wallThick - margin, level.height - wallThick - margin); // bottom-right

            // small walls
            float slimHalfW = 0.25f;
            level.walls.push_back({b2Vec2(level.width * 0.33f, level.height * 0.35f), b2Vec2(slimHalfW, 3.5f)});
            level.walls.push_back({b2Vec2(level.width * 0.66f, level.height * 0.70f), b2Vec2(slimHalfW, 3.5f)});

            // Moving water blocks (vertical oscillation, opposite directions)
            float waterHalfW = 2.3f;
            float waterHalfH = 1.8f;
            float amp = 2.5f;     // meters up/down
            float speed = 1.0f;   // radians/sec (slow)

            MovingWaterDef left;
            left.basePosition.Set(level.width * 0.38f, level.height * 0.60f);
            left.position = left.basePosition;
            left.size.Set(waterHalfW, waterHalfH);
            left.amplitude = amp;
            left.speed = speed;
            left.phase = 0.0f;
            left.direction = 1.0f;

            MovingWaterDef right;
            right.basePosition.Set(level.width * 0.62f, level.height * 0.60f);
            right.position = right.basePosition;
            right.size.Set(waterHalfW, waterHalfH);
            right.amplitude = amp;
            right.speed = speed;
            right.phase = 3.14159265f; // start opposite
            right.direction = -1.0f;

            level.movingWater.push_back(left);
            level.movingWater.push_back(right);

            return level;
        }

        if (id == 4)
        {
            // Level 4: randomly placed static water rectangles; start top-left, hole bottom-right.
            // putting water near edges to prevent players hugging the edge.
            float margin = 1.5f;
            level.ballStartPos.Set(wallThick + margin, wallThick + margin); // top-left
            level.holePos.Set(level.width - wallThick - margin, level.height - wallThick - margin); // bottom-right

            // Scatter water rectangles; leave gaps wide enough for the ball in some spots
            auto addWater = [&](float cx, float cy, float hx, float hy) {
                level.water.push_back({b2Vec2(cx, cy), b2Vec2(hx, hy)});
            };

            float wSmallX = 1.6f, wSmallY = 1.2f;
            float wMedX = 2.0f,  wMedY = 1.5f;

            addWater(level.width * 0.26f, level.height * 0.22f, wMedX, wMedY);
            addWater(level.width * 0.58f, level.height * 0.20f, wMedX, wSmallY);
            addWater(level.width * 0.80f, level.height * 0.32f, wSmallX, wMedY);
            addWater(level.width * 0.22f, level.height * 0.50f, wSmallX, wMedY);
            addWater(level.width * 0.50f, level.height * 0.50f, wMedX, wMedY);
            addWater(level.width * 0.76f, level.height * 0.52f, wSmallX, wMedY);
            addWater(level.width * 0.30f, level.height * 0.74f, wMedX, wSmallY);
            addWater(level.width * 0.56f, level.height * 0.78f, wSmallX, wSmallY);
            addWater(level.width * 0.80f, level.height * 0.72f, wSmallX, wSmallY);

            // water strips on the edges to block wall hugging (leave small gaps near start/hole)
            float edgeXGap = wallThick + 1.0f;
            float edgeStripHalfW = 0.6f;
            float edgeStripHalfH = 2.0f;
            // Left edge strips (avoiding start corner)
            addWater(edgeXGap, level.height * 0.35f, edgeStripHalfW, edgeStripHalfH);
            addWater(edgeXGap, level.height * 0.80f, edgeStripHalfW, edgeStripHalfH);
            // Right edge strips
            addWater(level.width - edgeXGap, level.height * 0.40f, edgeStripHalfW, edgeStripHalfH);
            addWater(level.width - edgeXGap, level.height * 0.78f, edgeStripHalfW, edgeStripHalfH);
            // Top  
            addWater(level.width * 0.60f, wallThick + 0.8f, 3.0f, 0.6f);
            // Bottom 
            addWater(level.width * 0.50f, level.height - wallThick - 0.8f, 4.0f, 0.6f);

            return level;
        }
        // Default Level (id == 1 or others): three evenly spaced horizontal walls and a flag at the hole.
        // Layout: start near top-left, hole near bottom-left. Walls sit at 1/4, 1/2, 3/4 height and alternate which side they touch (L, R, L)
        float margin = 1.5f; // meters from inner border for start/hole placement
        level.ballStartPos.Set(wallThick + margin, wallThick + margin + 0.5f); // top-left
        level.holePos.Set(wallThick + margin + 1.0f, level.height - wallThick - margin); // bottom-left-ish

        // Wall thickness (thin)
        float barHalfThickness = 0.25f; // 0.5 m tall

        // Three bars at 1/4, 1/2, 3/4 of the playfield height
        float y1 = level.height * 0.25f;
        float y2 = level.height * 0.50f;
        float y3 = level.height * 0.75f;

        // Left-touching bar (gap on the right)
        float gapRight = 3.0f;
        float lenLeft = (level.width - 2 * wallThick) - gapRight;
        float halfLeft = lenLeft * 0.5f;
        float centerLeft = wallThick + halfLeft;

        // Right-touching bar (gap on the left)
        float gapLeft = 3.0f;
        float lenRight = (level.width - 2 * wallThick) - gapLeft;
        float halfRight = lenRight * 0.5f;
        float centerRight = level.width - wallThick - halfRight;

        level.walls.push_back({b2Vec2(centerLeft, y1), b2Vec2(halfLeft, barHalfThickness)});   // bar 1 (touching left)
        level.walls.push_back({b2Vec2(centerRight, y2), b2Vec2(halfRight, barHalfThickness)}); // bar 2 (touching right)
        level.walls.push_back({b2Vec2(centerLeft, y3), b2Vec2(halfLeft, barHalfThickness)});   // bar 3 (touching left)

        return level;
    }
};

#endif
