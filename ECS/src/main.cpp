/*
** EPITECH PROJECT, 2025
** main.cpp
*/

#include "raylib.h"
#include "ECS/include/registry.hpp"
#include "ECS/include/Component.hpp"
#include "ECS/include/system.hpp"
#include <random>

void create_entity(
    Registry &reg, 
    float posX, 
    float posY, 
    float velX, 
    float velY, 
    Color color, 
    float size
) {
    Entity entity = reg.spawn_entity();
    
    reg.add_component<Position>(entity, {posX, posY});
    reg.add_component<velocity>(entity, {velX, velY});
    reg.add_component<Comp>(entity, {color, size});
}

int main() {
    Registry reg;
    Systems systems;

    InitWindow(1920, 1080, "R-Type tesste");
    SetTargetFPS(60);

    reg.register_component<Position>();
    reg.register_component<velocity>();
    reg.register_component<Comp>();

    create_entity(reg, 
        100.0f, 100.0f,
        1.0f, 0.5f,
        BLUE,
        70.0f
    );

    create_entity(reg, 
        700.0f, 500.0f,
        -3.0f, -2.0f,
        RED,
        30.0f
    );
    
    create_entity(reg, 
        400.0f, 300.0f, 
        0.0f, 0.0f,
        GRAY, 
        150.0f
    );
    
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> pos_dist(0.0f, 800.0f);
    std::uniform_real_distribution<float> vel_dist(-5.0f, 5.0f);

    for (int i = 0; i < 50; ++i) {
        create_entity(reg, 
            pos_dist(rng), 
            pos_dist(rng), 
            vel_dist(rng) * 0.5f, // Vitesse réduite
            vel_dist(rng) * 0.5f, 
            GREEN, 
            10.0f
        );
    }
    
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        systems.movement(reg, deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE); 
        
        systems.rendering(reg);
        systems.logging(reg);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}