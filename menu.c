#include "game.h"
#include "audio.h"
#include "anime3.c"
#include "LA.c"
#include "database.h"

#include <dirent.h>
extern void start_game(void);
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h> 
#include <math.h>
#include <wchar.h>
#define USERS_PER_PAGE 5

void start_game();

int difficulty = DIFFICULTY_MEDIUM; 
extern Mix_Music *current_music;
int music_choice = 0;
int character_color = 20;
char messages[10][100];
int message_count=0;
extern bool db_enabled;


void pre_game_menu(void);
void displayLeaderboard(void);
void set_difficulty(void);
void set_character_color(void);
void set_music(void);
void settings_menu(void);
void display_profile_menu(void);
void log_message(const char *new_message);


int compareLeaderboard(const void *a, const void *b) {
    return ((LeaderboardEntry *)b)->totalPoints - ((LeaderboardEntry *)a)->totalPoints;
}

void assignTitlesAndEmojis(LeaderboardData *data) {
    for (int i = 0; i < data->totalEntries; i++) {
        if (i == 0) {
            strcpy(data->entries[i].title, "GOAT");
            strcpy(data->entries[i].emoji, "🏆");
        } else if (i == 1) {
            strcpy(data->entries[i].title, "Legend");
            strcpy(data->entries[i].emoji, "🥈");
        } else if (i == 2) {
            strcpy(data->entries[i].title, "Hero");
            strcpy(data->entries[i].emoji, "🥉");
        } else {
            strcpy(data->entries[i].title, "");
            strcpy(data->entries[i].emoji, "");
        }
    }
}

void displayLeaderboard() {
    LeaderboardData data = {0};
    loadLeaderboard(&data);

    int height = 20, width = 75;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    WINDOW *leaderboard_win = newwin(height, width, start_y, start_x);
    keypad(leaderboard_win, TRUE);

    start_color();
    init_pair(10, COLOR_YELLOW, COLOR_BLACK);  // Gold
    init_pair(11, COLOR_CYAN, COLOR_BLACK);    // Silver
    init_pair(12, COLOR_RED, COLOR_BLACK);     // Bronze
    init_pair(13, COLOR_GREEN, COLOR_BLACK);   // Current user
    init_pair(14, COLOR_WHITE, COLOR_BLACK);   // Others

    int scroll_position = 0;
    int max_scroll = (data.totalEntries > 0) ? data.totalEntries - 1 : 0;

    while (1) {
        werase(leaderboard_win);
        box(leaderboard_win, 0, 0);

        mvwprintw(leaderboard_win, 0, (width - 15) / 2, "[ LEADERBOARD ]");
        
        for (int i = 0; i < MIN(3, data.totalEntries); i++) {
            LeaderboardEntry *entry = &data.entries[i];
            
            wattron(leaderboard_win, COLOR_PAIR(10 + i) | A_BOLD);
            if (i == 0) mvwprintw(leaderboard_win, 1, 2, "🏆 Champion: %s (Score: %d)", entry->username, entry->totalPoints);
            else if (i == 1) mvwprintw(leaderboard_win, 2, 2, "🥈 Runner-up: %s (Score: %d)", entry->username, entry->totalPoints);
            else mvwprintw(leaderboard_win, 3, 2, "🥉 Third Place: %s (Score: %d)", entry->username, entry->totalPoints);
            wattroff(leaderboard_win, COLOR_PAIR(10 + i) | A_BOLD);
        }

        mvwprintw(leaderboard_win, 4, 2, "Rank  Username          Points   Gold    Games   Exp(days)  Title");
        mvwprintw(leaderboard_win, 5, 2, "--------------------------------------------------------------------");

        int list_start = 6;
        int visible_entries = height - list_start - 2;
        
        for (int i = 0; i < visible_entries && (i + scroll_position) < data.totalEntries; i++) {
            LeaderboardEntry *entry = &data.entries[i + scroll_position];
            int y_pos = list_start + i;
            
            if (entry->rank <= 3) wattron(leaderboard_win, COLOR_PAIR(10 + entry->rank - 1));
            else if (entry->isCurrentUser) wattron(leaderboard_win, COLOR_PAIR(13));
            else wattron(leaderboard_win, COLOR_PAIR(14));

            mvwprintw(leaderboard_win, y_pos, 2, "%3d %s %-15s %7d %7d %7d %8d    %s",
                     entry->rank,
                     entry->emoji,
                     entry->username,
                     entry->totalPoints,
                     entry->totalGold,
                     entry->gamesCompleted,
                     entry->timeSinceFirstGame,
                     entry->title);

            if (entry->rank <= 3 || entry->isCurrentUser) 
                wattroff(leaderboard_win, COLOR_PAIR(entry->rank <= 3 ? 10 + entry->rank - 1 : 13));
        }

        mvwprintw(leaderboard_win, height - 1, 2, 
                 "↑/↓: Scroll | B: Back | Position: %d/%d",
                 scroll_position + 1, 
                 data.totalEntries);

        wrefresh(leaderboard_win);

        int ch = wgetch(leaderboard_win);
        if (ch == 'B' || ch == 'b') {
            delwin(leaderboard_win);
            clear();
            refresh();
            pre_game_menu();
            return;
        }
        else if (ch == KEY_UP && scroll_position > 0) {
            scroll_position--;
        }
        else if (ch == KEY_DOWN && scroll_position < max_scroll) {
            scroll_position++;
        }
    }
}

void save_leaderboard_data(Player *player) {
    char stats_file[256];
    time_t now = time(NULL);
    int playtime = (int)(now - current_player_data.last_save_time) / 60;
    
    snprintf(stats_file, sizeof(stats_file), "player_data/%s_stats.txt", player->username);
    FILE *file = fopen(stats_file, "w");
    if (!file) {
        show_message_for_2_seconds("Error saving stats!");
        return;
    }

    fprintf(file, "Session End: %s", ctime(&now));
    fprintf(file, "Playtime: %d minutes\n", playtime);
    fprintf(file, "Final Gold: %d\n", player->gold);
    fprintf(file, "Final Health: %d/%d\n", 
            player->health.current_health, 
            player->health.max_health);
    fprintf(file, "------------------\n");
    fclose(file);

    if (db_enabled) {
        update_player_stats_db(player->username, 
                             player->gold,
                             player->completed_games,
                             player->health.current_health,
                             player->health.max_health);
    }
}

