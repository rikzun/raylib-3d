#include "application.h"
#include <Tracy.hpp>
#include <tracy_profiler.h>

int main()
{
    TracyProfilerLaunch;
    TracyNoop;

    Application app;
    app.loop();

    TracyProfilerClose;
    return 0;
}