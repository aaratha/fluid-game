#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>

#include "physics.hpp"
#include "player.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "utils.hpp"

Parameters gravParams = {.screenWidth = 1280,
                         .screenHeight = 720,
                         .particleCount = 4000,
                         .particleRadius = 2.0f,
                         .collisionDamping = 0.5f,
                         .friction = 1.0f,
                         .gravity = 1000.0,
                         .smoothingMultiplier = 9.0f,
                         .substeps = 8,
                         .targetDensity = 24.0f,
                         .pressureMultiplier = 800000.0f,
                         .maxVelocity = 1000.0f,
                         .nearPressureMultiplier = -40000.0,
                         .viscosity = 200.0,
                         .maxAcceleration = 67.0f,
                         .mass = 100000.0f,
                         .mouseRadius = 100.0,
                         .mouseStrength = 40000.0};

Parameters zeroGravParams = {.screenWidth = 1280,
                             .screenHeight = 720,
                             .particleCount = 4000,
                             .particleRadius = 2.9f,
                             .collisionDamping = 0.5f,
                             .friction = 1.0f,
                             .gravity = 0.0,
                             .smoothingMultiplier = 9.1f,
                             .substeps = 8,
                             .targetDensity = 4.5f,
                             .pressureMultiplier = 1240000.0f,
                             .maxVelocity = 1000.0f,
                             .nearPressureMultiplier = -40000.0,
                             .viscosity = 150.0,
                             .maxAcceleration = 67.0f,
                             .mass = 100000.0f,
                             .mouseRadius = 100.0,
                             .mouseStrength = 20000.0};

/// Set Parameters
Parameters params = zeroGravParams;

float floatWidth = static_cast<float>(params.screenWidth);
float floatHeight = static_cast<float>(params.screenHeight);

float centerX = floatWidth / 2;
float centerY = floatHeight / 2;

std::vector<Obstacle> NoObstacles{};

std::vector<Obstacle> TestSceneObstacles{
    Obstacle(vec2(centerX, centerY), EXT_CIRCLE, 1.0, Rectangle{}),
    Obstacle(vec2(centerX + 400, centerY), EXT_CIRCLE, 50.0, Rectangle{}),
    Obstacle(vec2(centerX, centerY), EXT_CIRCLE, 20.0, Rectangle{})
    // Obstacle(vec2(0, 0), EXT_RECTANGLE, 0.0, Rectangle{centerX / 2, centerY /
    // 2, 200.0, 200.0})
};

std::vector<Obstacle> PlayerObstacle{
    Obstacle(vec2(centerX, centerY), EXT_CIRCLE, 0.0, Rectangle{})};

void TestSceneSchedule(std::vector<Obstacle>& obstacles, float dt) {
    obstacles[0].lerpRadius(200.0, 1, dt);
    obstacles[1].setPos(
        vec2(centerX + 420 * sin(GetTime()), centerY + 70 * cos(GetTime())));
    obstacles[1].setRadius(50 + 30 * cos(GetTime()));
    obstacles[2].setPos(vec2(centerX + 260 * sin(GetTime() * 8),
                             centerY + 260 * cos(GetTime() * 8)));
}

void NoObstaclesSchedule(std::vector<Obstacle>& obstacles, float dt) {}

Player player =
    Player(vec2(centerX, centerY), vec2(centerX, centerY), 0.05, 0.0);

void PlayerObstacleSchedule(std::vector<Obstacle>& obstacles, float dt) {
    float targRadius = 80.0;
    obstacles[0].setPos(player.getPos());
    if (player.getRadius() < targRadius) {
        player.setRadius(lerp1D(player.getRadius(), targRadius, 0.05));
        obstacles[0].setRadius(player.getRadius());
    }
}

/// Set Obstacles
std::vector<Obstacle> Obstacles = PlayerObstacle;

struct SliderInfo {
    const char* label;  // Display label
    float* value;       // Reference to the parameter
    float min;          // Minimum value
    float max;          // Maximum value
    float yOffset;      // Vertical offset for placement
};

void createSliders(Parameters& params) {
    SliderInfo sliders[] = {
        {"smoothing radius: ", &params.smoothingMultiplier, 1, 50, 40},
        {"pressure multiplier: ", &params.pressureMultiplier, 0.1f, 10000000,
         70},
        {"max acceleration: ", &params.maxAcceleration, 0, 100, 100},
        {"target density: ", &params.targetDensity, 0.0f, 50, 130},
        {"radius: ", &params.particleRadius, 0, 20, 160},
        {"viscosity: ", &params.viscosity, 0, 300, 190},
        {"gravity: ", &params.gravity, -1000, 1000, 220},
        {"near pressure: ", &params.nearPressureMultiplier, -100000, 100000,
         250}};

    // Loop through the sliders and create each one dynamically
    for (const auto& slider : sliders) {
        std::string labelText = slider.label + std::to_string(*slider.value);
        const char* label = labelText.c_str();

        GuiSliderBar((Rectangle){10, slider.yOffset, 120, 20}, NULL, label,
                     slider.value, slider.min, slider.max);
    }
}

int main(void) {
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = params.screenWidth;
    const int screenHeight = params.screenHeight;

    // SetConfigFlags(FLAG_WINDOW_TRANSPARENT); // set window to transparent
    rl::Window window(screenWidth, screenHeight, "SPH Fluid Simulation");

    // Initialize solver and particles
    Solver solver;
    solver.obstacles = Obstacles;
    solver.initializeCache(params.particleCount);

    // Initialize particle positions in a grid
    int gridCols = static_cast<int>(sqrt(params.particleCount));
    int gridRows = params.particleCount / gridCols +
                   (params.particleCount % gridCols > 0 ? 1 : 0);

    float gridSpacing =
        params.particleRadius * 2.0f;  // Spacing based on particle radius
    float startX =
        (screenWidth - gridCols * gridSpacing) / 2.0f;  // Center horizontally
    float startY =
        (screenHeight - gridRows * gridSpacing) / 2.0f;  // Center vertically

    for (int i = 0; i < params.particleCount; ++i) {
        int row = i / gridCols;
        int col = i % gridCols;

        vec2 gridPos =
            vec2{startX + col * gridSpacing, startY + row * gridSpacing};
        solver.positions[i] = gridPos;
        solver.predictedPositions[i] = gridPos;
    }

    bool pause = false;  // Movement pause

    SetTargetFPS(120);  // Set our game to run at 120 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose()) {  // Detect window close button or ESC key
        // Update
        //-----------------------------------------------------
        // Pause handling
        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        }

        if (!pause) {
            float dt = GetFrameTime();
            solver.update(dt, params);
            // solver.obstacles[0].radius += dt * 10;
            // TestSceneSchedule(solver.obstacles, dt);
            // NoObstaclesSchedule(solver.obstacles, dt);
            PlayerObstacleSchedule(solver.obstacles, dt);
            player.update(dt, params);
        }

        // Rendering
        //-----------------------------------------------------
        window.ClearBackground(BLANK);

        BeginDrawing();

        for (int i = 0; i < params.particleCount; i++) {
            DrawCircleV(solver.positions[i], params.particleRadius,
                        solver.colors[i]);
        }

        DrawCircle(GetMouseX(), GetMouseY(),
                   params.smoothingMultiplier * params.particleRadius,
                   (Color){0, 255, 0, 100});

        DrawCircleLines(GetMouseX(), GetMouseY(), params.mouseRadius, GREEN);

        DrawFPS(10, 10);

        // createSliders(params);

        EndDrawing();
    }

    return 0;
}