void loadLeaderboard(LeaderboardData *data) {
    data->totalEntries = 0;
    data->currentPage = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir("player_data");
    
    if (!d) return;

    while ((dir = readdir(d)) != NULL && data->totalEntries < 100) {
        if (strstr(dir->d_name, "_stats.txt")) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "player_data/%s", dir->d_name);
            
            FILE *stats = fopen(filepath, "r");
            if (!stats) continue;

            LeaderboardEntry *entry = &data->entries[data->totalEntries];
            char line[256], *username;
            int gold = 0, health = 0;
            
            username = strtok(dir->d_name, "_");
            if (!username) continue;
            
            strncpy(entry->username, username, sizeof(entry->username) - 1);
            entry->username[sizeof(entry->username) - 1] = '\0';
            
            while (fgets(line, sizeof(line), stats)) {
                if (strstr(line, "Final Gold:")) {
                    sscanf(line, "Final Gold: %d", &gold);
                }
                else if (strstr(line, "Final Health:")) {
                    sscanf(line, "Final Health: %d/%*d", &health);
                }
            }
            
            int game_count = 0;
            rewind(stats);
            while (fgets(line, sizeof(line), stats)) {
                if (strstr(line, "Session End:")) {
                    game_count++;
                }
            }
            
            time_t first_game_time = time(NULL);
            rewind(stats);
            while (fgets(line, sizeof(line), stats)) {
                if (strstr(line, "Login Time:")) {
                    struct tm tm;
                    if (strptime(line + 11, "%c", &tm) != NULL) {
                        time_t login_time = mktime(&tm);
                        if (login_time < first_game_time) {
                            first_game_time = login_time;
                        }
                    }
                }
            }
            
            entry->totalPoints = (gold * 5) + health;
            entry->totalGold = gold;
            entry->gamesCompleted = game_count > 0 ? game_count : 1;
            entry->timeSinceFirstGame = (int)((time(NULL) - first_game_time) / (24 * 60 * 60));
            entry->isCurrentUser = (strcmp(username, current_player.username) == 0);
            
            data->totalEntries++;
            fclose(stats);
        }
    }
    closedir(d);

    qsort(data->entries, data->totalEntries, sizeof(LeaderboardEntry), compareLeaderboard);
    for (int i = 0; i < data->totalEntries; i++) {
        data->entries[i].rank = i + 1;
    }
    
    assignTitlesAndEmojis(data);
}

