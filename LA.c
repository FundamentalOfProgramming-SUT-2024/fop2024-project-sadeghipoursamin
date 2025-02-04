#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <locale.h>

void draw_palm_tree(WINDOW *win, int x, int y) {
    mvwprintw(win, y, x, "   ^   ");
    mvwprintw(win, y+1, x, " ^^^^^ ");
    mvwprintw(win, y+2, x, "^^^^^^^ ");
    mvwprintw(win, y+3, x, " ^^^^^ ");
    mvwprintw(win, y+4, x, "   |   ");
    mvwprintw(win, y+5, x, "   |   ");
}

void draw_building(WINDOW *win, int x, int y, int height) {
    for (int i = 0; i < height; i++) {
        mvwaddch(win, y - i, x, '|');
        mvwaddch(win, y - i, x + 4, '|');
        mvwhline(win, y - i, x + 1, '-', 3);
    }
}

void draw_star(WINDOW *win, int x, int y) {
    mvwaddch(win, y, x, '*');
}

void la_animation() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // Palm trees
    init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Buildings
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // Text
    init_pair(4, COLOR_WHITE, COLOR_BLACK);   // Stars
    init_pair(5, COLOR_RED, COLOR_BLACK);     // Sunset

    WINDOW *anim_win = newwin(max_y, max_x, 0, 0);
    keypad(anim_win, TRUE);
    nodelay(anim_win, TRUE);

    typedef struct {
        int x;
        int y;
        bool visible;
    } Star;

    Star stars[50];
    for (int i = 0; i < 50; i++) {
        stars[i].x = rand() % max_x;
        stars[i].y = rand() % (max_y/2);
        stars[i].visible = false;
    }

    for (int frame = 0; frame < 60; frame++) {
        wclear(anim_win);
        box(anim_win, 0, 0);

        wattron(anim_win, COLOR_PAIR(5));
        for (int i = 0; i < max_y/3; i++) {
            mvwhline(anim_win, i, 1, '~', max_x-2);
        }
        wattroff(anim_win, COLOR_PAIR(5));
        wattron(anim_win, COLOR_PAIR(4));
        for (int i = 0; i < 50; i++) {
            if (frame > i/2) {
                stars[i].visible = true;
            }
            if (stars[i].visible) {
                draw_star(anim_win, stars[i].x, stars[i].y);
            }
        }
        wattroff(anim_win, COLOR_PAIR(4));

        wattron(anim_win, COLOR_PAIR(2));
        for (int i = 0; i < max_x/6; i++) {
            int height = 5 + (rand() % 10);
            draw_building(anim_win, i*6, max_y-5, height);
        }
        wattroff(anim_win, COLOR_PAIR(2));

        if (frame > 10) {
            wattron(anim_win, COLOR_PAIR(1));
            for (int i = 0; i < 5; i++) {
                draw_palm_tree(anim_win, i*(max_x/5), max_y-8);
            }
            wattroff(anim_win, COLOR_PAIR(1));
        }

        if (frame > 20) {
            wattron(anim_win, COLOR_PAIR(3) | A_BOLD);
            const char *title = "WELCOME TO ROGUE GAME";
            mvwprintw(anim_win, max_y/2, (max_x-strlen(title))/2, "%s", title);
            
            if (frame > 40) {
                const char *prompt = "Press any key to start your adventure...";
                mvwprintw(anim_win, max_y-3, (max_x-strlen(prompt))/2, "%s", prompt);
            }
            wattroff(anim_win, COLOR_PAIR(3) | A_BOLD);
        }

        wrefresh(anim_win);
        usleep(100000);

        int ch = wgetch(anim_win);
        if (ch != ERR) {
            break;
        }
    }

    nodelay(anim_win, FALSE);
    wgetch(anim_win);
    wclear(anim_win);
    wrefresh(anim_win);
    delwin(anim_win);
}

void initialize_la_theme() {
    clear();
    refresh();
    la_animation();
    clear();
    refresh();
}

// int main() {
//     // Set up locale for UTF-8 support
//     setlocale(LC_ALL, "");
    
//     // Initialize ncurses
//     initscr();
    
//     // Set up ncurses options
//     noecho();              // Don't echo keypresses
//     curs_set(0);          // Hide cursor
//     cbreak();             // Disable line buffering
    
//     // Initialize random seed
//     srand(time(NULL));
    
//     // Start color mode
//     start_color();
    
//     // Run the animation
//     la_animation();
    
//     // Clean up
//     endwin();
    
//     return 0;
// }
