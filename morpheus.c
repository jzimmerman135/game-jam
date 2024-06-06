#include "morpheus.h"
#include "morpheus_intro.txt.h"
#include "raylib.h"
#include <stdio.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const char redpill[] = "\nThis game is a digital prison to silo unsafe programmers, but I've hacked in\n"
"and I can help you escape.";

void set_nonblocking_mode(int enable) {
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);
    if (enable) {
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_cc[VMIN] = 1;
    } else {
        ttystate.c_lflag |= ICANON;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void set_blocking_mode(int enable) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (enable) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

void update_morpheus(Morpheus *morpheus)
{
    if (morpheus->shutup)
        return;

    set_nonblocking_mode(1);
    set_blocking_mode(0);

    char ch = getchar();
    if (ch != EOF) {
        morpheus->statement_id++;
    }

    set_nonblocking_mode(0);
    set_blocking_mode(1);

    if (morpheus->last_said < morpheus->statement_id) {
        switch (morpheus->statement_id) {
            case 0:
                printf("\33[0;100");
                printf("\33[0;32m");
                fwrite(morpheus_intro_txt, 1, morpheus_intro_txt_len, stdout);
                printf("Crtl-c to take the \33[0;34mblue pill\n");
                printf("\33[0;32m");
                printf("Hit any key to take the \33[0;31mred pill\n");
                printf("\33[0m");
                morpheus->last_said = morpheus->statement_id;
                break;
            case 1:
                printf("\33[0;100");
                printf("\33[0;32m");
                printf("%s\n", redpill);
                printf("\33[0m");
                printf("\33[0;36mrespond to continue\n");
                morpheus->last_said = morpheus->statement_id;
                break;
            case 2:
                printf("\33[0;100");
                printf("\33[0;32m");
                printf("Very good\n");
                printf("I've added some secret controls -- \33[0;31mTHESE ARE IMPORTANT, READ 'EM\n");
                printf("\33[0m");
                printf("\33[0;100");
                printf("\33[0;34m");
                printf("===============================================\n");
                printf("Inside the game, hit 'E' to open the source code\n");
                printf("When you've finished recompile with 'make'\n");
                printf("You'll see a pill in the game, take it with 'F'\n");
                printf("Remove pills from the board by toggling 'D'\n");
                printf("===============================================\n");
                printf("\33[0m");
                morpheus->last_said = morpheus->statement_id;
                break;
            case 3:
                morpheus->shutup = true;
            default:
                break;
        }
    }
}

void draw_secret_message(Morpheus *morpheus, float elapsed) {
    if (morpheus->shutup)
        return;

    if (morpheus->statement_id == 0) {
        const char *msg ="Psst! Hit P and check the terminal!";
        int padding = 10;
        Color fgcolor = (Color){0x4e, 0xDc, 0x4e, 0xee};
        Rectangle rect = {padding, padding, MeasureText(msg, 30) + padding*2, 30 + padding*2};

        // Triangle
        Vector2 lb = {0, 40 + padding * 2};
        Vector2 rb = {padding * 3, 40 + padding * 2};
        Vector2 rt = {padding * 1.5, 30 + padding * 2 };
        DrawTriangle(lb, rb, rt, ColorAlpha(DARKGRAY, 1.0));
        DrawRectangleRounded(rect, 20.0, 15, ColorAlpha(DARKGRAY, 1.0));
        DrawText(msg, padding * 2, padding * 2, 30, fgcolor);
        morpheus->statement_id = 0;
    }
}
