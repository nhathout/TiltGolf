#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <vector>
#include "box2d/box2d.h"

// Conversion factor: 30 pixels = 1 meter
// Adjust this based on your screen size (e.g., 800x480 vs 480x272)
const float PPM = 30.0f;

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
    float width;  // World width in meters
    float height; // World height in meters
};

class LevelData {
public:
    static LevelConfig getLevel(int id) {
        LevelConfig level;
        level.id = id;

        // Define World Size (e.g., 800px / 30 = ~26.6m width)
        level.width = 26.0f;
        level.height = 15.0f;

        // 1. Ball spawns Top Right
        level.ballStartPos.Set(level.width - 4.0f, 4.0f);

        // 2. Hole in Center
        level.holePos.Set(level.width / 2.0f, level.height / 2.0f);
        level.holeRadius = 1.0f; // 1 meter radius

        // 3. Walls on all 4 sides
        float wallThick = 0.5f;
        
        // Top Wall
        level.walls.push_back({ b2Vec2(level.width/2, 0.0f), b2Vec2(level.width/2, wallThick) });
        // Bottom Wall
        level.walls.push_back({ b2Vec2(level.width/2, level.height), b2Vec2(level.width/2, wallThick) });
        // Left Wall
        level.walls.push_back({ b2Vec2(0.0f, level.height/2), b2Vec2(wallThick, level.height/2) });
        // Right Wall
        level.walls.push_back({ b2Vec2(level.width, level.height/2), b2Vec2(wallThick, level.height/2) });

        return level;
    }
};

#endif