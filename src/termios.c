#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_t;

void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_t);
}

void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_t);
    atexit(disable_raw_mode);
    struct termios raw = orig_t;
    raw.c_lflag &= ~(tcflag_t)(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static const char *clear_screen = "\033[2J";
static const char *bg_blue = "\033[44m";
static const char *fg_white = "\033[97m";
static const char *reset = "\033[0m";
static const char *hide_cursor = "\033[?25l";
static const char *show_cursor = "\033[?25h";
static const char *cursor_home = "\033[H";

void move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

void draw_window(int row, int col, int width, int height, const char *title) {
    fputs(bg_blue, stdout);
    fputs(fg_white, stdout);
    move_cursor(row, col);
    fputs("┌", stdout);
    for (int i = 0; i < width - 2; ++i)
        fputs("─", stdout);
    fputs("┐", stdout);
    if (title != nullptr && strlen(title) > 0) {
        move_cursor(row, col + 2);
        printf(" %s ", title);
    }
    for (int r = 1; r < height - 1; ++r) {
        move_cursor(row + r, col);
        fputs("│", stdout);
        for (int i = 0; i < width - 2; ++i)
            fputs(" ", stdout);
        fputs("│", stdout);
    }
    move_cursor(row + height - 1, col);
    fputs("└", stdout);
    for (int i = 0; i < width - 2; ++i)
        fputs("─", stdout);
    fputs("┘", stdout);
    fflush(stdout);
}

int main(void) {
    enable_raw_mode();
    fputs(clear_screen, stdout);
    fputs(hide_cursor, stdout);
    draw_window(5, 10, 50, 12, "Configuration Menu");
    move_cursor(7, 12);
    fputs("Welcome to the custom terminal window.", stdout);
    move_cursor(9, 12);
    fputs("Press 'q' to quit...", stdout);
    fflush(stdout);
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 'q') {
            break;
        }
    }
    fputs(reset, stdout);
    fputs(clear_screen, stdout);
    fputs(cursor_home, stdout);
    fputs(show_cursor, stdout);
    fflush(stdout);

    return 0;
}
