#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

// Function to set the terminal to non-canonical mode
void set_nonblocking_mode(int enable) {
    struct termios ttystate;

    // Get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        // Turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        // Minimum number of characters for non-canonical read
        ttystate.c_cc[VMIN] = 1;
    } else {
        // Restore the original mode
        ttystate.c_lflag |= ICANON;
    }

    // Set the terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Function to set the terminal to non-blocking mode
void set_blocking_mode(int enable) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (enable) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

int main() {
    // Set terminal to non-canonical and non-blocking mode

    printf("Press any key to continue...\n");

    int ch;
    while (1) {
        set_nonblocking_mode(1);
        set_blocking_mode(0);

        // Read character from stdin
        ch = getchar();
        if (ch != EOF) {
            // If a key is pressed, break the loop
            break;
        }

        printf("nothign\n");

        // Sleep for a short period to reduce CPU usage
        usleep(10000); // 10 milliseconds

        set_nonblocking_mode(0);
        set_blocking_mode(1);
    }

    printf("Key pressed! Exiting...\n");

    // Restore terminal to canonical and blocking mode
    set_nonblocking_mode(0);
    set_blocking_mode(1);

    return 0;
}
