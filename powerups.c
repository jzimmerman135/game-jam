#include "powerups.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

void place_powerup(Powerups *powerups, Settings *settings, Vector2 pos) {
    // TODO: use circular buffer
    if (powerups->nPowerups > MAX_POWERUPS) {
        fprintf(stderr, "uh oh too many powerups\n");
        exit(1);
    }
    assert(settings->api_version != LOAD_NEW_API);
    powerups->powerups[powerups->nPowerups].position = pos;
    powerups->powerups[powerups->nPowerups].api_version_id = settings->api_version;
    powerups->nPowerups++;

}