int username_exists(const char *username) {
    FILE *file = fopen("filepro.txt", "r");
    if (!file) return 0;

    char line[256];
    char existing_username[50];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,]", existing_username);
        if (strcmp(existing_username, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int validate_password(const char *password) {
    if (strlen(password) < 7) return 0;

    int has_digit = 0, has_upper = 0, has_lower = 0;
    for (int i = 0; password[i]; i++) {
        if (isdigit(password[i])) has_digit = 1;
        else if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
    }
    return has_digit && has_upper && has_lower;
}

int validate_email(const char *email) {
    const char *at = strchr(email, '@');
    if (!at) return 0;

    const char *dot = strchr(at, '.');
    if (!dot || dot == at + 1) return 0;

    return strlen(dot + 1) > 1;
}

void generate_random_password(char *password, int length) {
    const char *upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lower = "abcdefghijklmnopqrstuvwxyz";
    const char *numbers = "0123456789";
    password[0] = upper[rand() % strlen(upper)];
    password[1] = lower[rand() % strlen(lower)];
    password[2] = numbers[rand() % strlen(numbers)];
    
    const char *all = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 3; i < length - 1; i++) {
        password[i] = all[rand() % strlen(all)];
    }
    
    for (int i = 0; i < length - 2; i++) {
        int j = i + rand() % (length - 1 - i);
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }
    
    password[length - 1] = '\0';
}

void new_user_menu(WINDOW *menu_win) {
    char username[100] = {0}, password[50] = {0}, email[100] = {0};

    wbkgd(menu_win, COLOR_PAIR(1)); 
    wclear(menu_win);
    
    wattron(menu_win, COLOR_PAIR(4) | A_BOLD);
    box(menu_win, ACS_VLINE, ACS_HLINE);
    mvwhline(menu_win, 2, 1, ACS_HLINE, getmaxx(menu_win) - 2);
    mvwvline(menu_win, 3, getmaxx(menu_win) - 2, ACS_VLINE, getmaxy(menu_win) - 5);
    mvwvline(menu_win, 3, 1, ACS_VLINE, getmaxy(menu_win) - 5);
    mvwaddch(menu_win, 2, 0, ACS_LTEE);
    mvwaddch(menu_win, 2, getmaxx(menu_win) - 1, ACS_RTEE);
    mvwaddch(menu_win, getmaxy(menu_win) - 1, 0, ACS_LLCORNER);
    mvwaddch(menu_win, getmaxy(menu_win) - 1, getmaxx(menu_win) - 1, ACS_LRCORNER);
    wattroff(menu_win, COLOR_PAIR(4) | A_BOLD);

    wattron(menu_win, A_BOLD | COLOR_PAIR(2));
    mvwprintw(menu_win, 1, (getmaxx(menu_win) - 10) / 2, "LOGIN MENU");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(2));

    int start_y = 5;
    const char *fields[] = {"Username:", "Password:", "Email:"};
    for (int i = 0; i < 3; i++) {
        WINDOW *field_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, start_y + i * 4, 5);
        wattron(field_box, COLOR_PAIR(4) | A_BOLD);
        box(field_box, 0, 0);
        wattroff(field_box, COLOR_PAIR(4) | A_BOLD);
        wattron(field_box, COLOR_PAIR(2));
        mvwprintw(field_box, 1, 2, fields[i]);
        wattroff(field_box, COLOR_PAIR(2));
        wrefresh(field_box);
    }

    wattron(menu_win, COLOR_PAIR(5));
    mvwprintw(menu_win, getmaxy(menu_win) - 5, 5, "Press F to recover forgotten password");
    mvwprintw(menu_win, getmaxy(menu_win) - 4, 5, "Press R for a random password");
    wattroff(menu_win, COLOR_PAIR(5));
    
    wrefresh(menu_win);

    while (1) {
        noecho();
        int ch = wgetch(menu_win);
        
        if (ch == 'F' || ch == 'f') {
            wclear(menu_win);
            wattron(menu_win, COLOR_PAIR(4) | A_BOLD);
            box(menu_win, ACS_VLINE, ACS_HLINE);
            mvwhline(menu_win, 2, 1, ACS_HLINE, getmaxx(menu_win) - 2);
            wattroff(menu_win, COLOR_PAIR(4) | A_BOLD);
            
            wattron(menu_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(menu_win, 1, (getmaxx(menu_win) - 17) / 2, "PASSWORD RECOVERY");
            wattroff(menu_win, A_BOLD | COLOR_PAIR(2));

            WINDOW *email_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, 5, 5);
            wattron(email_box, COLOR_PAIR(4) | A_BOLD);
            box(email_box, 0, 0);
            mvwprintw(email_box, 1, 2, "Enter your email:");
            wattroff(email_box, COLOR_PAIR(4) | A_BOLD);
            wrefresh(email_box);
            
            mvwprintw(menu_win, 6, 25, "                    ");
            wmove(menu_win, 6, 25);
            wrefresh(menu_win);
            echo();
            wgetnstr(menu_win, email, 40);
            noecho();

            if (!validate_email(email)) {
                wattron(menu_win, COLOR_PAIR(3));
                mvwprintw(menu_win, 9, 5, "Error: Invalid email format!");
                wattroff(menu_win, COLOR_PAIR(3));
            } else {
                char username[50];
                if (find_user_by_email(email, username)) {
                    char new_password[12];
                    generate_random_password(new_password, 12);
                    
                    char filepath[256];
                    snprintf(filepath, sizeof(filepath), "player_data/%s_stats.txt", username);
                    FILE *file = fopen(filepath, "a");
                    if (file) {
                        time_t now = time(NULL);
                        fprintf(file, "\nPassword Changed: %s\n", new_password);
                        fprintf(file, "Change Time: %s", ctime(&now));
                        fclose(file);
                        
                        WINDOW *result_box = derwin(menu_win, 4, getmaxx(menu_win) - 10, 9, 5);
                        wattron(result_box, COLOR_PAIR(4) | A_BOLD);
                        box(result_box, 0, 0);
                        mvwprintw(result_box, 1, 2, "Your new password is: %s", new_password);
                        mvwprintw(result_box, 2, 2, "Please login with this password");
                        wattroff(result_box, COLOR_PAIR(4) | A_BOLD);
                        wrefresh(result_box);
                    }
                } else {
                    wattron(menu_win, COLOR_PAIR(3));
                    mvwprintw(menu_win, 9, 5, "Email not found!");
                    wattroff(menu_win, COLOR_PAIR(3));
                }
            }
            
            mvwprintw(menu_win, getmaxy(menu_win) - 3, 5, "Press any key to return to login...");
            wrefresh(menu_win);
            wgetch(menu_win);
            
            wclear(menu_win);
            return new_user_menu(menu_win);
        }
        
        mvwprintw(menu_win, 6, 25, "                    ");
        wmove(menu_win, 6, 25);
        wrefresh(menu_win);
        echo();
        wgetnstr(menu_win, username, 20);
        noecho();
        
        if (username_exists(username)) {
            WINDOW *error_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, getmaxy(menu_win) - 8, 5);
            wattron(error_box, COLOR_PAIR(3));
            box(error_box, 0, 0);
            mvwprintw(error_box, 1, 2, "Error: Username already exists!");
            wattroff(error_box, COLOR_PAIR(3));
            wrefresh(error_box);
            continue;
        }

        mvwprintw(menu_win, 10, 25, "                    ");
        wmove(menu_win, 10, 25);
        wrefresh(menu_win);
        echo();
        ch = wgetch(menu_win);
        
        if (ch == 'R' || ch == 'r') {
            generate_random_password(password, 12);
            mvwprintw(menu_win, 10, 25, "%-20s", password);
            wrefresh(menu_win);
        } else {
            ungetch(ch);
            wgetnstr(menu_win, password, 20);
        }
        noecho();

        if (!validate_password(password)) {
            WINDOW *error_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, getmaxy(menu_win) - 8, 5);
            wattron(error_box, COLOR_PAIR(3));
            box(error_box, 0, 0);
            mvwprintw(error_box, 1, 2, "Error: Weak password!");
            wattroff(error_box, COLOR_PAIR(3));
            wrefresh(error_box);
            continue;
        }

        mvwprintw(menu_win, 14, 25, "                    ");
        wmove(menu_win, 14, 25);
        wrefresh(menu_win);
        echo();
        wgetnstr(menu_win, email, 40);
        noecho();
        
        if (!validate_email(email)) {
            WINDOW *error_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, getmaxy(menu_win) - 8, 5);
            wattron(error_box, COLOR_PAIR(3));
            box(error_box, 0, 0);
            mvwprintw(error_box, 1, 2, "Error: Invalid email format!");
            wattroff(error_box, COLOR_PAIR(3));
            wrefresh(error_box);
            continue;
        }

        WINDOW *success_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, getmaxy(menu_win) - 8, 5);
        wattron(success_box, COLOR_PAIR(4) | A_BOLD);
        box(success_box, 0, 0);
        mvwprintw(success_box, 1, 2, "User created successfully!");
        wattroff(success_box, COLOR_PAIR(4) | A_BOLD);
        wrefresh(success_box);
        
        if (init_database()) {
            if (store_player_db(username, email)) {
                log_message("Player data stored in database successfully!");
            } else {
                log_message("Note: Could not store player in database (continuing with file storage)");
            }
        }

        FILE *file = fopen("filepro.txt", "a");
        if (file) {
            fprintf(file, "%s,%s,%s\n", username, password, email);
            fclose(file);
        }

        time_t current_time = time(NULL);
        char filename[256];
        snprintf(filename, sizeof(filename), "player_data/%s_stats.txt", username);
        mkdir("player_data", 0777);
        FILE *player_file = fopen(filename, "a");
        if (player_file) {
            fprintf(player_file, "Login Time: %s\n", ctime(&current_time));
            fclose(player_file);
        }

        strncpy(current_player_data.username, username, sizeof(current_player_data.username) - 1);
        current_player_data.username[sizeof(current_player_data.username) - 1] = '\0';
        current_player_data.is_guest = false;
        current_player_data.last_save_time = time(NULL);

        initialize_player(&current_player, username, email);
        
        mvwprintw(menu_win, getmaxy(menu_win) - 2, 5, "Press any key to return to the main menu...");
        wrefresh(menu_win);
        wgetch(menu_win);
        break;
    }
}

void log_message(const char *new_message) {
    init_pair(5,COLOR_YELLOW,COLOR_BLACK);
    if (message_count==10){
        for (int i=0; i<9; i++){
            strcpy(messages[i], messages[i+1]);
        }
        message_count--;
    }
    strncpy(messages[message_count], new_message, 99);
    messages[message_count][99] = '\0';
    message_count++;
    for (int i = 0; i < message_count; i++) {
        start_color();
        attron(COLOR_PAIR(5));
        mvprintw(i, 0, "%s", messages[i]);
        attroff(COLOR_PAIR(5));
        clrtoeol();
    }
    refresh(); 
}

