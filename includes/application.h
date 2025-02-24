#pragma once
#include <raylib.h>

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

        bool m_f1 = false;
        bool m_f2 = false;

        void ProcessInput();
        void Render3D();
        void Render2D();
};