#include <stdio.h>
#include "map.h"
#include "api.h"
#include "lib/cJSON/cJSON.h"

static void load_tube(const cJSON *data, Tubes *tb, float *xpos)
{
    const cJSON *width, *height, *move, *ypos, *leftpad;
    float xoff;

    width = cJSON_GetObjectItemCaseSensitive(data, "width");
    height = cJSON_GetObjectItemCaseSensitive(data, "height");
    move = cJSON_GetObjectItemCaseSensitive(data, "move");
    ypos = cJSON_GetObjectItemCaseSensitive(data, "ypos");
    leftpad = cJSON_GetObjectItemCaseSensitive(data, "leftpad");

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
    *xpos += xoff;
}

static void _load_map(game_state *gs)
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
            //gs->tubesPos[i].x = 400 + 280*i;
            //gs->tubesPos[i].y = -rect->valuedouble;
            load_tube(rect, &gs->tubes[i], &xpos);
            i++;
        }

        cJSON_Delete(json);
        gs->nTubes = i;
    }
}

void load_map(game_state *gs, char *filename)
{
    // TODO: take in filename
    _load_map(gs);
}
