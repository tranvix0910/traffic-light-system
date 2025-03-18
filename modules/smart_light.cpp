#include "smart_light.h"
#include "auto_mode.h"

void smart_light_init()
{
    auto_mode_init();
}

void smart_light_update()
{
    auto_mode_update();
    delay(SYSTEM_TIME_INCREMENT_MS);
}
