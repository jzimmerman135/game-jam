#ifndef MORHPEUS_H
#define MORHPEUS_H

#include <stdbool.h>

typedef struct {
    int statement_id;
    int last_said;
    bool shutup;
    bool fleo_is_ready;
} Morpheus;

void update_morpheus(Morpheus *morpheus);

void draw_secret_message(Morpheus *morpheus, float elapsed);

#endif
