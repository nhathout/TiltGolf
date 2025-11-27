
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2025 Laurent Gomila (laurent@sfml-dev.org)
//

// Box2D/SFML Headers

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/StencilMode.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/View.hpp>

#include <SFML/Window.hpp>

#include <box2d/box2d.h>
#include <algorithm>
#include <optional>

static const float SCALE = 30.0f; // means 30px = 1m

// helper function to create/draw wall
b2BodyId createWall(b2WorldId worldId, float centerXpx, float centerYpx, float widthPx, float heightPx)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody; // ball will be dynamic
    bodyDef.position = {centerXpx / SCALE, centerYpx / SCALE};
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon box = b2MakeBox((widthPx / 2.0f) / SCALE, (heightPx / 2.0f) / SCALE);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.0f;
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 1.0f; // walls are bouncy
    b2CreatePolygonShape(bodyId, &shapeDef, &box);

    return bodyId;
}

int main()
{
    const unsigned int WINDOW_W = 800; // change to px of LCD screen
    const unsigned int WINDOW_H = 600;

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_W, WINDOW_H)), "Box2D TEST");

    window.setFramerateLimit(60);

    // make the Box2D world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f}; // no gravity; top-down view
    b2WorldId world = b2CreateWorld(&worldDef);

    // make the walls
    const float wallThickness = 20.0f; // px

    // top wall
    createWall(world, WINDOW_W / 2.0f, wallThickness / 2.0f, WINDOW_W, wallThickness);
    // bottom wall
    createWall(world, WINDOW_W / 2.0f, WINDOW_H - wallThickness / 2.0f, WINDOW_W, wallThickness);
    // left wall
    createWall(world, wallThickness / 2.0f, WINDOW_H / 2.0f, wallThickness, WINDOW_H);
    // right wall
    createWall(world, WINDOW_W - wallThickness / 2.0f, WINDOW_H / 2.0f, wallThickness, WINDOW_H);

    // make the dynamic ball
    b2BodyDef ballDef = b2DefaultBodyDef();
    ballDef.type = b2_dynamicBody;
    // start ball in the middle of the window
    ballDef.position = {(WINDOW_W / 2.0f) / SCALE, (WINDOW_H / 2.0f) / SCALE};
    ballDef.isBullet = true; // prevents "tunneling" where at high speeds the ball might phase through walls
    b2BodyId ballBody = b2CreateBody(world, &ballDef);

    float ballRadiusPx = 15.0f;
    b2Circle circle = {};
    circle.center = {0.0f, 0.0f};
    circle.radius = ballRadiusPx / SCALE;

    b2ShapeDef ballShapeDef = b2DefaultShapeDef();
    ballShapeDef.density = 1.0f;
    ballShapeDef.material.friction = 0.2f;
    ballShapeDef.material.restitution = 0.9f; // bouncy ball
    b2CreateCircleShape(ballBody, &ballShapeDef, &circle);

    b2Body_SetLinearDamping(ballBody, 0.5f); // linear resistance (like air resistance)
    b2Body_SetAngularDamping(ballBody, 0.5f);

    // use SFML to draw the ball
    sf::CircleShape ballShape(ballRadiusPx);
    ballShape.setOrigin(sf::Vector2f(ballRadiusPx, ballRadiusPx)); // center-based origin
    ballShape.setFillColor(sf::Color::White); // change color of ball
    ballShape.setOutlineThickness(2.0f);
    ballShape.setOutlineColor(sf::Color::Black);

    // arrow key inputs & acceleration
    float upHold = 0.0f;
    float downHold = 0.0f;
    float leftHold = 0.0f;
    float rightHold = 0.0f;

    const float BASE_ACC = 25.0f; // m/s^2
    const float HOLD_MAX = 1.5f; // set a max to scale acceleration

    // fixed timestep to increase acceleration
    const float timeStep = 1.0f / 60.0f;
    const int subStepCount = 4; // Box2D 3.x uses sub-steps instead of vel/pos iters

    sf::Clock clock;
    float accumulator = 0.0f;

    while(window.isOpen())
    {
        while(true)
        {
            const std::optional event = window.pollEvent();
            if(!event){
                break;
            }

            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        float frameTime = clock.restart().asSeconds();
        accumulator += frameTime;

        // update <key>Hold values based on real (frameTime)
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
            upHold += frameTime;
        }else{
            upHold = 0.0f;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            downHold += frameTime;
        }else{
            downHold = 0.0f;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            leftHold += frameTime;
        }else{
            leftHold = 0.0f;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            rightHold += frameTime;
        }else{
            rightHold = 0.0f;
        }

        // make sure acceleration doesn't increase forever
        upHold = std::min(upHold, HOLD_MAX);
        downHold = std::min(downHold, HOLD_MAX);
        leftHold = std::min(leftHold, HOLD_MAX);
        rightHold = std::min(rightHold, HOLD_MAX);

        // PHYSICS update in fixed steps/frames
        while(accumulator >= timeStep)
        {
            // get accelerations based on key & time held
            float accX = 0.0f;
            float accY = 0.0f;

            if(upHold > 0.0f){
                accY -= BASE_ACC * (1.0f + upHold);
            }
            if(downHold > 0.0f){
                accY += BASE_ACC * (1.0f + downHold);
            }
            if(leftHold > 0.0f){
                accX -= BASE_ACC * (1.0f + leftHold);
            }
            if(rightHold > 0.0f){
                accX += BASE_ACC * (1.0f + rightHold);
            }

            // accel -> force: F = m * a
            float mass = b2Body_GetMass(ballBody);
            b2Vec2 force = {accX * mass, accY * mass};
            b2Body_ApplyForceToCenter(ballBody, force, true);

            b2World_Step(world, timeStep, subStepCount);
            accumulator -= timeStep;
        }

        // RENDER (SFML)
        window.clear(sf::Color(0, 128, 0));

        // update SFML ball shape from Box2D pos
        b2Vec2 pos = b2Body_GetPosition(ballBody);
        ballShape.setPosition(sf::Vector2f(pos.x * SCALE, pos.y * SCALE));

        window.draw(ballShape);

        // draw walls
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color(188, 74, 60));
        // top
        rect.setSize(sf::Vector2f(WINDOW_W, wallThickness));
        rect.setPosition(sf::Vector2f(0.0f, 0.0f));
        window.draw(rect);
        // bottom
        rect.setPosition(sf::Vector2f(0.0f, WINDOW_H - wallThickness));
        window.draw(rect);
        // left
        rect.setSize(sf::Vector2f(wallThickness, WINDOW_H));
        rect.setPosition(sf::Vector2f(0.0f, 0.0f));
        window.draw(rect);
        // right
        rect.setPosition(sf::Vector2f(WINDOW_W - wallThickness, 0.0f));
        window.draw(rect);

        window.display();
    }

    b2DestroyWorld(world);
    return 0;
}
