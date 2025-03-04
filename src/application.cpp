#include "application.h"
#include <algorithm>
#include <iostream>
#include "rlights.h"
#include <raymath.h>
#include <random>
#include <rlgl.h>
#include <TracyC.h>
#include <Tracy.hpp>

#define MOUSE_SENSIVITY 0.1f
#define BLOCKS 16

Shader shader;
Light light;
Model sun;

std::vector<float> GenerateHeightMap(int seed) {
    std::vector<float> heightMap(BLOCKS * BLOCKS);
    std::mt19937 localGen(seed);
    std::uniform_real_distribution<float> noiseDist(-0.5f, 0.5f);
    
    for (int x = 0; x < BLOCKS; ++x) {
        float baseHeight = 10.0f + 5.0f * std::sin(x * 0.3f) + 3.0f * std::sin(x * 0.1f);
        for (int y = 0; y < BLOCKS; ++y) {
            float variation = noiseDist(localGen);
            heightMap[y + x] = std::round(baseHeight + variation);
        }
    }
    return heightMap;
}

Application::Application()
{
    InitWindow(800, 600, "raylib 3d");
    DisableCursor();
    SetConfigFlags(FLAG_MSAA_4X_HINT); 
    SetTargetFPS(60);

    m_camera.position = { 1.0f, 25.0f, 1.0f };
    m_camera.target = { 1.0f, 2.0f, 0.0f };
    m_camera.up = { 0.0f, 1.0f, 0.0f };
    m_camera.fovy = 60.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;

    shader = LoadShader("assets/lighting.vs", "assets/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    float a[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    
    SetShaderValue(shader, ambientLoc, a, SHADER_UNIFORM_VEC4);

    light = CreateLight(LIGHT_DIRECTIONAL, { 5, 20, 5 }, { 0, 0, 0 }, YELLOW, shader);

    Mesh sphere = GenMeshSphere(1.0f, 16, 16);
    sun = LoadModelFromMesh(sphere);
    Texture2D sunTexture = LoadTexture("assets/sun.png"); 
    sun.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sunTexture;

    Mesh grassMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    m_grass_model = LoadModelFromMesh(grassMesh);

    Texture2D grassTexture = LoadTexture("assets/grass.png"); 
    m_grass_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;
    m_grass_model.materials[0].shader = shader;

    auto heighMap = GenerateHeightMap(1337);
    for (int i = 0; i < BLOCKS; i++) {
        for (int ii = 0; ii < BLOCKS; ii++)
        {
            int y = (int)heighMap[i + ii];

            for (int iii = y; iii > 0; iii--)
            {
                blocks.push_back(Block{ i, iii, ii });
                blocks.push_back(Block{ -i, iii, ii });
                blocks.push_back(Block{ i, iii, -ii });
                blocks.push_back(Block{ -i, iii, -ii });
            }
        }
    }
}

void Application::loop()
{
    while (!WindowShouldClose())
    {
        TracyCZoneNC(mlctx, "MainLoop", tracy::Color::DarkGray, 1);

        TracyCZoneNC(pictx, "Process Input", tracy::Color::Aqua, 1);
        processInput();
        TracyCZoneEnd(pictx);

        TracyCZoneNC(bdctx, "Begin Drawing", tracy::Color::Brown, 1);
        BeginDrawing();
            TracyCZoneNC(bm3dctx, "Begin Mode 3D", tracy::Color::DarkMagenta, 1);
            BeginMode3D(m_camera);
                TracyCZoneNC(r3dctx, "Render 3D", tracy::Color::Green, 1);
                render3D();
                TracyCZoneEnd(r3dctx);
            EndMode3D();
            TracyCZoneEnd(bm3dctx);

            TracyCZoneNC(r2dctx, "Render 2D", tracy::Color::Yellow, 1);
            render2D();
            TracyCZoneEnd(r2dctx);
        EndDrawing();
        TracyCZoneEnd(bdctx);

        TracyCZoneEnd(mlctx);
        TracyCFrameMark;
    }
}

void Application::processInput()
{
    m_camera_speed = std::clamp(m_camera_speed + GetMouseWheelMove() / 100, 0.01f, 1.0f);
    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) m_camera_speed = 0.2f;

    float deltaFactor = GetFrameTime() * 50;
    float speed = m_camera_speed * deltaFactor;

    UpdateCameraPro(&m_camera,
        {
            IsKeyDown(KEY_W) * speed - IsKeyDown(KEY_S) * speed,
            IsKeyDown(KEY_D) * speed - IsKeyDown(KEY_A) * speed,
            IsKeyDown(KEY_SPACE) * speed - IsKeyDown(KEY_LEFT_SHIFT) * speed
        },
        {
            GetMouseDelta().x * MOUSE_SENSIVITY,
            GetMouseDelta().y * MOUSE_SENSIVITY,
            0.0f
        },
        0
    );

    if (IsKeyReleased(KEY_F1)) m_f1 = !m_f1;
    if (IsKeyReleased(KEY_F2)) m_f2 = !m_f2;
}

void Application::render3D()
{
    light.position.x = (light.position.x || 0.1f) * (float)sin(PI * GetTime() / 6.0f) * 25.0f;
    light.position.z = (light.position.z || 0.1f) * (float)cos(PI * GetTime() / 6.0f) * 25.0f;

    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &m_camera.position, SHADER_UNIFORM_VEC3);
    UpdateLightValues(shader, light);

    ClearBackground(SKYBLUE);
    BeginShaderMode(shader);

    //DrawSphereEx(light.position, 1.0f, 8, 8, light.color);
    DrawModel(sun, light.position, 1.0f, WHITE);


    for (int i = 0; i < blocks.size(); i++)
    {
        Block block = blocks[i];
        Vector3 pos = { block.x + 0.5f, block.y + 0.5f, block.z + 0.5f };

        DrawModel(m_grass_model, pos, 1.0f, WHITE);
        if (m_f2) DrawModelWires(m_grass_model, pos, 1.0f, BLACK);
    }

    EndShaderMode();

    if (m_f1) DrawGrid(100, 1.0f);
}

void Application::render2D()
{
    if (m_f1)
    {
        DrawText(TextFormat("time: %.1f", GetTime()), 1, 1, 20, BLACK);
        DrawText(TextFormat("fps: %i", GetFPS()), 1, 21, 20, BLACK);
        DrawText(TextFormat("pos_x: % .2f", m_camera.position.x), 1, 41, 20, BLACK);
        DrawText(TextFormat("pos_y: % .2f", m_camera.position.y), 1, 61, 20, BLACK);
        DrawText(TextFormat("pos_z: % .2f", m_camera.position.z), 1, 81, 20, BLACK);
        DrawText(TextFormat("speed: %.2f", m_camera_speed), 1, 101, 20, BLACK);
    }
}

Application::~Application()
{
    CloseWindow();
}

void UpdateMesh(Model *model)
{
    UpdateMeshBuffer(
        model->meshes[0],
        1,
        model->meshes->texcoords,
        model->meshes->vertexCount * 2 * sizeof(float),
        0
    );
}