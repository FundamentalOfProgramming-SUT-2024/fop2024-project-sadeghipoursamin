#include <ncurses.h>
#include <unistd.h>

#define DELAY 40000 

const char *ascii_art[] = {
    "                                                       (                            ",
    " (  (           (                           *   )      )\\ )                         ",
    " )\\))(   '   (  )\\             )      (   ` )  /(     (()/(      (  (     (     (   ",
    "((_)()\\ )   ))\\((_) (   (     (      ))\\   ( )(_))(    /(_)) (   )\\))(   ))\\   ))\\  ",
    "_(())\\_)() /((_)_   )\\  )\\    )\\  ' /((_) (_(_()) )\\  (_))   )\\ ((_))\\  /((_) /((_) ",
    "\\ \\((_)/ /(_)) | | ((_)((_) _((_)) (_))   |_   _|((_) | _ \\ ((_) (()(_)(_))( (_))   ",
    " \\ \\/\\/ / / -_)| |/ _|/ _ \\| '  \\()/ -_)    | | / _ \\ |   // _ \\/ _` | | || |/ -_)  ",
    "  \\_/\\_/  \\___||_|\\__|\\___/|_|_|_| \\___|    |_| \\___/ |_|_\\\\___/\\__, |  \\_,_|\\___|  ",
    "                                                                |___/                ",
    NULL
};

void draw_borders() {
    attron(COLOR_PAIR(1)); // Red
    for (int col = 0; col < COLS; col++) {
        mvaddch(0, col, '*');
        mvaddch(LINES - 1, col, '*');
    }
    for (int row = 0; row < LINES; row++) {
        mvaddch(row, 0, '*');
        mvaddch(row, COLS - 1, '*');
    }
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2)); // Yellow
    for (int col = 1; col < COLS - 1; col++) {
        mvaddch(1, col, '*');
        mvaddch(LINES - 2, col, '*');
    }
    for (int row = 1; row < LINES - 1; row++) {
        mvaddch(row, 1, '*');
        mvaddch(row, COLS - 2, '*');
    }
    attroff(COLOR_PAIR(2));
}

void draw_frame(int offset) {
    clear();
    draw_borders();
    attron(COLOR_PAIR(1));
    for (int i = 0; ascii_art[i] != NULL; i++) {
        mvprintw(i + offset, (COLS - 80) / 2, "%s", ascii_art[i]);
    }
    attroff(COLOR_PAIR(1));
    refresh();
}

void run_anime_intro() {
    
    initscr();
    for (int y = 0; y < LINES; y++) {
        for (int x = 0; x < COLS; x++) {
            mvaddch(y, x, ' ');
        }
    }
    start_color();
    curs_set(0);
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    
    init_pair(1, 196, COLOR_BLACK);
    init_pair(2, 228, COLOR_BLACK);

    clear();

    int max_offset = LINES - 10 - 2;
    int offset = max_offset;

    while (offset > 2) {
        clear();
        draw_borders();
        attron(COLOR_PAIR(1));
        for (int i = 0; ascii_art[i] != NULL; i++) {
            mvprintw(i + offset, (COLS - 80) / 2, "%s", ascii_art[i]);
        }
        attroff(COLOR_PAIR(1));
        refresh();
        offset--;
        usleep(DELAY);
    }
    
    usleep(500000);
    clear();
    refresh();
}

// int main() {
//     run_anime_intro();
//     return 0;
// }