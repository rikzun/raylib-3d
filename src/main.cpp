#include "application.h"
#include <Remotery.h>

int main()
{
    Remotery* rmt;
    rmt_CreateGlobalInstance(&rmt);

    Application app;
    app.loop();

    return 0;
}