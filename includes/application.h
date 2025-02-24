#pragma once
#include <raylib.h>
#include <vector>

typedef struct Block {
    int x;
    int y;
    int z;
} Block;

class Application
{
    public:
        Application();
        ~Application();

        void loop();
        
    private:
        Camera m_camera = { 0 };
        float m_camera_speed = 0.2f;
        Model m_grass_model;

        std::vector<Block> blocks;

        bool m_f1 = false;
        bool m_f2 = false;

        void ProcessInput();
        void Render3D();
        void Render2D();
};