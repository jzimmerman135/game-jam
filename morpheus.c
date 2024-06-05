#include "morpheus.h"
#include "raylib.h"
#include <stdio.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const char cowpheus[] = "\n\n\nmorpheus: moo! (hit any key)";
const char bluepill[] = "bluepill";

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
                printf("%s\n", cowpheus);
                printf("hit any key to continue\n");
                morpheus->last_said = morpheus->statement_id;
                break;
            case 1:
                printf("%s\n", bluepill);
                printf("respond to continue\n");
                morpheus->last_said = morpheus->statement_id;
                break;
            case 2:
                printf("I've added some secret controls -- THESE ARE FUCKIN IMPORTANT READ 'EM\n");
                printf("Hit 'E' to open the source code\n");
                printf("When you've finished recompile with 'make'\n");
                printf("You'll see a pill in the game, take it with 'F'\n");
                morpheus->last_said = morpheus->statement_id;
                break;
            default:
                break;
        }
    }
}

void draw_secret_message(Morpheus *morpheus, float elapsed) {
    if (morpheus->shutup)
        return;

    if (morpheus->last_said <= 0 && elapsed > 2.0) {
        const char *msg ="Psst! Hit P and Check your terminal!"; 
        int padding = 10;
        Color fgcolor = (Color){0x4e, 0xDc, 0x4e, 0xff};
        DrawRectangle(0, 0, MeasureText(msg, 30) + padding*2, 30 + padding*2, BLACK);
        DrawText(msg, padding, padding, 30, fgcolor);
        morpheus->statement_id = 0;
    }
}
