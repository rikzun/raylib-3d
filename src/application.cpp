#include <application.h>
#include <algorithm>
#include <iostream>
#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>
#include <raymath.h>

#define MOUSE_SENSIVITY 0.1f
#define BLOCKS 20

Shader shader;
Light light;

Application::Application()
{
    InitWindow(800, 600, "raylib 3d");
    DisableCursor();
    SetConfigFlags(FLAG_MSAA_4X_HINT); 

    m_camera.position = { 1.0f, 2.0f, 1.0f };
    m_camera.target = { 1.0f, 2.0f, 0.0f };
    m_camera.up = { 0.0f, 1.0f, 0.0f };
    m_camera.fovy = 60.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;

    shader = LoadShader("assets/lighting.vs", "assets/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);

    light = CreateLight(LIGHT_DIRECTIONAL, { 0, 40, 0 }, { 0, 30, 0 }, YELLOW, shader);

    Mesh grassMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    m_grass_model = LoadModelFromMesh(grassMesh);

    Texture2D grassTexture = LoadTexture("assets/grass.png"); 
    m_grass_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;
    m_grass_model.materials[0].shader = shader;

    for (int i = 0; i < BLOCKS; i++)
    {
        for (int ii = 0; ii < BLOCKS; ii++)
        {
            blocks.push_back({ i, 0, ii });
        }
    }
}

void Application::loop()
{
    while (!WindowShouldClose())
    {
        ProcessInput();

        BeginDrawing();
            BeginMode3D(m_camera);
                Render3D();
            EndMode3D();

            Render2D();
        EndDrawing();
    }
}

void Application::ProcessInput()
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

void Application::Render3D()
{
    // fix that
    light.position.x = light.position.x * (float)sin(PI * GetTime());
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &m_camera.position, SHADER_UNIFORM_VEC3);
    UpdateLightValues(shader, light);

    ClearBackground(SKYBLUE);
    BeginShaderMode(shader);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        DrawSphereEx(light.position, 1.0f, 8, 8, light.color);
    }

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

void Application::Render2D()
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