#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <vector>
#include <algorithm>
#include "box2d/box2d.h"

// Conversion factor: 15 pixels = 1 meter (compile-time constant)
const float PPM = 15.0f;

struct WallDef {
    b2Vec2 position; // Center position in meters
    b2Vec2 size;     // Half-width and Half-height in meters
};

struct LevelConfig {
    int id;
    b2Vec2 ballStartPos;
    b2Vec2 holePos;
    float holeRadius;
    std::vector<WallDef> walls;
    std::vector<WallDef> water;
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

            // Horizontal bar spanning most of the playfield, placed near the upper third
            float barHalfThickness = 0.25f;
            float barMargin = 2.0f;
            float barHalfWidth = (level.width - 2 * barMargin) * 0.5f;
            float barCenterX = level.width * 0.5f;
            float barY = level.height * 0.33f;
            level.walls.push_back({b2Vec2(barCenterX, barY), b2Vec2(barHalfWidth, barHalfThickness)});

            // Water: large rectangle covering bottom-left ~2/3 width and bottom half height
            float waterHalfW = level.width * 0.33f;        // ~10 m half-width (covers ~20 m of 30)
            float waterHalfH = level.height * 0.40f;       // covers most of lower half
            float waterCenterX = waterHalfW + wallThick;   // start from left border inward
            float waterCenterY = level.height - wallThick - waterHalfH;
            level.water.push_back({b2Vec2(waterCenterX, waterCenterY), b2Vec2(waterHalfW, waterHalfH)});

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
