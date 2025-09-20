#include <iostream>
#include "raylib.h"
#include <cmath>
#include <vector>
#include "Bullet.h"
#include "PhysicsConstants.h"
#include "RenderBullet.h"
#include "VisualUtilities.h"
#define RAYGUI_IMPLEMENTATION
#include "UI.h"

float pixelPerMeter = 5.0f;


int main() {
    InitWindow(screenHeight, screenWidth, "2D Ballistic Simulation");
    SetTargetFPS(60);

    MainUI menu;
    
    Bullet* bullet1 = new Bullet(400.0f, 45.0f, "Pistol_bullet");

    RenderObject rbullet1(bullet1);

    //std::vector<Vec2> trail;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime()*timeScale;

        HandleZoomInput(pixelPerMeter);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        //Draw graph
        BeginScissorMode(graphArea.x, 
            graphArea.y, 
            graphArea.width, 
            graphArea.height);

        if (menu.getFireRequested()) 
        {
            delete bullet1;
            bullet1 = new Bullet(menu.getUserVelocity(), menu.getUserAngle(), "UserBullet");
            rbullet1 = RenderObject(bullet1);
            menu.setFireRequested(false);
        }


        rbullet1.setPosition(deltaTime); // Trajectory 
        menu.DrawGridWithLabelsMeters(pixelPerMeter, LIGHTGRAY, DARKGRAY); //Draw grid

        DrawCircleV({ graphArea.x, graphArea.y + graphArea.height }, 5, BLUE); // (0,0)
        rbullet1.renderObject(pixelPerMeter); // Draw Bullet
        rbullet1.renderTrail(pixelPerMeter); //Draw trajectory
        EndScissorMode();
        //Finish draw graph

        menu.DrawUIPanel();
        menu.DrawAngleInput();
        menu.DrawVelocityInput();
        menu.DrawPlayButton();
        menu.DrawTimeScaleToggle(timeScale);
        menu.DrawObjectData(rbullet1);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}