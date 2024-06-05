#include <stdio.h>
#include <math.h>
#include "map.h"
#include "lib/cJSON/cJSON.h"
#include "palette.h"
#include <stdlib.h>

static void load_tube(const cJSON *data, Tubes *tb, float *xpos)
{
    const cJSON *width, *height, *move, *ypos, *leftpad, *type;
    float xoff;

    width = cJSON_GetObjectItemCaseSensitive(data, "width");
    height = cJSON_GetObjectItemCaseSensitive(data, "height");
    move = cJSON_GetObjectItemCaseSensitive(data, "move");
    ypos = cJSON_GetObjectItemCaseSensitive(data, "ypos");
    leftpad = cJSON_GetObjectItemCaseSensitive(data, "leftpad");
    type = cJSON_GetObjectItemCaseSensitive(data, "type");

    xoff = 0;
    xoff = move->valuedouble;
    tb->rec.x = *xpos;

    if (leftpad != NULL) {
        tb->rec.x += leftpad->valuedouble;
        xoff += leftpad->valuedouble;
    }

    tb->rec.y = ypos->valuedouble;
    tb->rec.width = width->valuedouble;
    tb->rec.height = height->valuedouble;
    tb->color = (Color){0, 0, 0, 255};
    tb->type = floor(type->valuedouble);
    *xpos += xoff;
}

void init_map(Map *m)
{
    cJSON *json;
    char *str;
    FILE *fp;
    size_t nbytes;
    int i;
    const char *mapFilePath;

    mapFilePath = "proto/maps/01.json";
    fp = fopen(mapFilePath, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not map: %s\n", mapFilePath);
        fprintf(stderr, "Has it been generated yet?\n");
        exit(1);
    }

    i = 0;

    if (fp != NULL) {
        const cJSON *rects, *rect;
        float xpos;

        fseek(fp, 0L, SEEK_END);
        nbytes = ftell(fp);
        str = malloc(nbytes + 1);
        rewind(fp);
        fread(str, 1, nbytes, fp);
        str[nbytes] = 0;
        fclose(fp);
        json = cJSON_Parse(str);
        if (json == NULL) {
            fprintf(stderr, "!!!!invalid map data!!!!");
        }
        free(str);

        rects = cJSON_GetObjectItemCaseSensitive(json, "rects");

        i = 0;
        xpos = 0;
        cJSON_ArrayForEach(rect, rects) {
            load_tube(rect, &(m->tubes[i]), &xpos);
            i++;
        }

        cJSON_Delete(json);
        m->nTubes = i;
    }
}

void draw_map(Map *map) {
    (void)map;
    for (int i = 0; i < map->nTubes; i++) {
        DrawRectangle(map->tubes[i].rec.x + 8,
            map->tubes[i].rec.y + 8,
            map->tubes[i].rec.width,
            map->tubes[i].rec.height,
            get_color(COLOR_TUBE_SHADOW)
            );
        DrawRectangle(map->tubes[i].rec.x,
            map->tubes[i].rec.y,
            map->tubes[i].rec.width,
            map->tubes[i].rec.height,
            get_color(COLOR_TUBE_DEFAULT)
            );
    }
}

src_file_id relevant_src_file_id_from_world_pos(const Map *map, Vector2 playerpos) {
    return FIRST_FILE;
}