void guest_user(WINDOW *menu_win) {
    char guest_name[50];
    snprintf(guest_name, sizeof(guest_name), "Guest_%ld", time(NULL));
    
    strncpy(current_player_data.username, guest_name, sizeof(current_player_data.username) - 1);
    current_player_data.username[sizeof(current_player_data.username) - 1] = '\0';
    current_player_data.is_guest = true;
    current_player_data.last_save_time = time(NULL);
    initialize_player(&current_player, guest_name, "guest@guest.com");

    wclear(menu_win);
    wbkgd(menu_win, COLOR_PAIR(1));
    
    wattron(menu_win, COLOR_PAIR(4) | A_BOLD);
    box(menu_win, ACS_VLINE, ACS_HLINE);
    
    mvwhline(menu_win, 2, 1, ACS_HLINE, getmaxx(menu_win) - 2);
    mvwvline(menu_win, 3, getmaxx(menu_win) - 2, ACS_VLINE, getmaxy(menu_win) - 5);
    mvwvline(menu_win, 3, 1, ACS_VLINE, getmaxy(menu_win) - 5);
    
    mvwaddch(menu_win, 2, 0, ACS_LTEE);
    mvwaddch(menu_win, 2, getmaxx(menu_win) - 1, ACS_RTEE);
    mvwaddch(menu_win, getmaxy(menu_win) - 1, 0, ACS_LLCORNER);
    mvwaddch(menu_win, getmaxy(menu_win) - 1, getmaxx(menu_win) - 1, ACS_LRCORNER);
    wattroff(menu_win, COLOR_PAIR(4) | A_BOLD);

    wattron(menu_win, A_BOLD | COLOR_PAIR(2));
    mvwprintw(menu_win, 1, (getmaxx(menu_win) - 11) / 2, "GUEST LOGIN");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(2));

    WINDOW *info_box = derwin(menu_win, 5, getmaxx(menu_win) - 10, 8, 5);
    wattron(info_box, COLOR_PAIR(4) | A_BOLD);
    box(info_box, 0, 0);
    wattroff(info_box, COLOR_PAIR(4) | A_BOLD);
    
    wattron(info_box, COLOR_PAIR(2));
    mvwprintw(info_box, 1, 2, "Logging in as: %s", guest_name);
    mvwprintw(info_box, 3, 2, "Note: Progress won't be saved in guest mode");
    wattroff(info_box, COLOR_PAIR(2));
    wrefresh(info_box);

    WINDOW *prompt_box = derwin(menu_win, 3, getmaxx(menu_win) - 10, getmaxy(menu_win) - 6, 5);
    wattron(prompt_box, COLOR_PAIR(4) | A_BOLD);
    box(prompt_box, 0, 0);
    wattroff(prompt_box, COLOR_PAIR(4) | A_BOLD);
    
    wattron(prompt_box, COLOR_PAIR(2));
    mvwprintw(prompt_box, 1, 2, "Press any key to continue...");
    wattroff(prompt_box, COLOR_PAIR(2));
    wrefresh(prompt_box);

    wrefresh(menu_win);
    wgetch(menu_win);
    
    delwin(info_box);
    delwin(prompt_box);
}

void login() {
    int log_in = 0;
    curs_set(0);
    colorspair(); 
    character_color = 20;
    int height = 25, width = 70;  
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *menu_win = newwin(height, width, start_y, start_x);
    keypad(menu_win, TRUE);

    init_pair(1, COLOR_WHITE, 86);
    init_pair(2, COLOR_BLACK, 86);// Text color
    init_pair(3, COLOR_RED, 86); // Error/highlight color
    init_pair(4, COLOR_BLACK, 105);   // Border color
    wbkgd(menu_win, COLOR_PAIR(1));
    
    wattron(menu_win, COLOR_PAIR(4) | A_BOLD);
    box(menu_win, ACS_VLINE, ACS_HLINE);
    
    mvwhline(menu_win, 2, 1, ACS_HLINE, width - 2);
    mvwvline(menu_win, 3, width - 2, ACS_VLINE, height - 5);
    mvwvline(menu_win, 3, 1, ACS_VLINE, height - 5);
    
    mvwaddch(menu_win, 2, 0, ACS_LTEE);
    mvwaddch(menu_win, 2, width - 1, ACS_RTEE);
    mvwaddch(menu_win, height - 1, 0, ACS_LLCORNER);
    mvwaddch(menu_win, height - 1, width - 1, ACS_LRCORNER);
    
    wattroff(menu_win, COLOR_PAIR(4) | A_BOLD);

    wattron(menu_win, A_BOLD | COLOR_PAIR(2));
    mvwprintw(menu_win, 1, (width - 10) / 2, "LOGIN MENU");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(2));

    wattron(menu_win, COLOR_PAIR(2));
    mvwprintw(menu_win, 5, (width - 24) / 2, "Welcome to Rogue Game!");
    mvwprintw(menu_win, 7, (width - 30) / 2, "Select Login Option:");
    
    for (int i = 0; i < 2; i++) {
        WINDOW *option_box = derwin(menu_win, 3, width - 10, 10 + i * 5, 5);
        
        wattron(option_box, COLOR_PAIR(4) | A_BOLD);
        box(option_box, 0, 0);
        wattroff(option_box, COLOR_PAIR(4) | A_BOLD);
        
        wattron(option_box, COLOR_PAIR(2));
        if (i == 0) {
            mvwprintw(option_box, 1, 2, "[1] Create New Account");
        } else {
            mvwprintw(option_box, 1, 2, "[2] Enter as Guest");
        }
        wattroff(option_box, COLOR_PAIR(2));
        
        wrefresh(option_box);
    }

    wrefresh(menu_win);

    echo();
    mvwprintw(menu_win, height - 5, (width - 20) / 2, "Enter choice: ");
    wscanw(menu_win, "%d", &log_in);
    noecho();

    switch (log_in) {
        case 1:
            new_user_menu(menu_win);
            break;
        case 2:
            guest_user(menu_win);
            break;
        default:
            wattron(menu_win, COLOR_PAIR(3));
            mvwprintw(menu_win, height - 5, (width - 24) / 2, "Invalid choice. Press any key.");
            wattroff(menu_win, COLOR_PAIR(3));
            wrefresh(menu_win);
            wgetch(menu_win);
            break;
    }
    delwin(menu_win);
}

