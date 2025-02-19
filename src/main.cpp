#include "raylib.h"
#include "rcamera.h"

#define PLAYER_WALK_SPEED 0.15f
#define PLAYER_RUN_SPEED 0.3f
#define PLAYER_VERT_SPEED 0.2f
#define MOUSE_SENSIVITY 0.1f
#define MAX_COLUMNS 20

int main()
{
    InitWindow(800, 600, "raylib 3d");

    Camera camera = { 0 };
    camera.position = { 1.0f, 2.0f, 1.0f };
    camera.target = { 1.0f, 2.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = { (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = { (unsigned char)GetRandomValue(20, 255), (unsigned char)GetRandomValue(10, 55), 30, 255 };
    }

    DisableCursor();
    SetTargetFPS(60);

    bool info = false;

    while (!WindowShouldClose()) {
        float speed = IsKeyDown(KEY_LEFT_SHIFT)
            ? PLAYER_RUN_SPEED
            : PLAYER_WALK_SPEED;

        UpdateCameraPro(&camera,
            {
                IsKeyDown(KEY_W) * speed - IsKeyDown(KEY_S) * speed,
                IsKeyDown(KEY_D) * speed - IsKeyDown(KEY_A) * speed,
                IsKeyDown(KEY_Q) * -PLAYER_VERT_SPEED +
                IsKeyDown(KEY_E) * PLAYER_VERT_SPEED +
                (GetMouseWheelMove() / 2)
            },
            {
                GetMouseDelta().x * MOUSE_SENSIVITY,
                GetMouseDelta().y * MOUSE_SENSIVITY,
                0.0f
            },
            0
        );

        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode3D(camera);

        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f }, LIGHTGRAY);
        for (int i = 0; i < MAX_COLUMNS; i++)
        {
            DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
            DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
        }

        EndMode3D();

        if (IsKeyReleased(KEY_F1)) {
            info = !info;
        }
        
        if (info) {
            DrawText(TextFormat("fps: %i", GetFPS()), 1, 1, 20, BLACK);
            DrawText(TextFormat("frame: %i", GetFrameTime()), 1, 21, 20, BLACK);
            DrawText(TextFormat("pos_x: %.2f", camera.position.x), 1, 41, 20, BLACK);
            DrawText(TextFormat("pos_y: %.2f", camera.position.y), 1, 61, 20, BLACK);
            DrawText(TextFormat("pos_z: %.2f", camera.position.z), 1, 81, 20, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}