#include "powerups.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void place_powerup(Powerups *powerups, Vector2 pos, int id) {
    if (powerups->nPowerups > MAX_POWERUPS) {
        powerups->powerup[0] = powerups->powerup[--powerups->nPowerups];
    }
    powerups->powerup[powerups->nPowerups].position = pos;
    powerups->powerup[powerups->nPowerups].api_version_id = id;
    powerups->powerup[powerups->nPowerups].color = ORANGE;
    powerups->nPowerups++;
}

void destroy_powerup(Powerups *powerups, int id) {
    for (int i = 0; i < powerups->nPowerups % MAX_POWERUPS; i++) {
        if (powerups->powerup[i].api_version_id == id) {
            powerups->powerup[i] = powerups->powerup[--powerups->nPowerups];
        }
    }
}

char *decode_fileid(src_file_id fileid) {
    switch (fileid) {
        case FIRST_FILE:
            return "api.c";
        default:
            return NULL;
    }
}

bool try_open_text_editor(char *filename) {
    if (filename == NULL)
        return true;

    int pid = fork();
    if (pid == 0) {
        execlp(getenv("EDITOR"), getenv("EDITOR"), filename, NULL);
        perror("execlp");
        exit(1);
    }

    return true;
}

void draw_powerups(Powerups *powerups) {
    for (int i = 0; i < powerups->nPowerups % MAX_POWERUPS; i++) {
        Powerup p = powerups->powerup[i];
        DrawCircleV(p.position, powerups->radius, p.color);
    }
}
