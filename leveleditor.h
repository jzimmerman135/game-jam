#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "raylib.h"
#include "types.h"

typedef struct {
    int selection;
    Camera2D godcam;
    Map map;
} Leveleditor;

Map read_level(const char *filename);
bool save_level(Leveleditor *editor, const char *filename);
bool update_leveleditor(Leveleditor *editor);
void draw_leveleditor(Leveleditor *editor);

#endif
