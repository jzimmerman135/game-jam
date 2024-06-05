#include <stdio.h>
#include <math.h>
#include "map.h"
#include "lib/cJSON/cJSON.h"
#include "palette.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

static Vector2 scale_by(Vector2 pt, Vector2 scale, Vector2 origin);
static void load_tube(const cJSON *data, Tubes *tb, float *xpos)
{
    const cJSON *width, *height, *move, *ypos, *leftpad, *type;
    float xoff;

    width = cJSON_GetObjectItemCaseSensitive(data, "width");
    height = cJSON_GetObjectItemCaseSensitive(data, "height");
    move = cJSON_GetObjectItemCaseSensitive(data, "move");
    ypos = cJSON_GetObjectItemCaseSensitive(data, "ypos");
    leftpad = cJSON_GetObjectItemCaseSensitive(data, "leftpad");
    type = cJSON_GetObjectItemCaseSensitive(data, "types");

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
    tb->toggled = 0;
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

    mapFilePath = "proto/maps/03.json";
    fp = fopen(mapFilePath, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not map: %s\n", mapFilePath);
        fprintf(stderr, "Has it been generated yet?\n");
        exit(1);
    }
    m->visibility = 0;

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

    m->scale = (Vector2){1, 1};
}

Rectangle transform_rec(Rectangle tb_rec, Vector2 scale, Vector2 origin)
{
    Vector2 tb_xy = (Vector2){ tb_rec.x, tb_rec.y };
    Vector2 tb_yz = (Vector2){ tb_rec.x + tb_rec.width, tb_rec.y + tb_rec.height };
    tb_xy = scale_by(tb_xy, scale, origin);
    tb_yz = scale_by(tb_yz, scale, origin);
    Vector2 tb_wh = Vector2Subtract(tb_yz, tb_xy);
    return (Rectangle){
        .x = tb_xy.x,
        .y = tb_xy.y,
        .width = tb_wh.x,
        .height = tb_wh.y
    };
}

Rectangle mkshadow(Rectangle r) {
    return (Rectangle){
        .x = r.x + 8,
        .y = r.y + 8,
        .width = r.width,
        .height = r.height
    };
}

static void draw_tube(Tubes *tb, Vector2 scale, Vector2 origin, int visibility)
{
    Rectangle rec = transform_rec(tb->rec, scale, origin);
    Rectangle shadowrec = transform_rec(mkshadow(tb->rec), scale, origin);
    switch (tb->type) {
        case TUBE_DEATH: {
            DrawRectangleRec(shadowrec, get_color(COLOR_TUBE_SHADOW));
            DrawRectangleRec(rec, get_color(COLOR_TUBE_DEATH));
            break;
        }
        case TUBE_PLATFORM:
            DrawRectangleRec(shadowrec, get_color(COLOR_TUBE_SHADOW));
            DrawRectangleRec(rec, get_color(COLOR_TUBE_PLATFORM));
            break;
        case TUBE_TOGGLE:
            DrawRectangleRec(shadowrec, get_color(COLOR_TUBE_SHADOW));
            DrawRectangleRec(rec, get_color(COLOR_TUBE_BLUE));
            break;
        case TUBE_BLUE:
            if (visibility == 0) {
                DrawRectangleRec(shadowrec, get_color(COLOR_TUBE_SHADOW));
                DrawRectangleRec(rec, get_color(COLOR_TUBE_BLUE));
            }
            break;
        default:
            DrawRectangleRec(shadowrec, get_color(COLOR_TUBE_SHADOW));
            DrawRectangleRec(rec, get_color(COLOR_TUBE_DEFAULT));
            break;
    }
}

void draw_map(Map *map, Vector2 origin) {
    for (int i = 0; i < map->nTubes; i++) {
        draw_tube(&map->tubes[i], map->scale, origin, map->visibility);
    }
}

src_file_id relevant_src_file_id_from_world_pos(const Map *map, Vector2 playerpos) {
    return FIRST_FILE;
}

static Vector2 scale_by(Vector2 pt, Vector2 scale, Vector2 origin) {
    Vector2 movedpt = Vector2Subtract(pt, origin);
    return Vector2Add(Vector2Multiply(movedpt, scale), origin);
}