void set_difficulty(void) {
    const char *difficulties[] = {
        "Easy",
        "Medium",
        "Hard"
    };
    int num_choices = 3;
    int highlight = 0;
    int choice = -1;
    int height = 25, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *difficulty_win = newwin(height, width, start_y, start_x);
    keypad(difficulty_win, TRUE);

    init_pair(1, COLOR_WHITE, 86);
    init_pair(2, COLOR_BLACK, 86);
    init_pair(3, COLOR_RED, 86);
    init_pair(4, COLOR_BLACK, 105);
    wbkgd(difficulty_win, COLOR_PAIR(1));
    
    while (1) {
        werase(difficulty_win);
        
        wattron(difficulty_win, COLOR_PAIR(4) | A_BOLD);
        box(difficulty_win, ACS_VLINE, ACS_HLINE);
        
        mvwhline(difficulty_win, 2, 1, ACS_HLINE, width - 2);
        mvwvline(difficulty_win, 3, width - 2, ACS_VLINE, height - 5);
        mvwvline(difficulty_win, 3, 1, ACS_VLINE, height - 5);
        
        mvwaddch(difficulty_win, 2, 0, ACS_LTEE);
        mvwaddch(difficulty_win, 2, width - 1, ACS_RTEE);
        mvwaddch(difficulty_win, height - 1, 0, ACS_LLCORNER);
        mvwaddch(difficulty_win, height - 1, width - 1, ACS_LRCORNER);
        wattroff(difficulty_win, COLOR_PAIR(4) | A_BOLD);

        wattron(difficulty_win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(difficulty_win, 1, (width - 18) / 2, "SELECT DIFFICULTY");
        wattroff(difficulty_win, A_BOLD | COLOR_PAIR(2));

        for (int i = 0; i < num_choices; i++) {
            WINDOW *diff_box = derwin(difficulty_win, 3, width - 10, 5 + i * 4, 5);
            
            wattron(diff_box, COLOR_PAIR(4) | A_BOLD);
            box(diff_box, 0, 0);
            wattroff(diff_box, COLOR_PAIR(4) | A_BOLD);
            
            wattron(diff_box, COLOR_PAIR(2));
            if (i == highlight) {
                wattron(diff_box, A_REVERSE);
            }
            mvwprintw(diff_box, 1, 2, "[%d] %s", i + 1, difficulties[i]);
            
            if (i == difficulty - 1) {
                wattron(diff_box, COLOR_PAIR(3));
                mvwprintw(diff_box, 1, width - 20, "CURRENT");
                wattroff(diff_box, COLOR_PAIR(3));
            }
            
            if (i == highlight) {
                wattroff(diff_box, A_REVERSE);
            }
            wattroff(diff_box, COLOR_PAIR(2));
            
            wrefresh(diff_box);
        }

        mvwprintw(difficulty_win, height - 3, (width - 30) / 2, "Use arrow keys to navigate");
        mvwprintw(difficulty_win, height - 2, (width - 24) / 2, "Press Enter to select");
        
        wrefresh(difficulty_win);

        int c = wgetch(difficulty_win);
        switch (c) {
            case KEY_UP:
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_choices;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            case '1': case '2': case '3':
                choice = c - '1';
                break;
        }

        if (choice != -1) {
            difficulty = choice + 1;
            HealthSystem temp_health;
            init_health_system(&temp_health);
            
            delwin(difficulty_win);
            clear();
            refresh();
            pre_game_menu();
            return;
        }
    }
}

void set_character_color(void) {
    const char *colors[] = {
        "Blue",
        "Purple",
        "Barbie Pink",
        "Banana Yellow"
    };
    int num_choices = 4;
    int highlight = 0;
    int choice = -1;
    int height = 25, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *color_win = newwin(height, width, start_y, start_x);
    keypad(color_win, TRUE);

    init_pair(20, 69, COLOR_BLACK);
    init_pair(21, 57, COLOR_BLACK); 
    init_pair(22, 198, COLOR_BLACK); 
    init_pair(23, 193, COLOR_BLACK);

    init_pair(1, COLOR_WHITE, 86);
    init_pair(2, COLOR_BLACK, 86);
    init_pair(3, COLOR_RED, 86);
    init_pair(4, COLOR_BLACK, 105);
    wbkgd(color_win, COLOR_PAIR(1));
    
    while (1) {
        werase(color_win);
        
        wattron(color_win, COLOR_PAIR(4) | A_BOLD);
        box(color_win, ACS_VLINE, ACS_HLINE);
        
        mvwhline(color_win, 2, 1, ACS_HLINE, width - 2);
        mvwvline(color_win, 3, width - 2, ACS_VLINE, height - 5);
        mvwvline(color_win, 3, 1, ACS_VLINE, height - 5);
        
        mvwaddch(color_win, 2, 0, ACS_LTEE);
        mvwaddch(color_win, 2, width - 1, ACS_RTEE);
        mvwaddch(color_win, height - 1, 0, ACS_LLCORNER);
        mvwaddch(color_win, height - 1, width - 1, ACS_LRCORNER);
        wattroff(color_win, COLOR_PAIR(4) | A_BOLD);

        wattron(color_win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(color_win, 1, (width - 20) / 2, "SELECT CHARACTER COLOR");
        wattroff(color_win, A_BOLD | COLOR_PAIR(2));

        for (int i = 0; i < num_choices; i++) {
            WINDOW *color_box = derwin(color_win, 3, width - 10, 5 + i * 4, 5);
            
            wattron(color_box, COLOR_PAIR(4) | A_BOLD);
            box(color_box, 0, 0);
            wattroff(color_box, COLOR_PAIR(4) | A_BOLD);
            
            wattron(color_box, COLOR_PAIR(2));
            if (i == highlight) {
                wattron(color_box, A_REVERSE);
            }
            mvwprintw(color_box, 1, 2, "[%d] %s", i + 1, colors[i]);
            
            wattron(color_box, COLOR_PAIR(20 + i));
            mvwprintw(color_box, 1, width - 20, "@");
            wattroff(color_box, COLOR_PAIR(20 + i));
            
            
            if (i == highlight) {
                wattroff(color_box, A_REVERSE);
            }
            wattroff(color_box, COLOR_PAIR(2));
            
            wrefresh(color_box);
        }

        WINDOW *controls_box = derwin(color_win, 3, width - 10, height - 6, 5);
        wattron(controls_box, COLOR_PAIR(4) | A_BOLD);
        box(controls_box, 0, 0);
        wattroff(controls_box, COLOR_PAIR(4) | A_BOLD);
        
        wattron(controls_box, COLOR_PAIR(2));
        mvwprintw(controls_box, 1, 2, "Enter: Select | B: Back to Settings");
        wattroff(controls_box, COLOR_PAIR(2));
        wrefresh(controls_box);

        wrefresh(color_win);

        int c = wgetch(color_win);
        switch (c) {
            case KEY_UP:
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_choices;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            case '1': case '2': case '3': case '4':
                choice = c - '1';
                break;
            case 'b':
            case 'B':
                delwin(color_win);
                clear();
                refresh();
                settings_menu();
                return;
        }

        if (choice != -1) {
            character_color = 20 + choice;
            char *color_names[] = {"Blue", "Purple", "Pink", "Yellow"};
            
            WINDOW *success_box = derwin(color_win, 3, width - 20, height - 10, 10);
            wattron(success_box, COLOR_PAIR(4) | A_BOLD);
            box(success_box, 0, 0);
            mvwprintw(success_box, 1, 2, "Character color changed to %s!", color_names[choice]);
            wattroff(success_box, COLOR_PAIR(4) | A_BOLD);
            wrefresh(success_box);
            wgetch(color_win);
            
            delwin(color_win);
            clear();
            refresh();
            settings_menu();
            return;
        }
    }
}

void set_music(void) {
    if (!Mix_PlayingMusic()) {
        if (!init_audio()) {
            show_message_for_2_seconds("Failed to initialize audio!");
            return;
        }
        play_background_music(0);
    }

    const char *tracks[] = {
        "Holy Wars",
        "Peace sells",
        "Surprise Track"
    };
    int num_choices = 3;
    int highlight = 0;
    int choice = -1;
    int height = 25, width = 70; 
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *music_win = newwin(height, width, start_y, start_x);
    keypad(music_win, TRUE);

    init_pair(1, COLOR_WHITE, 86);
    init_pair(2, COLOR_BLACK, 86);
    init_pair(3, COLOR_RED, 86);
    init_pair(4, COLOR_BLACK, 105);
    init_pair(7, COLOR_GREEN, 86);
    wbkgd(music_win, COLOR_PAIR(1));
    
    while (1) {
        werase(music_win);
        
        wattron(music_win, COLOR_PAIR(4) | A_BOLD);
        box(music_win, ACS_VLINE, ACS_HLINE);
        
        mvwhline(music_win, 2, 1, ACS_HLINE, width - 2);
        mvwvline(music_win, 3, width - 2, ACS_VLINE, height - 5);
        mvwvline(music_win, 3, 1, ACS_VLINE, height - 5);
        
        mvwaddch(music_win, 2, 0, ACS_LTEE);
        mvwaddch(music_win, 2, width - 1, ACS_RTEE);
        mvwaddch(music_win, height - 1, 0, ACS_LLCORNER);
        mvwaddch(music_win, height - 1, width - 1, ACS_LRCORNER);
        wattroff(music_win, COLOR_PAIR(4) | A_BOLD);

        wattron(music_win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(music_win, 1, (width - 18) / 2, "SELECT MUSIC TRACK");
        wattroff(music_win, A_BOLD | COLOR_PAIR(2));

        for (int i = 0; i < num_choices; i++) {
            WINDOW *track_box = derwin(music_win, 3, width - 10, 5 + i * 4, 5);
            
            wattron(track_box, COLOR_PAIR(4) | A_BOLD);
            box(track_box, 0, 0);
            wattroff(track_box, COLOR_PAIR(4) | A_BOLD);
            
            wattron(track_box, COLOR_PAIR(2));
            if (i == highlight) {
                wattron(track_box, A_REVERSE);
            }
            mvwprintw(track_box, 1, 2, "[%d] %s", i + 1, tracks[i]);
            
            if (i == music_choice) {
                wattron(track_box, COLOR_PAIR(7));
                mvwprintw(track_box, 1, width - 15, "▶ ");
                wattroff(track_box, COLOR_PAIR(7));
            }
            
            if (i == highlight) {
                wattroff(track_box, A_REVERSE);
            }
            wattroff(track_box, COLOR_PAIR(2));
            
            wrefresh(track_box);
        }

        WINDOW *controls_box = derwin(music_win, 3, width - 10, height - 6, 5);
        wattron(controls_box, COLOR_PAIR(4) | A_BOLD);
        box(controls_box, 0, 0);
        wattroff(controls_box, COLOR_PAIR(4) | A_BOLD);
        
        wattron(controls_box, COLOR_PAIR(2));
        mvwprintw(controls_box, 1, 2, "M: Mute/Unmute | B: Back to Settings | Enter: Select Track");
        wattroff(controls_box, COLOR_PAIR(2));
        wrefresh(controls_box);

        wrefresh(music_win);

        int c = wgetch(music_win);
        switch (c) {
            case KEY_UP:
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_choices;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            case '1': case '2': case '3':
                choice = c - '1';
                break;
            case 'm':
            case 'M':
                if (Mix_PausedMusic()) {
                    Mix_ResumeMusic();
                    show_message_for_2_seconds("Music resumed!");
                } else if (Mix_PlayingMusic()) {
                    Mix_PauseMusic();
                    show_message_for_2_seconds("Music paused!");
                }
                break;
            case 'b':
            case 'B':
                delwin(music_win);
                clear();
                refresh();
                settings_menu();
                return;
        }

        if (choice != -1) {
            music_choice = choice;
            if (!play_background_music(music_choice)) {
                WINDOW *error_box = derwin(music_win, 3, width - 20, height - 10, 10);
                wattron(error_box, COLOR_PAIR(3) | A_BOLD);
                box(error_box, 0, 0);
                mvwprintw(error_box, 1, 2, "Failed to play selected track!");
                wattroff(error_box, COLOR_PAIR(3) | A_BOLD);
                wrefresh(error_box);
                wgetch(music_win);
                delwin(error_box);
            } else {
                WINDOW *success_box = derwin(music_win, 3, width - 20, height - 10, 10);
                wattron(success_box, COLOR_PAIR(4) | A_BOLD);
                box(success_box, 0, 0);
                mvwprintw(success_box, 1, 2, "Track changed successfully!");
                wattroff(success_box, COLOR_PAIR(4) | A_BOLD);
                wrefresh(success_box);
                wgetch(music_win);
                delwin(success_box);
            }
            delwin(music_win);
            clear();
            refresh();
            settings_menu();
            return;
        }
    }

    delwin(music_win);
    refresh();
}

void settings_menu(void) {
    const char *choices[] = {
        "Set Difficulty",
        "Change Character Color",
        "Select Background Music",
        "Back to Main Menu"
    };
    int num_choices = 4;
    int highlight = 0;
    int choice = -1;
    int height = 25, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *settings_win = newwin(height, width, start_y, start_x);
    keypad(settings_win, TRUE);

    init_pair(1, COLOR_WHITE, 86);
    init_pair(2, COLOR_BLACK, 86);
    init_pair(3, COLOR_RED, 86);
    init_pair(4, COLOR_BLACK, 105);
    wbkgd(settings_win, COLOR_PAIR(1));
    
    while (1) {
        werase(settings_win);
        wattron(settings_win, COLOR_PAIR(4) | A_BOLD);
        box(settings_win, ACS_VLINE, ACS_HLINE);
        
        mvwhline(settings_win, 2, 1, ACS_HLINE, width - 2);
        mvwvline(settings_win, 3, width - 2, ACS_VLINE, height - 5);
        mvwvline(settings_win, 3, 1, ACS_VLINE, height - 5);
        
        mvwaddch(settings_win, 2, 0, ACS_LTEE);
        mvwaddch(settings_win, 2, width - 1, ACS_RTEE);
        mvwaddch(settings_win, height - 1, 0, ACS_LLCORNER);
        mvwaddch(settings_win, height - 1, width - 1, ACS_LRCORNER);
        wattroff(settings_win, COLOR_PAIR(4) | A_BOLD);

        wattron(settings_win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(settings_win, 1, (width - 14) / 2, "SETTINGS MENU");
        wattroff(settings_win, A_BOLD | COLOR_PAIR(2));

        for (int i = 0; i < num_choices; i++) {
            WINDOW *option_box = derwin(settings_win, 3, width - 10, 5 + i * 4, 5);
            
            wattron(option_box, COLOR_PAIR(4) | A_BOLD);
            box(option_box, 0, 0);
            wattroff(option_box, COLOR_PAIR(4) | A_BOLD);
            
            wattron(option_box, COLOR_PAIR(2));
            if (i == highlight) {
                wattron(option_box, A_REVERSE);
            }
            mvwprintw(option_box, 1, 2, "[%d] %s", i + 1, choices[i]);
            if (i == highlight) {
                wattroff(option_box, A_REVERSE);
            }
            wattroff(option_box, COLOR_PAIR(2));
            
            wrefresh(option_box);
        }

        mvwprintw(settings_win, height - 3, (width - 30) / 2, "Use arrow keys to navigate");
        mvwprintw(settings_win, height - 2, (width - 24) / 2, "Press Enter to select");
        
        wrefresh(settings_win);

        int c = wgetch(settings_win);
        switch (c) {
            case KEY_UP:
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_choices;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            case '1': case '2': case '3': case '4':
                choice = c - '1';
                break;
        }

        if (choice != -1) break;
    }

    for (int i = 0; i < num_choices; i++) {
        WINDOW *option_box = derwin(settings_win, 3, width - 10, 5 + i * 4, 5);
        delwin(option_box);
    }

    switch (choice) {
        case 0:
            set_difficulty();
            break;
        case 1:
            set_character_color();
            break;
        case 2:
            set_music();
            break;
        case 3:
            delwin(settings_win);
            clear();
            refresh();
            pre_game_menu();
            return;
            break;
    }

    delwin(settings_win);
    refresh();
    getch();
}

void pre_game_menu() {
    const char *choices[] = {
        "Create New Game",
        "Continue Previous Game",
        "Score Table",
        "Settings",
        "Profile"
    };
    int num_choices = 5;
    int highlight = 0;
    int choice = -1;
    int height = 25, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *menu_win = newwin(height, width, start_y, start_x);
    keypad(menu_win, TRUE);

    init_pair(1, COLOR_WHITE, 86);
    init_pair(2, COLOR_BLACK, 86);
    init_pair(3, COLOR_RED, 86);
    init_pair(4, COLOR_BLACK, 105);
    wbkgd(menu_win, COLOR_PAIR(1));
    
    while (1) {
        werase(menu_win);
        
        wattron(menu_win, COLOR_PAIR(4) | A_BOLD);
        box(menu_win, ACS_VLINE, ACS_HLINE);
        
        mvwhline(menu_win, 2, 1, ACS_HLINE, width - 2);
        mvwvline(menu_win, 3, width - 2, ACS_VLINE, height - 5);
        mvwvline(menu_win, 3, 1, ACS_VLINE, height - 5);
        
        mvwaddch(menu_win, 2, 0, ACS_LTEE);
        mvwaddch(menu_win, 2, width - 1, ACS_RTEE);
        mvwaddch(menu_win, height - 1, 0, ACS_LLCORNER);
        mvwaddch(menu_win, height - 1, width - 1, ACS_LRCORNER);
        wattroff(menu_win, COLOR_PAIR(4) | A_BOLD);

        wattron(menu_win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(menu_win, 1, (width - 14) / 2, "PRE-GAME MENU");
        wattroff(menu_win, A_BOLD | COLOR_PAIR(2));

        for (int i = 0; i < num_choices; i++) {
            WINDOW *option_box = derwin(menu_win, 3, width - 10, 5 + i * 4, 5);
            
            wattron(option_box, COLOR_PAIR(4) | A_BOLD);
            box(option_box, 0, 0);
            wattroff(option_box, COLOR_PAIR(4) | A_BOLD);
            
            wattron(option_box, COLOR_PAIR(2));
            if (i == highlight) {
                wattron(option_box, A_REVERSE);
            }
            mvwprintw(option_box, 1, 2, "[%d] %s", i + 1, choices[i]);
            if (i == highlight) {
                wattroff(option_box, A_REVERSE);
            }
            wattroff(option_box, COLOR_PAIR(2));
            
            wrefresh(option_box);
        }

        wrefresh(menu_win);

        int c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_choices;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            case '1': case '2': case '3': case '4': case '5':
                choice = c - '1';
                break;
        }

        if (choice != -1) break;
    }

    for (int i = 0; i < num_choices; i++) {
        WINDOW *option_box = derwin(menu_win, 3, width - 10, 5 + i * 4, 5);
        delwin(option_box);
    }

    switch (choice) {
        case 0:
            clear();
            refresh();
            endwin();
            initialize_la_theme();
            start_game();
            initscr();
            clear();
            refresh();
            break;
        case 1: {
            int load_height = 6, load_width = 60;
            int load_y = (LINES - load_height) / 2;
            int load_x = (COLS - load_width) / 2;
            WINDOW *load_win = newwin(load_height, load_width, load_y, load_x);
            
            wattron(load_win, COLOR_PAIR(3));
            box(load_win, 0, 0);
            wattroff(load_win, COLOR_PAIR(3));
            
            wattron(load_win, COLOR_PAIR(2));
            mvwprintw(load_win, 2, (load_width - 26) / 2, "Press 'L' twice to load saved game");
            wattroff(load_win, COLOR_PAIR(2));
            
            wrefresh(load_win);
            int ch = wgetch(load_win);
            delwin(load_win);
            
            clear();
            refresh();
            endwin();
            start_game();
            initscr();
            raw();
            keypad(stdscr, TRUE);
            noecho();
            start_color();
            clear();
            refresh();
            break;
        }
        case 2:
            clear();
            displayLeaderboard();
            return;
            break;
        case 3:
            clear();
            settings_menu();
            break;
        case 4:
            clear();
            refresh();
            display_profile_menu();
            return;
    }

    delwin(menu_win);
    refresh();
    getch();
}

void initialize_player(Player *player, const char *username, const char *email) {
    strncpy(player->username, username, sizeof(player->username) - 1);
    player->username[sizeof(player->username) - 1] = '\0';
    strncpy(player->email, email, sizeof(player->email) - 1);
    player->email[sizeof(player->email) - 1] = '\0';
    
    int gold, completed_games, current_health, max_health;
    if (db_enabled && load_player_stats_db(username, &gold, &completed_games, 
                                         &current_health, &max_health)) {
        player->gold = gold;
        player->completed_games = completed_games;
        player->health.current_health = current_health;
        player->health.max_health = max_health;
    } else {
        player->gold = 0;
        player->completed_games = 0;
        player->health.current_health = 100;
        player->health.max_health = 100;
    }

    player->gold = 0;
    player->traps = 0;
    player->basic_food_count = 0;
    player->premium_food_count = 0;
    player->magical_food_count = 0;
    player->spoiled_food_count = 0;
    player->spell_count = 0;
    player->health_spells = 0;
    player->speed_spells = 0;
    player->damage_spells = 0;
    player->ancient_keys = 0;
    player->last_food_check = time(NULL);
}

void display_profile_menu(void) {
    int height = 35, width = 100;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    WINDOW *profile_win = newwin(height, width, start_y, start_x);
    keypad(profile_win, TRUE);

    start_color();

    const char *ascii_art[] = {
        "                                                 __----~~~~~~~~~~~------___",
        "                                      .  .   ~~//====......          __--~ ~~",
        "                      -.            \\_|//     |||\\\\  ~~~~~~::::... /~",
        "                   ___-==_       _-~o~  \\/    |||  \\\\            _/~~-",
        "           __---~~~.==~||\\=_    -_--~/_-~|-   |\\\\   \\\\        _/~",
        "       _-~~     .=~    |  \\\\-_    '-~7  /-   /  ||    \\      /",
        "     .~       .~       |   \\\\ -_    /  /-   /   ||      \\   /",
        "    /  ____  /         |     \\\\ ~-_/  /|- _/   .||       \\ /",
        "    |~~    ~~|--~~~~--_ \\     ~==-/   | \\~--===~~        .\\",
        "             '         ~-|      /|    |-~\\~~       __--~~",
        "                         |-~~-_/ |    |   ~\\_   _-~            /\\",
        "                              /  \\     \\__   \\/~                \\__",
        "                          _--~ _/ | .-~~____--~-/                  ~~==.",
        "                         ((->/~   '.|||' -_|    ~~-/ ,              . _||",
        "                                    -_     ~\\      ~~---l__i__i__i--~~_/",
        "                                    _-~-__   ~)  \\--______________--~~",
        "                                  //.-~~~-~_--~- |-------~~~~~~~~",
        "                                         //.-~~~--\\"
    };

    while (1) {
        werase(profile_win);

        wattron(profile_win, COLOR_PAIR(113));
        
        for (int i = 0; i < width; i++) {
            mvwaddch(profile_win, 0, i, '*');
            mvwaddch(profile_win, height-1, i, '*');
        }
        
        for (int i = 0; i < height; i++) {
            mvwaddch(profile_win, i, 0, '*');
            mvwaddch(profile_win, i, width-1, '*');
        }
        
        wattroff(profile_win, COLOR_PAIR(113));

        wattron(profile_win, COLOR_PAIR(116));
        for (int i = 0; i < sizeof(ascii_art) / sizeof(ascii_art[0]); i++) {
            mvwprintw(profile_win, i + 2, 2, "%s", ascii_art[i]);
        }
        wattroff(profile_win, COLOR_PAIR(116));

        int stat_start_y = 21;

        // Player Stats Labels
        wattron(profile_win, COLOR_PAIR(111));
        mvwprintw(profile_win, stat_start_y, 5, "Username:");
        mvwprintw(profile_win, stat_start_y + 1, 5, "Email:");
        mvwprintw(profile_win, stat_start_y + 2, 5, "Total Score:");
        mvwprintw(profile_win, stat_start_y + 3, 5, "Won Battles:");
        mvwprintw(profile_win, stat_start_y + 4, 5, "Current Floor:");
        mvwprintw(profile_win, stat_start_y + 5, 5, "Total Gold:");
        mvwprintw(profile_win, stat_start_y + 6, 5, "Health:");
        wattroff(profile_win, COLOR_PAIR(111));

        // Player Stats Values
        wattron(profile_win, COLOR_PAIR(113));
        mvwprintw(profile_win, stat_start_y, 20, "%s", current_player.username);
        mvwprintw(profile_win, stat_start_y + 1, 20, "%s", current_player.email);
        mvwprintw(profile_win, stat_start_y + 2, 20, "%d", (current_player.gold * 5) + current_player.health.current_health);
        mvwprintw(profile_win, stat_start_y + 3, 20, "%d", current_player.completed_games);
        mvwprintw(profile_win, stat_start_y + 4, 20, "%d", current_floor + 1);
        mvwprintw(profile_win, stat_start_y + 5, 20, "%d", current_player.gold);
        mvwprintw(profile_win, stat_start_y + 6, 20, "%d/%d", current_player.health.current_health, 
                                                            current_player.health.max_health);
        wattroff(profile_win, COLOR_PAIR(113));

        // Status
        wattron(profile_win, COLOR_PAIR(116));
        mvwprintw(profile_win, height-3, 5, "Status: %s", 
                 current_player_data.is_guest ? "Guest Player" : "Registered Player");
        wattroff(profile_win, COLOR_PAIR(116));

        // Controls
        wattron(profile_win, COLOR_PAIR(116));
        mvwprintw(profile_win, height-2, 5, "Press 'B' to return to main menu");
        wattroff(profile_win, COLOR_PAIR(116));

        wrefresh(profile_win);

        int ch = wgetch(profile_win);
        if (ch == 'B' || ch == 'b') {
            delwin(profile_win);
            clear();
            refresh();
            pre_game_menu();
            return;
        }
    }
}

int find_user_by_email(const char* email, char* username) {
    DIR *d;
    struct dirent *dir;
    d = opendir("player_data");
    if (!d) return 0;

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, "_stats.txt")) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "player_data/%s", dir->d_name);
            
            FILE *file = fopen(filepath, "r");
            if (!file) continue;

            char line[256];
            char stored_email[100];
            
            strncpy(username, dir->d_name, strlen(dir->d_name) - strlen("_stats.txt"));
            username[strlen(dir->d_name) - strlen("_stats.txt")] = '\0';
            
            while (fgets(line, sizeof(line), file)) {
                if (strstr(line, "Email:")) {
                    sscanf(line, "Email: %s", stored_email);
                    if (strcmp(stored_email, email) == 0) {
                        fclose(file);
                        closedir(d);
                        return 1;
                    }
                }
            }
            fclose(file);
        }
    }
    closedir(d);
    return 0;
}

int main() {
    setlocale(LC_ALL, "");
    setenv("LANG", "en_US.UTF-8", 1);
    initscr();
    cbreak();

    if (!init_database()) {
        log_message("Note: Database initialization failed, using file storage only");
    }
    
    run_anime_intro();
    init_audio();
    
    login();
    pre_game_menu();
    curs_set(0);
    refresh();
    cleanup_audio();
    endwin();
    return 0;
}