#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include<locale.h>
const char *rockstar[] = {
    "    \\m/    ",
    "   _|_|_   ",
    "  |o   o|  ",
    "  | >_< |  ",
    "   \\___/   ",
    "   _| |_   ",
    "  / | | \\  ",
    " /  | |  \\ ",
    "|   | |   |",
    "    | |    ",
    "   /   \\   ",
    "  /     \\  ",
    " /       \\ ",
    "/         \\",
};

const char *guitar_art[] = {
    "       .-.",
    "  | |",
    "  | |   .-.",
    "  | |-._| |",
    "  |_| | | |",
    " / )|_|_|-|",
    "| | `-^-^ |",
    "|     ||  |",
    "\\     '   /",
    " |       |",
    " |       |"
};


void draw_speech_bubble(WINDOW *win, int x, int y, const char *text, int style) {
    int len = strlen(text);
    
    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, y, x, " ");
    for(int i = 0; i < len + 4; i++) mvwaddch(win, y, x + i, '_');
    mvwprintw(win, y + 1, x, "( %s )", text);
    mvwprintw(win, y + 2, x, " ");
    for(int i = 0; i < len + 4; i++) mvwaddch(win, y + 2, x + i, '-');
    if(style == 0) {
        mvwprintw(win, y + 3, x + 2, "\\");
        mvwprintw(win, y + 4, x + 3, "\\");
    } else {
        mvwprintw(win, y + 3, x + len + 2, "/");
        mvwprintw(win, y + 4, x + len + 1, "/");
    }
    
    wattroff(win, COLOR_PAIR(1) | A_BOLD);
}

void draw_rockstar(WINDOW *win, int x, int y, int frame) {
    int height = sizeof(rockstar) / sizeof(rockstar[0]);
    
    int color = (frame / 2 % 4) + 1;
    
    for(int i = 0; i < height; i++) {
        if(i < 5) {
            wattron(win, COLOR_PAIR(color) | A_BOLD);
        } else {
            wattron(win, COLOR_PAIR(2) | A_BOLD); 
        }
        
        mvwprintw(win, y + i, x, "%s", rockstar[i]);
        wattroff(win, COLOR_PAIR(color) | A_BOLD);
        wattroff(win, COLOR_PAIR(2) | A_BOLD);
    }
}

void draw_guitar(WINDOW *win, int x, int y, int frame, bool flip) {
    int height = sizeof(guitar_art) / sizeof(guitar_art[0]);
    wattron(win, COLOR_PAIR(3) | ((frame % 6 < 3) ? A_BOLD : 0));
    
    for(int i = 0; i < height; i++) {
        if(flip) {
            char flipped[50];
            strcpy(flipped, guitar_art[i]);
            int len = strlen(flipped);
            for(int j = 0; j < len/2; j++) {
                char temp = flipped[j];
                flipped[j] = flipped[len-1-j];
                flipped[len-1-j] = temp;
            }
            mvwprintw(win, y + i, x, "%s", flipped);
        } else {
            mvwprintw(win, y + i, x, "%s", guitar_art[i]);
        }
    }
    wattroff(win, COLOR_PAIR(3) | ((frame % 6 < 3) ? A_BOLD : 0));
}

void draw_stage_lights(WINDOW *win, int max_y, int max_x, int frame) {
    const char* patterns[] = {
        "\\\\|||//",
        "//|||\\\\",
        "\\\\---//",
        "//---\\\\"
    };
    
    for(int i = 0; i < max_x; i += 10) {
        wattron(win, COLOR_PAIR((i/10 % 4) + 4) | A_BOLD);
        mvwprintw(win, 2, i, "%s", patterns[frame % 4]);
        wattroff(win, COLOR_PAIR((i/10 % 4) + 4) | A_BOLD);
    }
}

const char* get_random_taunt() {
    const char* taunts[] = {
        "WHAT A LOSER!",
        "TOTALLY WEAK!",
        "NOT METAL ENOUGH!",
        "EPIC FAIL, DUDE!",
        "GO HOME ROOKIE!",
        "THAT WAS BOGUS!",
        "NO SKILLS, BRO!",
        "MEGA WEAK SAUCE!"
    };
    return taunts[rand() % 8];
}

void rock_star_game_over() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *anim_win = newwin(max_y, max_x, 0, 0);
    keypad(anim_win, TRUE);
    nodelay(anim_win, TRUE);

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);    // Base color
    init_pair(2, 122, COLOR_BLACK);   // Rockstar body
    init_pair(3, 197, COLOR_BLACK);      // Guitar
    init_pair(4, 229, COLOR_BLACK);  // Stage lights
    init_pair(5, 45, COLOR_BLACK);     // More lights
    init_pair(6, 69, COLOR_BLACK);    // More lights
    init_pair(7, 141, COLOR_BLACK);     // More lights
    init_pair(8, 205, COLOR_BLACK);      // Text emphasis

    const char *current_taunt = get_random_taunt();
    bool guitar_playing = false;

    for(int frame = 0; frame < 200; frame++) {
        wclear(anim_win);

        wattron(anim_win, COLOR_PAIR(1) | A_BOLD);
        mvwhline(anim_win, max_y-5, 0, '_', max_x);
        mvwhline(anim_win, max_y-1, 0, '=', max_x);
        wattroff(anim_win, COLOR_PAIR(1) | A_BOLD);

        draw_stage_lights(anim_win, max_y, max_x, frame);
        draw_rockstar(anim_win, 20, max_y-20, frame);
        
        draw_speech_bubble(anim_win, 35, max_y-25, current_taunt, 0);

        draw_guitar(anim_win, 40, max_y-15, frame, false);

        const char *game_over = "GAME OVER!";
        for(size_t i = 0; i < strlen(game_over); i++) {
            wattron(anim_win, COLOR_PAIR(4 + (i % 4)) | A_BOLD);
            mvwaddch(anim_win, 5, (max_x-strlen(game_over))/2 + i, game_over[i]);
            wattroff(anim_win, COLOR_PAIR(4 + (i % 4)) | A_BOLD);
        }

        if(frame > 30) {
            const char *exit_text = "Press any key to exit...";
            wattron(anim_win, COLOR_PAIR(1) | ((frame % 4 < 2) ? A_BOLD : 0));
            mvwprintw(anim_win, max_y-2, (max_x-strlen(exit_text))/2, "%s", exit_text);
            wattroff(anim_win, COLOR_PAIR(1) | A_BOLD);
        }

        if(frame % 50 == 0) {
            current_taunt = get_random_taunt();
        }

        wrefresh(anim_win);
        usleep(50000); 

        int ch = wgetch(anim_win);
        if(ch != ERR && frame > 30) {
            break;
        }
    }

    wclear(anim_win);
    wrefresh(anim_win);
    delwin(anim_win);
}

// int main() {
//     setlocale(LC_ALL, "");
//     initscr();
//     noecho();
//     curs_set(0);
//     srand(time(NULL));
//     rock_star_game_over(1500);
    
//     endwin();
//     return 0;
// }

void rockstar_animation() {
    clear();
    refresh();
    rock_star_game_over();
    clear();
    refresh();
}