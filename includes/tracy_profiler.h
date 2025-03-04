#pragma once

class TracyProfiler
{
    public:
        TracyProfiler();
        ~TracyProfiler();
};

#define TracyProfilerLaunch TracyProfiler* tracyProfilerInstance = new TracyProfiler();
#define TracyProfilerClose delete tracyProfilerInstance;