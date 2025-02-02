#include "game.h"

// glitch on the screen, makes screen stop and lose 
// monster attack system
// player pick up ability
// music for room entrance
// traps and enemy system in themed rooms

int current_floor = 0;
Floor floors[4];
MonsterList floor_monsters[4];
SpellItem collected_spells[MAX_SPELLS_PLAYER];
FoodItem stored_food[6];
int spell_in_room = 0;
int food_count = 0;
Player current_player; 
PlayerSaveData current_player_data;

const WeaponStats WEAPON_PROPERTIES[] = {
    [MACE] = {
        .damage = 5,
        .range = 1,
        .quantity = 1,
        .max_quantity = 1,
        .is_throwable = false,
        .is_permanent = true,
        .menu_char = 'm'
    },
    [DAGGER] = {
        .damage = 12,
        .range = 5,
        .quantity = 0,
        .max_quantity = 10,
        .is_throwable = true,
        .is_permanent = false,
        .menu_char = 'd'
    },
    [MAGIC_WAND] = {
        .damage = 15,
        .range = 10,
        .quantity = 0,
        .max_quantity = 8,
        .is_throwable = true,
        .is_permanent = false,
        .menu_char = 'w'
    },
    [NORMAL_ARROW] = {
        .damage = 5,
        .range = 5,
        .quantity = 0,
        .max_quantity = 20,
        .is_throwable = true,
        .is_permanent = false,
        .menu_char = 'a'
    },
    [SWORD] = {
        .damage = 10,
        .range = 1,
        .quantity = 0,
        .max_quantity = 1,
        .is_throwable = false,
        .is_permanent = true,
        .menu_char = 's'
    }
};

Monster create_monster(MonsterType type, int x, int y) {
    Monster monster = {
        .type = type,
        .x = x,
        .y = y,
        .initial_x = x,
        .initial_y = y,
        .is_alive = true,
        .is_following = false,
        .is_paralyzed = false,
        .follow_steps = 0
    };
    
    switch(type) {
        case DEMON:
            monster.max_health = 5;
            monster.damage = 5;
            break;
        case FIRE:
            monster.max_health = 10;
            monster.damage = 8;
            break;
        case GIANT:
            monster.max_health = 15;
            monster.damage = 12;
            break;
        case SNAKE:
            monster.max_health = 20;
            monster.damage = 15;
            break;
        case UNDEAD:
            monster.max_health = 30;
            monster.damage = 20;
            break;
    }
    monster.health = monster.max_health;
    return monster;
}

void colorspair(){
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_WHITE); // display messages
    init_pair(6, 50, COLOR_BLACK);
    init_pair(7, COLOR_GREEN,COLOR_BLACK);
    init_pair(8, COLOR_BLACK,190); // money money

    //character
    init_pair(20, 69, COLOR_BLACK);
    init_pair(21, 57, COLOR_BLACK);
    init_pair(22, 198, COLOR_BLACK); 
    init_pair(23, 193, COLOR_BLACK); 

    // rang jadid haha
    init_pair(101, 23, COLOR_BLACK);
    init_pair(102, 24, COLOR_BLACK);
    init_pair(103, 27, COLOR_BLACK);
    init_pair(104, 12, COLOR_BLACK);
    init_pair(105, 31, COLOR_BLACK);
    init_pair(106, 33, COLOR_BLACK);
    init_pair(107, 37, COLOR_BLACK);
    init_pair(108, 51, COLOR_BLACK); 
    init_pair(109, 52, COLOR_BLACK);
    init_pair(110, 53, COLOR_BLACK);
    init_pair(111, 56, COLOR_BLACK);
    init_pair(112, 63, COLOR_BLACK);
    init_pair(113, 69, COLOR_BLACK); 
    init_pair(114, 70, COLOR_BLACK);
    init_pair(115, 40, COLOR_BLACK); //green for password door
    init_pair(116, 86, COLOR_BLACK); 
    init_pair(117, 88, COLOR_BLACK); 
    init_pair(118, 89, COLOR_BLACK); //border
    init_pair(119, 90, COLOR_BLACK);
    init_pair(120, 97, COLOR_BLACK);
    init_pair(121, 103, COLOR_BLACK);
    init_pair(122, 100, COLOR_BLACK); // for treasure room
    init_pair(123, 141, COLOR_BLACK);
    init_pair(124, 132, COLOR_BLACK);
    init_pair(125, 169, COLOR_BLACK);
    init_pair(126, 162, COLOR_BLACK); 
    init_pair(127, 202, COLOR_BLACK); // orange
    init_pair(128, 208, COLOR_BLACK);// light orange
    init_pair(129, 214, COLOR_BLACK);
    init_pair(130, 226, COLOR_BLACK);
    init_pair(131, 227, COLOR_BLACK); 
    init_pair(132, 229, COLOR_BLACK);
    init_pair(133, 230, COLOR_BLACK);
    init_pair(134, 105, COLOR_BLACK); //enchant room
    init_pair(135,  87, COLOR_BLACK); //door
    init_pair(136,  45, COLOR_BLACK); 
    init_pair(137, 197, COLOR_BLACK); 
    init_pair(139, 48, COLOR_BLACK);
    init_pair(140, 163, COLOR_BLACK); //message test
    init_pair(141, 201, COLOR_BLACK); //message test
    init_pair(142, 190, COLOR_BLACK); //gold
    init_pair(143, 82, COLOR_BLACK); //G
    init_pair(144, 46, COLOR_BLACK); //F
    init_pair(145, 47, COLOR_BLACK); //U
    init_pair(146, 83, COLOR_BLACK); //S
    init_pair(147, 77, COLOR_BLACK); //D
}

void place_staircase(Floor *floor, Room *room, char staircase_type) {
    int staircase_x = random_generator(room->x + 1, room->x + room->width - 2);
    int staircase_y = random_generator(room->y + 1, room->y + room->height - 2);
    int attempts = 0;
    while (floor->map[staircase_y][staircase_x] == '<' || 
           floor->map[staircase_y][staircase_x] == '>' || 
           attempts < 10) {
        staircase_x = random_generator(room->x + 1, room->x + room->width - 2);
        staircase_y = random_generator(room->y + 1, room->y + room->height - 2);
        attempts++;
    }

    room->staircase_x = staircase_x;
    room->staircase_y = staircase_y;
    floor->map[staircase_y][staircase_x] = staircase_type;
}

void initialize_floor(Floor *floor, int rows, int cols) {
   floor->map = malloc(rows * sizeof(char *));
   for (int i = 0; i < rows; i++) {
       floor->map[i] = malloc(cols * sizeof(char));
       memset(floor->map[i], ' ', cols);
       for (int j = 0; j < cols; j++) {
           if (i == 0 || i == rows - 2 || j == 0 || j == cols - 1) {
               floor->map[i][j] = '*';
           }
       }
   }

    floor->visibility = malloc(rows * sizeof(bool *));
   for (int i = 0; i < rows; i++) {
       floor->visibility[i] = malloc(cols * sizeof(bool));
       memset(floor->visibility[i], false, cols);
   }
   floor->reveal_all = false;


   floor->food_count = 0;
   floor->spell_count = 0;
   floor->weapon_count = 0;

   memset(floor->rooms, 0, sizeof(Room) * 6);

   memset(floor->food_items, 0, sizeof(FoodItem) * MAX_FOOD_NUMBER * 6);
   memset(floor->spell_items, 0, sizeof(SpellItem) * MAX_SPELLS_PLAYER);
   memset(floor->weapon_items, 0, sizeof(WeaponItem) * 20);
   memset(floor->password_doors, 0, sizeof(PasswordDoor) * 6);
   memset(floor->spell_items, 0, sizeof(SpellItem) * MAX_SPELLS_PLAYER);
   memset(&floor->food_count, 0, sizeof(int));
   memset(&floor->spell_count, 0, sizeof(int));
   memset(&floor->weapon_count, 0, sizeof(int));
}

void clear_visibility(Floor *floor) {
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            floor->visibility[i][j] = false;
        }
    }
}

void update_visibility(Floor *floor, Player *player) {
    clear_visibility(floor);
    for (int dy = -4; dy <= 4; dy++) {
        for (int dx = -4; dx <= 4; dx++) {
            int new_y = player->y + dy;
            int new_x = player->x + dx;
            
            if (new_y >= 0 && new_y < MAP_ROWS && 
                new_x >= 0 && new_x < MAP_COLS) {
                double distance = sqrt(dx*dx + dy*dy);
                if (distance <= 4) {
                    floor->visibility[new_y][new_x] = true;
                }
            }
        }
    }
}

void initialize_all_floors(int rows, int cols) {
    for (int i = 0; i < 4; i++) {
        initialize_floor(&floors[i], rows, cols);
        floors[i].floor_index = i;
    }
}

void free_floor(Floor *floor, int rows) {
    for (int i = 0; i < rows; i++) {
        free(floor->visibility[i]);
        free(floor->map[i]);
    }
    free(floor->visibility);
    free(floor->map);
}

void free_all_floors(int rows) {
    for (int i = 0; i < 4; i++) {
        free_floor(&floors[i], rows);
    }
}

void render_map(Floor *floor) {
    clear();
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (!floor->reveal_all && !floor->visibility[i][j]) {
                if (floor->map[i][j] == '*') {
                    attron(COLOR_PAIR(118));
                    mvaddch(i, j, '*');
                    attroff(COLOR_PAIR(118));
                    continue;
                }
                mvaddch(i, j, ' '); 
                continue;
            }
            char ch = floor->map[i][j];
            if (ch == '*') {
                attron(COLOR_PAIR(118));  // border
                mvaddch(i, j, ch);
                attroff(COLOR_PAIR(118));
            } 
            
            else if (ch == '+') {
                attron(COLOR_PAIR(135));  // doors
                mvaddch(i, j, ch);
                attroff(COLOR_PAIR(135));
            }
            else if (ch == '=') {
                attron(COLOR_PAIR(117));
                mvaddch(i, j, '=');
                attroff(COLOR_PAIR(117));
            }
            else if (ch == '@') {
                attron(COLOR_PAIR(character_color));
                mvaddch(i, j, ch);
                attroff(COLOR_PAIR(character_color));
            }
            else if (ch == '#') {
                attron(COLOR_PAIR(110));  // corridor
                mvaddch(i, j, ch);
                attroff(COLOR_PAIR(110));
            } 
            else if (ch== '^') {
                attron(COLOR_PAIR(102));
                mvaddch(i, j, '.'); 
                attroff(COLOR_PAIR(102));
            }
            else if (ch== '%') {
                attron(COLOR_PAIR(6));
                mvaddch(i, j, '%'); 
                attroff(COLOR_PAIR(6));
            }
            else if (ch == 'd') { // Demon
                attron(COLOR_PAIR(147));
                mvaddch(i, j, 'D');
                attroff(COLOR_PAIR(147));
            }
            else if (ch == 'F') { // Fire Breathing Monster
                attron(COLOR_PAIR(144));
                mvaddch(i, j, 'F'); 
                attroff(COLOR_PAIR(144));
            }
            else if (ch == 'X') { // Giant
                attron(COLOR_PAIR(143)); 
                mvaddch(i, j, 'G'); 
                attroff(COLOR_PAIR(143));
            }
            else if (ch == 'x') { // Snake
                attron(COLOR_PAIR(146)); 
                mvaddch(i, j, 'S');
                attroff(COLOR_PAIR(146));
            }
            else if (ch == '?') {
                attron(COLOR_PAIR(6));
                mvaddch(i, j, '?');
                attroff(COLOR_PAIR(6));
            }
            else if (ch == 'U') { // Undead
                attron(COLOR_PAIR(145));
                mvaddch(i, j, 'U');
                attroff(COLOR_PAIR(145));
            }
            else if (ch == 'P') {
                PasswordDoor *door = NULL;
                for (int d = 0; d < 6; d++) {
                    if (floor->password_doors[d].x == j && floor->password_doors[d].y == i) {
                        door = &floor->password_doors[d];
                        break;
                    }
                }
                
                if (door && !door->is_locked) {
                    attron(COLOR_PAIR(7)); // Green for unlocked
                    mvaddch(i, j, '@');
                    attroff(COLOR_PAIR(7));
                } else {
                    attron(COLOR_PAIR(1)); // Red for locked
                    mvaddch(i, j, '@');
                    attroff(COLOR_PAIR(1));
                }
            }
            else if (ch == 'Q') {
                attron(COLOR_PAIR(134));
                mvaddch(i, j, '-');
                attroff(COLOR_PAIR(134));
            }
            else if (ch == '&') {
                attron(COLOR_PAIR(140)); // password key
                mvaddch(i, j, '&');
                attroff(COLOR_PAIR(140));
            }
            else if (ch == 'B') { // basic food
                mvprintw(i, j-1, "🍏");
            }
            else if (ch == 'A') { //premium food
                mvprintw(i, j-1, "🌭");
            }
            else if (ch == 'M') { // magical food
                mvprintw(i, j-1, "🍕");
            }
            else if (ch == 'G') { // spoiled food
                mvprintw(i, j, "🌶️");
            }
            else if (ch == '$') { // GOLD
                attron(COLOR_PAIR(142));
                mvaddch(i, j, '$');
                attroff(COLOR_PAIR(142));
            }
            else if (ch == 'g') { // black GOLD
                attron(COLOR_PAIR(8));
                mvaddch(i, j, '$');
                attroff(COLOR_PAIR(8));
            }
            else if (ch == 'H') { // Health spell
                mvprintw(i, j-1, "💉");
            }
            else if (ch == 'V') { // Speed spell 
                mvprintw(i, j-1, "🔥");
            }
            else if (ch == 'D') { // Damage spell
                mvprintw(i, j-1, "💀");
            }
            else if (ch == 'W') {  // Mace
                mvprintw(i, j-1, "🎯");
            }
            else if (ch == 'K') {  // Dagger
                mvprintw(i, j, "🗡️");
            }
            else if (ch == 'T') {  // Magic Wand
                mvprintw(i, j-1, "🔮");
            }
            else if (ch == 'R') {  // Normal Arrow
                mvprintw(i, j-1, "🏹");
            }
            else if (ch == 'S') {  // Sword
                mvprintw(i, j-1, "🔪");
            }
            
            else {
                bool in_enchanted_room = false;
                bool in_nightmare_room = false;
                bool in_treasure_room = false;
                
                for (int r = 0; r < 6; r++) {
                    Room *room = &floor->rooms[r];
                    if (room->theme == ENCHANTED_ROOM &&
                        i >= room->y && i < room->y + room->height &&
                        j >= room->x && j < room->x + room->width) {
                        in_enchanted_room = true;
                        break;
                    }
                    if (room->theme == TREASURE_ROOM &&
                        i >= room->y && i < room->y + room->height &&
                        j >= room->x && j < room->x + room->width) {
                        in_treasure_room = true;
                        break;
                    }
                }
                
                if (in_treasure_room) {
                    attron(COLOR_PAIR(132));  // Yellow for treasure room
                    mvaddch(i, j, ch);
                    attroff(COLOR_PAIR(132));
                }
                else if (in_enchanted_room) {
                    attron(COLOR_PAIR(134));  // purple for enchanted
                    mvaddch(i, j, ch);
                    attroff(COLOR_PAIR(134));
                }
                else if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                    for (int r = 2; r <= 4; r++) {
                        if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                            i >= floor->rooms[r].y && i < floor->rooms[r].y + floor->rooms[r].height &&
                            j >= floor->rooms[r].x && j < floor->rooms[r].x + floor->rooms[r].width) {
                            in_nightmare_room = true;
                            break;
                        }
                    }
                    if (in_nightmare_room) {
                        attron(COLOR_PAIR(103));  // Blue for nightmare
                        mvaddch(i, j, ch);
                        attroff(COLOR_PAIR(103));
                    } else {
                        attron(COLOR_PAIR(107));  // for normal rooms
                        mvaddch(i, j, ch);
                        attroff(COLOR_PAIR(107));
                    }
                } else {
                    attron(COLOR_PAIR(107));  // for normal rooms
                    mvaddch(i, j, ch);
                    attroff(COLOR_PAIR(107));
                }
            }
        }
    }                                                            
}

int  random_generator(int start, int end) {
    return rand() % (end - start + 1) + start;
}

void generate_room(int max_xi, int max_yi, int max_x, int max_y, int *x, int *y, int *width, int *height) {
    *width = random_generator(8, max_x - max_xi); 
    *height = random_generator(8, max_y - max_yi);
    *x = random_generator(max_xi, max_x - *width);
    *y = random_generator(max_yi, max_y - *height);
}

void draw_room(Floor *floor,Room *room) {
    int x, y;
    colorspair();
    for (x = room->x; x < room->x + room->width; x++) {
        floor->map[room->y][x] = '-';
        floor->map[room->y + room->height - 1][x] = '-';
    }
    for (y = room->y + 1; y < room->y + room->height - 1; y++) {
        floor->map[y][room->x] = '|';
        floor->map[y][room->x + room->width - 1] = '|';
        for (x = room->x + 1; x < room->x + room->width - 1; x++) {
            floor->map[y][x] = '.';
        }
    }
}

void place_doors(Floor *floor, Room *room, int is_edge_room, int room_id) {
    int door_count = 0;

    if (is_edge_room) {
        int door_y = random_generator(room->y + 1, room->y + room->height - 2);
        room->door_coords[door_count][0] = room->x + room->width;
        room->door_coords[door_count][1] = door_y;
        floor->map[door_y][room->x + room->width - 1] = '+';
    } else {
        int door_y = random_generator(room->y + 1, room->y + room->height - 2);
        room->door_coords[door_count][0] = room->x; 
        room->door_coords[door_count][1] = door_y;
        floor->map[door_y][room->x] = '+';
        door_count++;

        door_y = random_generator(room->y + 1, room->y + room->height - 2);
        room->door_coords[door_count][0] = room->x + room->width - 1;
        room->door_coords[door_count][1] = door_y;
        floor->map[door_y][room->x + room->width - 1] = '+';
    }

    // Secret door for rooms 1 and 6, secret door is shoen with Q
    if (room_id == 1) { // Room 1 (top-left)
        int extra_door_x = random_generator(room->x + 1, room->x + room->width - 2);
        int extra_door_y = room->y + room->height - 1;
        floor->map[extra_door_y][extra_door_x] = 'Q';
        room->door_coords[2][0] = extra_door_x;
        room->door_coords[2][1] = extra_door_y;
    }
    else if (room_id == 6) { // Room 6 (bottom-left)
        int extra_door_x = random_generator(room->x + 1, room->x + room->width - 2);
        int extra_door_y = room->y;
        floor->map[extra_door_y][extra_door_x] = 'Q';
        room->door_coords[2][0] = extra_door_x;
        room->door_coords[2][1] = extra_door_y;
    }
}

void create_corridor(Floor *floor,int x1, int y1, int x2, int y2) {  

    int pathy, xx;
    if (x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1) {
        return;
    }
    if (x1 == x2) {
        for (pathy = y1; pathy != y2; ) {
            if (floor->map[pathy][x1] == ' ') {
                floor->map[pathy][x1] = '#'; 
            }
            if (y1 < y2) 
                pathy++;
            else if (y1 > y2)
                pathy--;
        }
    }
    else if (y1 == y2) {
        for (xx = x1; xx != x2; ) {
            if (floor->map[y1][xx] == ' ') {
                floor->map[y1][xx] = '#';
            }
            if (x1 < x2) 
                xx++;
            else if (x1 > x2)
                xx--;
        }
    }
    else {
        //vertically
        for (pathy = y1; pathy != y2; ) {
            if (floor->map[pathy][x1] == ' ') {
                floor->map[pathy][x1] = '#';
            }
            if (y1 < y2) 
                pathy++;
            else if (y1 > y2)
                pathy--;
        }

        // horizontally
        for (xx = x1; xx != x2; ) {
            if (floor->map[pathy][xx] == ' ') {
                floor->map[pathy][xx] = '#';
            }
            if (x1 < x2) 
                xx++;
            else if (x1 > x2)
                xx--;
        }
    }
}

void roommaker(Floor *floor) { 
    int rx, ry, width, height;
    int scr_row, scr_col;
    getmaxyx(stdscr, scr_row, scr_col);
    int section_margin = 5;


    // Room 1: Top-left section (Enchanted)
    generate_room(section_margin, section_margin, scr_col / 3 - section_margin, scr_row / 2 - section_margin, &rx, &ry, &width, &height);
    floor->rooms[0] = (Room){rx, ry, width, height, {{0}}, 0, 0, ENCHANTED_ROOM};
    draw_room(floor,&floor->rooms[0]);
    place_doors(floor, &floor->rooms[0], 1,1);

    // Room 2: Top-middle section
    generate_room(scr_col / 3 + section_margin, section_margin, (scr_col / 3) * 2 - section_margin, scr_row / 2 - section_margin, &rx, &ry, &width, &height);
    floor->rooms[1] = (Room){rx, ry, width, height};
    draw_room(floor,&floor->rooms[1]);
    place_doors(floor,&floor->rooms[1], 0,2);
    PasswordDoor passwordDoor1;
    place_password_doors(floor, &floor->rooms[1], &floor->password_doors[1]);
    place_password_button(floor,&floor->rooms[1]);
    place_windows(floor, &floor->rooms[1], 1);

    // Room 3: Top-right section
    generate_room((scr_col / 3) * 2 + section_margin, section_margin, scr_col - section_margin, scr_row / 2 - section_margin, &rx, &ry, &width, &height);
    floor->rooms[2] = (Room){rx, ry, width, height};
    draw_room(floor,&floor->rooms[2]);
    place_doors(floor,&floor->rooms[2], 0,3);

    // Room 4: Bottom-right section
    generate_room((scr_col / 3) * 2 + section_margin, scr_row / 2 + section_margin, scr_col - section_margin, scr_row - section_margin, &rx, &ry, &width, &height);
    floor->rooms[3] = (Room){rx, ry, width, height};
    draw_room(floor,&floor->rooms[3]);
    place_doors(floor,&floor->rooms[3], 0,4);
    PasswordDoor passwordDoor2;
    place_password_doors(floor, &floor->rooms[3], &floor->password_doors[3]);
    place_password_button(floor,&floor->rooms[3]);
    
    if (floor->floor_index == 1 || floor->floor_index == 2) {  // Floors 2 and 3
        int nightmare_room = random_generator(2, 4);  // Random room between 2-4
        floor->rooms[nightmare_room].theme = NIGHTMARE_ROOM;
    }

    // Room 5: Bottom-middle section
    generate_room(scr_col / 3 + section_margin, scr_row / 2 + section_margin, (scr_col / 3) * 2 - section_margin, scr_row - section_margin, &rx, &ry, &width, &height);
    floor->rooms[4] = (Room){rx, ry, width, height, {{0}}, 0, 0, NORMAL_ROOM};
    draw_room(floor, &floor->rooms[4]);
    place_doors(floor,&floor->rooms[4], 0,5);

    // Room 6: Bottom-left section (Enchanted)
    generate_room(section_margin, scr_row / 2 + section_margin, scr_col / 3 - section_margin, scr_row - section_margin, &rx, &ry, &width, &height);
    floor->rooms[5] = (Room){rx, ry, width, height, {{0}}, 0, 0, ENCHANTED_ROOM};
    draw_room(floor,&floor->rooms[5]);
    place_doors(floor,&floor->rooms[5], 1,6);

    if (floor->floor_index < 3) {
        place_staircase(floor, &floor->rooms[5], '>');
    }
    
    if (floor->floor_index > 0) {
        place_staircase(floor, &floor->rooms[0], '<');
    }

    // R1 to R2
    create_corridor(floor,floor->rooms[0].door_coords[0][0], floor->rooms[0].door_coords[0][1], floor->rooms[1].door_coords[0][0], floor->rooms[1].door_coords[0][1]);
    create_corridor(floor,floor->rooms[0].door_coords[0][0], floor->rooms[0].door_coords[0][1], floor->rooms[1].door_coords[2][0], floor->rooms[1].door_coords[2][1]);
    //R2 to R3
    create_corridor(floor,floor->rooms[1].door_coords[1][0] +1, floor->rooms[1].door_coords[1][1], floor->rooms[2].door_coords[0][0], floor->rooms[2].door_coords[0][1]);
    // R3 to R4
    if(floor->rooms[3].door_coords[1][0] > floor->rooms[2].door_coords[1][0]){
        create_corridor(floor,floor->rooms[3].door_coords[1][0] +1, floor->rooms[3].door_coords[1][1], floor->rooms[2].door_coords[1][0], floor->rooms[2].door_coords[1][1]);
    }
    else {create_corridor(floor,floor->rooms[2].door_coords[1][0] +1, floor->rooms[2].door_coords[1][1], floor->rooms[3].door_coords[1][0], floor->rooms[3].door_coords[1][1]);}
    // R4 to R5
    create_corridor(floor,floor->rooms[4].door_coords[1][0] +1, floor->rooms[4].door_coords[1][1], floor->rooms[3].door_coords[0][0], floor->rooms[3].door_coords[0][1]);
    create_corridor(floor,floor->rooms[4].door_coords[1][0] +1, floor->rooms[4].door_coords[1][1], floor->rooms[3].door_coords[2][0], floor->rooms[3].door_coords[2][1]);
    // R5 to R6
    create_corridor(floor,floor->rooms[5].door_coords[0][0], floor->rooms[5].door_coords[0][1], floor->rooms[4].door_coords[0][0], floor->rooms[4].door_coords[0][1]);

    if(floor->rooms[0].door_coords[2][0] > floor->rooms[5].door_coords[2][0]){
        create_corridor(floor, 
            floor->rooms[5].door_coords[2][0], floor->rooms[5].door_coords[2][1], 
            floor->rooms[0].door_coords[2][0]+1, floor->rooms[0].door_coords[2][1]+1);
    }
    else {
        create_corridor(floor, 
            floor->rooms[5].door_coords[2][0], floor->rooms[5].door_coords[2][1], 
            floor->rooms[0].door_coords[2][0]-1, floor->rooms[0].door_coords[2][1]+1
        );
    }

    for (int i = 0; i < 6; i++) {
        floor->rooms[i].visited = false;
        floor->rooms[i].window_count = 0;
        place_traps(floor, &floor->rooms[i]);
        place_food(floor, &floor->rooms[i], stored_food, &food_count);
        place_gold(floor, &floor->rooms[i]);
        place_weapons(floor, &floor->rooms[i]);
        if (i >= 2 && i < 5) {
            place_black_gold(floor, &floor->rooms[i]);
            place_monsters(floor, &floor->rooms[i]); 
        }
        place_spells(floor, &floor->rooms[i], collected_spells, &spell_in_room);
    }
    if (floor->floor_index == 3) {
        int random_room = random_generator(0, 5);
        place_question_mark(floor, &floor->rooms[random_room]);
    }
    int random_room = random_generator(0, 5);
    place_ancient_key(floor, &floor->rooms[random_room]);
    render_map(floor);
    refresh();
}

void move_player(int ch, Player *player, Floor *floor, int scr_row, int scr_col) {  
    // M key
    if (ch == 'M' || ch == 'm') {
        floor->reveal_all = !floor->reveal_all;
        show_message_for_2_seconds(floor->reveal_all ? 
            "Map Revealed!" : "Map Hidden!");
        render_map(floor);
        attron(COLOR_PAIR(character_color));
        mvaddch(player->y, player->x, '@');
        attroff(COLOR_PAIR(character_color));
        refresh();
        return;
    }

    check_windows(floor, player);
    update_room_visibility(floor, player);
    if (player->health.speed_spell_active) {
        move_player_speed(ch, player, floor, scr_row, scr_col);
        return;
    }
    
    if (floor->map[player->y][player->x] != '#' && floor->map[player->y][player->x] != '+' 
        && floor->map[player->y][player->x] != '&' && floor->map[player->y][player->x] != 's'
        && floor->map[player->y][player->x] != '<' && floor->map[player->y][player->x] != '>'
        && floor->map[player->y][player->x] != '@' && floor->map[player->y][player->x] != '=') {
        mvaddch(player->y, player->x, '.');
    }
    else if (floor->map[player->y][player->x]== '#') {
        mvaddch(player->y, player->x, '#');
    }
    else if (floor->map[player->y][player->x] == '+') {
        mvaddch(player->y, player->x, '+');
    }
    else if (floor->map[player->y][player->x] == 's') {
        mvaddch(player->y, player->x, 's');
    }
    else if (floor->map[player->y][player->x] == '&') {
        mvaddch(player->y, player->x, '&');
    }
    else if (floor->map[player->y][player->x] == 'P') {
        mvaddch(player->y, player->x, '@');
    }
    else if (floor->map[player->y][player->x] == '>') {
        mvaddch(player->y, player->x, '>');
    }
    else if (floor->map[player->y][player->x] == '=') {
        mvaddch(player->y, player->x, '=');
    }
    else if (floor->map[player->y][player->x] == '<') {
        mvaddch(player->y, player->x, '<');
    }
    else if (floor->map[player->y][player->x] == '@') {
        attron(COLOR_PAIR(7));
        mvaddch(player->y, player->x, '@');
        attroff(COLOR_PAIR(7));
    }
    
    int new_x = player->x;
    int new_y = player->y;
    update_visibility(floor, player);
    update_room_visibility(floor, player);
    render_map(floor);
    refresh();

    check_secret_doors(floor, player);

    if ((ch == 'J' || ch == 'j') && player->y > 1) { // Move up
        new_y--;
    }
    else if ((ch == 'K' || ch == 'k') && player->y < scr_row - 3) { // Move down
        new_y++;
    }
    else if ((ch == 'H' || ch == 'h') && player->x > 1) { // Move left
        new_x--;
    }
    else if ((ch == 'L' || ch == 'l') && player->x < scr_col - 2) { // Move right
        new_x++;
    }
    else if ((ch == 'Y' || ch == 'y') && player->y > 1 && player->x > 1) { // Move up-left
        new_y--;
        new_x--;
    }
    else if ((ch == 'U' || ch == 'u') && player->y > 1 && player->x < scr_col - 2) { // Move up-right
        new_y--;
        new_x++;
    }
    else if ((ch == 'B' || ch == 'b') && player->y < scr_row - 3 && player->x > 1) { // Move down-left
        new_y++;
        new_x--;
    }
    else if ((ch == 'N' || ch == 'n') && player->y < scr_row - 3 && player->x < scr_col - 2) { // Move down-right
        new_y++;
        new_x++;
    }
    
    if(ch == KEY_RIGHT) {  // Try to go up a floor
        if (floor->map[player->y][player->x] == '>' && current_floor < 3) {
            current_floor++;
            Room *first_room = &floors[current_floor].rooms[0];
            for (int y = first_room->y; y < first_room->y + first_room->height; y++) {
                for (int x = first_room->x; x < first_room->x + first_room->width; x++) {
                    if (floors[current_floor].map[y][x] == '<') {
                        player->x = x;
                        player->y = y;
                        goto found_stair;
                    }
                }
            }
found_stair:
            clear();
            update_visibility(&floors[current_floor], player);
            update_room_visibility(floor, player);
            render_map(&floors[current_floor]);
            display_floor_name(current_floor);
            refresh();
            return;
        }
    }
            
    else if(ch == KEY_LEFT) {   // Try to go down a floor
        if (floor->map[player->y][player->x] == '<' && current_floor > 0) {
            current_floor--;
            Room *last_room = &floors[current_floor].rooms[5];
            for (int y = last_room->y; y < last_room->y + last_room->height; y++) {
                for (int x = last_room->x; x < last_room->x + last_room->width; x++) {
                    if (floors[current_floor].map[y][x] == '>') {
                        player->x = x;
                        player->y = y;
                        goto found_stair_down;
                    }
                }
            }
found_stair_down:
            clear();
            update_visibility(&floors[current_floor], player);
            update_room_visibility(floor, player);
            render_map(&floors[current_floor]);
            display_floor_name(current_floor);
            refresh();
            return;
        }
    }
    else if (ch == 'f' || ch == 'F') {
       int continuous_move = getch();
        if (continuous_move == 'J' || continuous_move == 'j') {  // Move up
            while (floor->map[player->y - 1][player->x] == '.' || 
                    floor->map[player->y - 1][player->x] == '#' ||
                    floor->map[player->y - 1][player->x] == '$' ||
                    floor->map[player->y - 1][player->x] == 'g' ||
                    strchr("BAMG", floor->map[player->y - 1][player->x]) ||
                    strchr("WKTRS", floor->map[player->y - 1][player->x]) ||
                    strchr("HVD", floor->map[player->y - 1][player->x])) {
                char current_tile = floor->map[player->y][player->x];
                new_y--;
                
                // Gold collection
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                // Food collection
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_up;
                            }
                        }
                    }
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                // Weapon collection
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection;
                            }
                        }
                    }
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                // Spell collection
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection;
                            }
                        }
                    }
                    for (int i = 0; i < floor->spell_count; i++) {
                        if (new_x == floor->spell_items[i].x && new_y == floor->spell_items[i].y && !floor->spell_items[i].collected) {
                            floor->spell_items[i].collected = true;
                            switch(floor->spell_items[i].type) {
                                case SPELL_HEALTH:
                                    player->health_spells++;
                                    show_message_for_2_seconds("Health Spell Collected! (💉)");
                                    break;
                                case SPELL_SPEED:
                                    player->speed_spells++;
                                    show_message_for_2_seconds("Speed Spell Collected! (🔥)");
                                    break;
                                case SPELL_DAMAGE:
                                    player->damage_spells++;
                                    show_message_for_2_seconds("Damage Spell Collected! (💀)");
                                    break;
                            }
                            player->collected_spells[player->spell_count] = floor->spell_items[i];
                            player->spell_count++;
                            floor->map[new_y][new_x] = '.';
                            break;
                        }
                    }
                }

            skip_collection_up:
                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->y = new_y;
                refresh();
                napms(15);
            }
        }
       else if (continuous_move == 'K' || continuous_move == 'k') {  // Move down
            while (floor->map[player->y + 1][player->x] == '.' || 
                floor->map[player->y + 1][player->x] == '#' ||
                floor->map[player->y + 1][player->x] == '$' ||
                floor->map[player->y + 1][player->x] == 'g' ||
                strchr("BAMG", floor->map[player->y + 1][player->x]) ||
                strchr("WKTRS", floor->map[player->y + 1][player->x]) ||
                strchr("HVD", floor->map[player->y + 1][player->x])) {
                char current_tile = floor->map[player->y][player->x];
                new_y++;
                
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_down;
                            }
                        }
                    }
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_down;
                            }
                        }
                    }
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_down;
                            }
                        }
                    }
                    for (int i = 0; i < floor->spell_count; i++) {
                        if (new_x == floor->spell_items[i].x && new_y == floor->spell_items[i].y && !floor->spell_items[i].collected) {
                            floor->spell_items[i].collected = true;
                            switch(floor->spell_items[i].type) {
                                case SPELL_HEALTH:
                                    player->health_spells++;
                                    show_message_for_2_seconds("Health Spell Collected! (💉)");
                                    break;
                                case SPELL_SPEED:
                                    player->speed_spells++;
                                    show_message_for_2_seconds("Speed Spell Collected! (🔥)");
                                    break;
                                case SPELL_DAMAGE:
                                    player->damage_spells++;
                                    show_message_for_2_seconds("Damage Spell Collected! (💀)");
                                    break;
                            }
                            player->collected_spells[player->spell_count] = floor->spell_items[i];
                            player->spell_count++;
                            floor->map[new_y][new_x] = '.';
                            break;
                        }
                    }
                }

        skip_collection_down:
                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->y = new_y;
                refresh();
                napms(15);
            }
        }
        else if (continuous_move == 'H' || continuous_move == 'h') {  // Move left
            while (floor->map[player->y][player->x - 1] == '.' || 
                floor->map[player->y][player->x - 1] == '#' ||
                floor->map[player->y][player->x - 1] == '$' ||
                floor->map[player->y][player->x - 1] == 'g' ||
                strchr("BAMG", floor->map[player->y][player->x - 1]) ||
                strchr("WKTRS", floor->map[player->y][player->x - 1]) ||
                strchr("HVD", floor->map[player->y][player->x - 1])) {
                char current_tile = floor->map[player->y][player->x];
                new_x--;
                
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_left;
                            }
                        }
                    }
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_left;
                            }
                        }
                    }
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_left;
                            }
                        }
                    }
                    for (int i = 0; i < floor->spell_count; i++) {
                        if (new_x == floor->spell_items[i].x && new_y == floor->spell_items[i].y && !floor->spell_items[i].collected) {
                            floor->spell_items[i].collected = true;
                            switch(floor->spell_items[i].type) {
                                case SPELL_HEALTH:
                                    player->health_spells++;
                                    show_message_for_2_seconds("Health Spell Collected! (💉)");
                                    break;
                                case SPELL_SPEED:
                                    player->speed_spells++;
                                    show_message_for_2_seconds("Speed Spell Collected! (🔥)");
                                    break;
                                case SPELL_DAMAGE:
                                    player->damage_spells++;
                                    show_message_for_2_seconds("Damage Spell Collected! (💀)");
                                    break;
                            }
                            player->collected_spells[player->spell_count] = floor->spell_items[i];
                            player->spell_count++;
                            floor->map[new_y][new_x] = '.';
                            break;
                        }
                    }
                }

        skip_collection_left:
                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->x = new_x;
                refresh();
                napms(15);
            }
        }
        else if (continuous_move == 'L' || continuous_move == 'l') {  // Move right
            while (floor->map[player->y][player->x + 1] == '.' || 
                    floor->map[player->y][player->x + 1] == '#' ||
                    floor->map[player->y][player->x + 1] == '$' ||
                    floor->map[player->y][player->x + 1] == 'g' ||
                    strchr("BAMG", floor->map[player->y][player->x + 1]) ||
                    strchr("WKTRS", floor->map[player->y][player->x + 1]) ||
                    strchr("HVD", floor->map[player->y][player->x + 1])) {
                char current_tile = floor->map[player->y][player->x];
                new_x++;
                
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_right;
                            }
                        }
                    }
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_right;
                            }
                        }
                    }
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection_right;
                            }
                        }
                    }
                    for (int i = 0; i < floor->spell_count; i++) {
                        if (new_x == floor->spell_items[i].x && new_y == floor->spell_items[i].y && !floor->spell_items[i].collected) {
                            floor->spell_items[i].collected = true;
                            switch(floor->spell_items[i].type) {
                                case SPELL_HEALTH:
                                    player->health_spells++;
                                    show_message_for_2_seconds("Health Spell Collected! 💉");
                                    break;
                                case SPELL_SPEED:
                                    player->speed_spells++;
                                    show_message_for_2_seconds("Speed Spell Collected! 🔥");
                                    break;
                                case SPELL_DAMAGE:
                                    player->damage_spells++;
                                    show_message_for_2_seconds("Damage Spell Collected! 💀");
                                    break;
                            }
                            player->collected_spells[player->spell_count] = floor->spell_items[i];
                            player->spell_count++;
                            floor->map[new_y][new_x] = '.';
                            break;
                        }
                    }
                }

            skip_collection_right:
                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->x = new_x;
                refresh();
                napms(15);
            }
        }
        else if (continuous_move == 'Y' || continuous_move == 'y') {  // Move up-left
            while (floor->map[player->y - 1][player->x - 1] == '.' || 
                floor->map[player->y - 1][player->x - 1] == '#' ||
                floor->map[player->y - 1][player->x - 1] == '$' ||
                floor->map[player->y - 1][player->x - 1] == 'g' ||
                strchr("BAMG", floor->map[player->y - 1][player->x - 1]) ||
                strchr("WKTRS", floor->map[player->y - 1][player->x - 1]) ||
                strchr("HVD", floor->map[player->y - 1][player->x - 1])) {
                char current_tile = floor->map[player->y][player->x];
                new_y--;
                new_x--;
                
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    collect_spell(floor, player, &floor->spell_items[0], &floor->spell_count);
                    floor->map[new_y][new_x] = '.';
                }

                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->y = new_y;
                player->x = new_x;
                refresh();
                napms(15);
            }
        }
        else if (continuous_move == 'U' || continuous_move == 'u') {  // Move up-right
            while (floor->map[player->y - 1][player->x + 1] == '.' || 
                floor->map[player->y - 1][player->x + 1] == '#' ||
                floor->map[player->y - 1][player->x + 1] == '$' ||
                floor->map[player->y - 1][player->x + 1] == 'g' ||
                strchr("BAMG", floor->map[player->y - 1][player->x + 1]) ||
                strchr("WKTRS", floor->map[player->y - 1][player->x + 1]) ||
                strchr("HVD", floor->map[player->y - 1][player->x + 1])) {
                char current_tile = floor->map[player->y][player->x];
                new_y--;
                new_x++;
                
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    collect_spell(floor, player, &floor->spell_items[0], &floor->spell_count);
                    floor->map[new_y][new_x] = '.';
                }

                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->y = new_y;
                player->x = new_x;
                refresh();
                napms(15);
            }
        }
        else if (continuous_move == 'B' || continuous_move == 'b') {  // Move down-left
            while (floor->map[player->y + 1][player->x - 1] == '.' || 
                floor->map[player->y + 1][player->x - 1] == '#' ||
                floor->map[player->y + 1][player->x - 1] == '$' ||
                floor->map[player->y + 1][player->x - 1] == 'g' ||
                strchr("BAMG", floor->map[player->y + 1][player->x - 1]) ||
                strchr("WKTRS", floor->map[player->y + 1][player->x - 1]) ||
                strchr("HVD", floor->map[player->y + 1][player->x - 1])) {
                char current_tile = floor->map[player->y][player->x];
                new_y++;
                new_x--;
                
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    collect_spell(floor, player, &floor->spell_items[0], &floor->spell_count);
                    floor->map[new_y][new_x] = '.';
                }

                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->y = new_y;
                player->x = new_x;
                refresh();
                napms(15);
            }
        }
        else if (continuous_move == 'N' || continuous_move == 'n') {  // Move down-right
            while (floor->map[player->y + 1][player->x + 1] == '.' || 
                floor->map[player->y + 1][player->x + 1] == '#' ||
                floor->map[player->y + 1][player->x + 1] == '$' ||
                floor->map[player->y + 1][player->x + 1] == 'g' ||
                strchr("BAMG", floor->map[player->y + 1][player->x + 1]) ||
                strchr("WKTRS", floor->map[player->y + 1][player->x + 1]) ||
                strchr("HVD", floor->map[player->y + 1][player->x + 1])) {
                char current_tile = floor->map[player->y][player->x];
                new_y++;
                new_x++;
                
                // Gold collection
                if (floor->map[new_y][new_x] == '$' || floor->map[new_y][new_x] == 'g') {
                    if (floor->map[new_y][new_x] == '$') player->gold++;
                    if (floor->map[new_y][new_x] == 'g') player->gold += 5;
                    floor->map[new_y][new_x] = '.';
                }
                
                // Food collection
                if (strchr("BAMG", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection;
                            }
                        }
                    }
                    collect_food(player, floor, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                // Weapon collection
                if (strchr("WKTRS", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection;
                            }
                        }
                    }
                    collect_weapon(floor, player, new_x, new_y);
                    floor->map[new_y][new_x] = '.';
                }
                
                // Spell collection
                if (strchr("HVD", floor->map[new_y][new_x])) {
                    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
                        for (int r = 2; r <= 4; r++) {
                            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                                new_x >= floor->rooms[r].x && new_x < floor->rooms[r].x + floor->rooms[r].width &&
                                new_y >= floor->rooms[r].y && new_y < floor->rooms[r].y + floor->rooms[r].height) {
                                show_message_for_2_seconds("It was just an illusion!");
                                floor->map[new_y][new_x] = '.';
                                goto skip_collection;
                            }
                        }
                    }
                    for (int i = 0; i < floor->spell_count; i++) {
                        if (new_x == floor->spell_items[i].x && new_y == floor->spell_items[i].y && !floor->spell_items[i].collected) {
                            floor->spell_items[i].collected = true;
                            switch(floor->spell_items[i].type) {
                                case SPELL_HEALTH:
                                    player->health_spells++;
                                    show_message_for_2_seconds("Health Spell Collected! 💉");
                                    break;
                                case SPELL_SPEED:
                                    player->speed_spells++;
                                    show_message_for_2_seconds("Speed Spell Collected! 🔥");
                                    break;
                                case SPELL_DAMAGE:
                                    player->damage_spells++;
                                    show_message_for_2_seconds("Damage Spell Collected! 💀");
                                    break;
                            }
                            player->collected_spells[player->spell_count] = floor->spell_items[i];
                            player->spell_count++;
                            floor->map[new_y][new_x] = '.';
                            break;
                        }
                    }
                }

        skip_collection:
                mvaddch(player->y, player->x, current_tile);
                attron(COLOR_PAIR(character_color));
                mvaddch(player->y, player->x, '@');
                attroff(COLOR_PAIR(character_color));
                player->y = new_y;
                player->x = new_x;
                refresh();
                napms(15);
            }
        }
   }
    if (ch == 's' || ch == 'S') { // s movement
        handle_scan_mode(ch, player, floor);
        return;
    }
    else if (ch == '%') { //use ancient key
        int door_x, door_y;
        if (is_adjacent_to_password_door(floor, player, &door_x, &door_y)) {
            if (player->ancient_keys > 0) {
                use_ancient_key(floor, player, door_x, door_y);
            } else {
                show_message_for_2_seconds("No ancient keys available!");
            }
        } else {
            show_message_for_2_seconds("No password door nearby!");
        }
        return;
    }
    // For continuous movement when 'g' is pressed
    else if (ch == 'g' || ch == 'G') {
    int continuous_move = getch();
    if (continuous_move == 'J' || continuous_move == 'j') {  // Move up
        while (floor->map[player->y - 1][player->x] == '.' || 
               floor->map[player->y - 1][player->x] == '#') {
            char current_tile = floor->map[player->y][player->x];
            new_y--;  // Move up
            mvaddch(player->y, player->x, current_tile);
            mvaddch(new_y, new_x, '@');
            player->y = new_y;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'K' || continuous_move == 'k') {  // Move down
        while (floor->map[player->y + 1][player->x] == '.' || 
               floor->map[player->y + 1][player->x] == '#') {
            char current_tile = floor->map[player->y][player->x];
            new_y++;  // Move down
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->y = new_y;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'H' || continuous_move == 'h') {  // Move left
        while (floor->map[player->y][player->x - 1] == '.' || 
               floor->map[player->y][player->x - 1] == '#') {
            char current_tile = floor->map[player->y][player->x];
            new_x--;  // Move left
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->x = new_x;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'L' || continuous_move == 'l') {  // Move right
        while (floor->map[player->y][player->x + 1] == '.' || 
               floor->map[player->y][player->x + 1] == '#') {
            char current_tile = floor->map[player->y][player->x];
            new_x++;  // Move right
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->x = new_x;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'Y' || continuous_move == 'y') {  // Move up-left
        while ((floor->map[player->y - 1][player->x - 1] == '.' || 
                floor->map[player->y - 1][player->x - 1] == '#')) {
            char current_tile = floor->map[player->y][player->x];
            new_y--;
            new_x--;
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->y = new_y;
            player->x = new_x;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'U' || continuous_move == 'u') {  // Move up-right
        while ((floor->map[player->y - 1][player->x + 1] == '.' || 
                floor->map[player->y - 1][player->x + 1] == '#')) {
            char current_tile = floor->map[player->y][player->x];
            new_y--;
            new_x++;
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->y = new_y;
            player->x = new_x;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'B' || continuous_move == 'b') {  // Move down-left
        while ((floor->map[player->y + 1][player->x - 1] == '.' || 
                floor->map[player->y + 1][player->x - 1] == '#')) {
            char current_tile = floor->map[player->y][player->x];
            new_y++;
            new_x--;
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->y = new_y;
            player->x = new_x;
            refresh();
            napms(15);
        }
    }
    else if (continuous_move == 'N' || continuous_move == 'n') {  // Move down-right
        while ((floor->map[player->y + 1][player->x + 1] == '.' || 
                floor->map[player->y + 1][player->x + 1] == '#')) {
            char current_tile = floor->map[player->y][player->x];
            new_y++;
            new_x++;
            mvaddch(player->y, player->x, current_tile);
            attron(COLOR_PAIR(character_color));
            mvaddch(player->y, player->x, '@');
            attroff(COLOR_PAIR(character_color));
            player->y = new_y;
            player->x = new_x;
            refresh();
            napms(15);
        }
    }
}
    if (floor->map[new_y][new_x] == 'P') {
        check_password_door(floor, player, new_x, new_y);
        if (floor->map[new_y][new_x] == '@') {  // Door was unlocked
            player->x = new_x;
            player->y = new_y;
        }
    }
    else if (can_move_to_position(floor, new_x, new_y)){
        player->x = new_x;
        player->y = new_y;
    }
    check_windows(floor, player);

    if (floor->map[new_y][new_x] == '^') {
        int trap_damage = 5;
        take_damage(&player->health, trap_damage);
        
        mvaddch(player->y, player->x, '^');
        refresh();
        napms(1000);
        attron(COLOR_PAIR(character_color));
        mvaddch(player->y, player->x, '@');
        attroff(COLOR_PAIR(character_color));
        refresh();
        
        char trap_msg[100];
        snprintf(trap_msg, sizeof(trap_msg), "Trap sprung! You take %d damage!", trap_damage);
        show_message_for_2_seconds(trap_msg);
        
        player->traps++;
    }
    if (floor->map[new_y][new_x] == '&') {
        mvaddch(player->y, player->x, '&');
        generate_password(&floor->password_doors[1]); // For room 2
        generate_password(&floor->password_doors[3]); // For room 4
        display_password_for_30_seconds(floor, &floor->password_doors[1]);
    }
    //spell
    if (strchr("HVD", floor->map[new_y][new_x])) {
        check_and_collect_spell(floor, player, new_x, new_y);
    }
    // 
    if (floor->map[new_y][new_x] == '?') {
        create_treasure_room(floor, player);
        update_visibility(floor, player);
        update_room_visibility(floor, player);
        render_map(floor);
        refresh();
        return;
    }
    // food:
    char food_type = floor->map[new_y][new_x];
    if (food_type == 'B' || food_type == 'A' || food_type == 'M' || food_type == 'G') {
        const char* food_names[] = {
            [FOOD_BASIC] = "basic food",
            [FOOD_PREMIUM] = "premium food",
            [FOOD_MAGICAL] = "magical food",
            [FOOD_SPOILED] = "spoiled food"
        };
        
        FoodType type;
        switch (food_type) {
            case 'B': type = FOOD_BASIC; break;
            case 'A': type = FOOD_PREMIUM; break;
            case 'M': type = FOOD_MAGICAL; break;
            case 'G': type = FOOD_SPOILED; break;
            default: type = FOOD_BASIC;
        }
        
        if (show_collection_prompt(food_names[type])) {
            collect_food(player, floor, new_x, new_y);
        }
    }


    // weapons:
    char weapon_type = floor->map[new_y][new_x];
    if (weapon_type == 'W' || weapon_type == 'K' || weapon_type == 'T' || 
        weapon_type == 'R' || weapon_type == 'S') {
        const char* weapon_names[] = {
            [MACE] = "mace",
            [DAGGER] = "dagger",
            [MAGIC_WAND] = "magic wand",
            [NORMAL_ARROW] = "arrow",
            [SWORD] = "sword"
        };
        
        WeaponType type;
        switch (weapon_type) {
            case 'W': type = MACE; break;
            case 'K': type = DAGGER; break;
            case 'T': type = MAGIC_WAND; break;
            case 'R': type = NORMAL_ARROW; break;
            case 'S': type = SWORD; break;
            default: type = MACE;
        }
        
        if (show_collection_prompt(weapon_names[type])) {
            collect_weapon(floor, player, new_x, new_y);
        }
    }

    if (floor->map[new_y][new_x] == '$') {
        if (show_collection_prompt("gold")) {
            player->gold++;
            mvaddch(player->y, player->x, '.');
            floor->map[new_y][new_x] = '.';
            show_message_for_2_seconds("Gold collected! Gold +1");
        }
    }
    if (floor->map[new_y][new_x] == 'g') {
        if (show_collection_prompt("black gold")) {
            player->gold += 5;
            mvaddch(player->y, player->x, '.');
            floor->map[new_y][new_x] = '.';
            show_message_for_2_seconds("Black Gold collected! Gold +5");
        }
    }
    if (floor->map[new_y][new_x] == '%') {
        if (show_collection_prompt("ancient key")) {
            collect_ancient_key(player, floor, new_x, new_y);
        }
    }
    attron(COLOR_PAIR(character_color));
    mvaddch(player->y, player->x, '@');
    attroff(COLOR_PAIR(character_color));
    move(player->y, player->x);
}

void check_secret_doors(Floor *floor, Player *player) {
    // Check for Room 1 (top-left)
    if (floor->rooms[0].door_coords[2][0] == player->x && 
        floor->rooms[0].door_coords[2][1] == player->y + 1 && 
        floor->map[player->y + 1][player->x] == 'Q') {
        floor->map[player->y + 1][player->x] = '+';
        attron(COLOR_PAIR(2));
        mvaddch(player->y + 1, player->x, '+');
        attroff(COLOR_PAIR(2));
    }
    // Check for Room 6 (bottom-left)
    else if (floor->rooms[5].door_coords[2][0] == player->x && 
             floor->rooms[5].door_coords[2][1] == player->y - 1 && 
             floor->map[player->y - 1][player->x] == 'Q') {
        floor->map[player->y - 1][player->x] = '+';
        attron(COLOR_PAIR(2));
        mvaddch(player->y - 1, player->x, '+');
        attroff(COLOR_PAIR(2));
    }
}

void place_traps(Floor *floor, Room *room) {  
        // More traps for treasure room
        int num_traps = (room->theme == TREASURE_ROOM) ? 
                       random_generator(2, 4) : 
                       random_generator(0, 1);
        
        for (int i = 0; i < num_traps; i++) {
            int trap_x = random_generator(room->x + 1, room->x + room->width - 2);
            int trap_y = random_generator(room->y + 1, room->y + room->height - 2);
            if (floor->map[trap_y][trap_x] == '.') {
                floor->map[trap_y][trap_x] = '^'; 
            } else {
                i--;
            }
        }
    }

void place_food(Floor *floor, Room *room, FoodItem *food_list, int *food_count) {  
    int num_food = random_generator(0, MAX_FOOD_NUMBER);
    
    for (int i = 0; i < num_food; i++) {
        int food_x = random_generator(room->x + 1, room->x + room->width - 2);
        int food_y = random_generator(room->y + 1, room->y + room->height - 2);
        
        if (floor->map[food_y][food_x] == '.') {
            FoodType random_type = random_generator(0, 3); // FOOD_BASIC to FOOD_SPOILED
            FoodItem new_food = {
                .type = random_type,
                .x = food_x,
                .y = food_y,
                .collected = false,
                .isSpoiled = false
            };
            
            switch (random_type) {
                case FOOD_BASIC:
                    new_food.healthRestore = 10; // Basic food
                    break;
                case FOOD_PREMIUM:
                    new_food.healthRestore = 15; // Premium food
                    break;
                case FOOD_MAGICAL:
                    new_food.healthRestore = 25; // Magical food
                    break;
                case FOOD_SPOILED:
                    new_food.healthRestore = -10; 
                    new_food.isSpoiled = true;
                    break;
            }
            
            food_list[*food_count] = new_food;
            (*food_count)++;
            
            char food_char;
            switch (random_type) {
                case FOOD_BASIC:
                    food_char = 'B'; // Basic food
                    break;
                case FOOD_PREMIUM:
                    food_char = 'A'; // Premium food
                    break;
                case FOOD_MAGICAL:
                    food_char = 'M'; // Magical food
                    break;
                case FOOD_SPOILED:
                    food_char = 'G'; // Spoiled food
                    break;
            }
            
            floor->map[food_y][food_x] = food_char;
        } else {
            i--;
        }
    }
}

void place_gold(Floor *floor, Room *room) {  
    int num_traps = random_generator(0, 3); 
    for (int i = 0; i < num_traps; i++) {
        int trap_x = random_generator(room->x + 1, room->x + room->width - 2);
        int trap_y = random_generator(room->y + 1, room->y + room->height - 2);
        if (floor->map[trap_y][trap_x] == '.') {
            floor->map[trap_y][trap_x] = '$'; 
        } else {
            i--;
        }
    }
}

void place_black_gold(Floor *floor, Room *room) {  
    int num_traps = random_generator(0, 1); 
    for (int i = 0; i < num_traps; i++) {
        int trap_x = random_generator(room->x + 1, room->x + room->width - 2);
        int trap_y = random_generator(room->y + 1, room->y + room->height - 2);
        if (floor->map[trap_y][trap_x] == '.') {
            floor->map[trap_y][trap_x] = 'g'; 
        } else {
            i--;
        }
    }
}

void place_password_doors(Floor *floor, Room *room, PasswordDoor *password_door) {  // p for password door , password doors only on the left wall of Room 2 & 4
    bool valid_position_found = false;
    int max_retries = 10;
    int retries = 0;
    while (!valid_position_found && retries < max_retries) {
        int passdoor_x = room->x;
        int passdoor_y = random_generator(room->y + 1, room->y + room->height - 2);
        if (floor->map[passdoor_y][passdoor_x] == '+') {
            retries++;
        } else {
            valid_position_found = true;
            password_door->x = passdoor_x;
            password_door->y = passdoor_y;
            generate_password(password_door);
            floor->map[passdoor_y][passdoor_x] = 'P';
            room->door_coords[2][0] = passdoor_x;
            room->door_coords[2][1] = passdoor_y;
        }
    }
    if (retries == max_retries) {
        return;
    }
}

void place_password_button(Floor *floor, Room *room) {
    int corner_choice = random_generator(0, 3);
    int button_x, button_y;
    switch (corner_choice) {
        case 0:
            button_x = room->x + 1; 
            button_y = room->y + 1;
            break;
        case 1:
            button_x = room->x + room->width - 2;  
            button_y = room->y + 1;  
            break;
        case 2:
            button_x = room->x + 1; 
            button_y = room->y + room->height - 2;  
        case 3:
            button_x = room->x + room->width - 2;
            button_y = room->y + room->height - 2;
            break;
    }
    floor->map[button_y][button_x] = '&';
}

void generate_password(PasswordDoor *door) {
   for (int i = 0; i < 4; i++) {
       door->password[i] = '0' + (rand() % 10);
   }
   door->password[4] = '\0';
   door->is_locked = 1;
   door->attempts_left = 3;
   door->password_time = time(NULL);
}

void display_password_for_30_seconds(Floor *floor, PasswordDoor *password) {
    int scr_row, scr_col;
    getmaxyx(stdscr, scr_row, scr_col);
    WINDOW *password_win = newwin(3, scr_col, 0, 0);
    box(password_win, 0, 0);
    colorspair();
    attron(COLOR_PAIR(5));
    mvwprintw(password_win, 1, 1, "Password: %s", password->password);
    attroff(COLOR_PAIR(5));
    wrefresh(password_win);
    usleep(1000000);
    clear();
    render_map(floor);
    refresh();
}

void show_message_for_2_seconds(const char *message) {
    int height = 3, width = strlen(message) + 4;
    int start_y =1; 
    int start_x =1;

    WINDOW *msg_win = newwin(height, width, start_y, start_x);
    box(msg_win, 0, 0);
    wattron(msg_win, COLOR_PAIR(6)); 
    mvwprintw(msg_win, 1, 2, message);
    wattroff(msg_win, COLOR_PAIR(6)); 
    wrefresh(msg_win);
    sleep(1);
    werase(msg_win);
    wrefresh(msg_win);
    delwin(msg_win);
    refresh();
}

void collect_food(Player *player, Floor *floor, int x, int y) {
    char food_type = floor->map[y][x];
    time_t current_time = time(NULL);
    
    bool can_collect = false;
    
    // Check for nightmare room illusions
    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
        for (int r = 2; r <= 4; r++) {
            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                x >= floor->rooms[r].x && x < floor->rooms[r].x + floor->rooms[r].width &&
                y >= floor->rooms[r].y && y < floor->rooms[r].y + floor->rooms[r].height) {
                show_message_for_2_seconds("It was just an illusion!");
                floor->map[y][x] = '.';
                return;
            }
        }
    }
    
    FoodItem new_food = {
        .collection_time = current_time
    };
    
    switch (food_type) {
        case 'B':  // Basic food
            if (player->basic_food_count < 5) {
                new_food.type = FOOD_BASIC;
                new_food.healthRestore = 10;
                player->basic_food[player->basic_food_count++] = new_food;
                show_message_for_2_seconds("Basic food collected! (+10 Health when eaten)");
                can_collect = true;
            } else {
                show_message_for_2_seconds("Basic food storage full!");
            }
            break;
            
        case 'A':  // Premium food
            if (player->premium_food_count < 5) {
                new_food.type = FOOD_PREMIUM;
                new_food.healthRestore = 15;
                player->premium_food[player->premium_food_count++] = new_food;
                show_message_for_2_seconds("Premium food collected! (+15 Health when eaten)");
                can_collect = true;
            } else {
                show_message_for_2_seconds("Premium food storage full!");
            }
            break;
            
        case 'M':  // Magical food
            if (player->magical_food_count < 5) {
                new_food.type = FOOD_MAGICAL;
                new_food.healthRestore = 25;
                player->magical_food[player->magical_food_count++] = new_food;
                show_message_for_2_seconds("Magical food collected! (+25 Health when eaten)");
                can_collect = true;
            } else {
                show_message_for_2_seconds("Magical food storage full!");
            }
            break;
            
        case 'G':  // Spoiled food
            if (player->spoiled_food_count < 5) {
                new_food.type = FOOD_SPOILED;
                new_food.healthRestore = -10;
                new_food.isSpoiled = true;
                player->spoiled_food[player->spoiled_food_count++] = new_food;
                show_message_for_2_seconds("Spoiled food collected! (-10 Health when eaten)");
                can_collect = true;
            } else {
                show_message_for_2_seconds("Spoiled food storage full!");
            }
            break;
    }
    
    if (can_collect) {
        floor->map[y][x] = '.';
    }
}

void show_some_message(const char *format, ...) {
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    int height = 3, width = strlen(message) + 4;
    int start_y = 1;
    int start_x = 1;

    WINDOW *msg_win = newwin(height, width, start_y, start_x);
    box(msg_win, 0, 0);
    mvwprintw(msg_win, 1, 2, "%s", message);
    wrefresh(msg_win);
    sleep(2);
    werase(msg_win);
    wrefresh(msg_win);
    delwin(msg_win);
    refresh();
}

void display_food_menu(Player *player) {
    int height = 12, width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *food_menu_win = newwin(height, width, start_y, start_x);
    box(food_menu_win, 0, 0);
    
    wattron(food_menu_win, COLOR_PAIR(6));
    mvwprintw(food_menu_win, 1, (width - 12) / 2, "Food Storage");
    wattroff(food_menu_win, COLOR_PAIR(6));
    
    mvwprintw(food_menu_win, 3, 2, "Health: %d/%d", 
        player->health.current_health, player->health.max_health);
    mvwprintw(food_menu_win, 4, 2, "Hunger: %d%%", player->health.hunger);
    
    mvwprintw(food_menu_win, 6, 2, "Basic Food   🍏: %d/5", player->basic_food_count);
    mvwprintw(food_menu_win, 7, 2, "Premium Food 🌭: %d/5", player->premium_food_count);
    mvwprintw(food_menu_win, 8, 2, "Magical Food 🍕: %d/5", player->magical_food_count);
    mvwprintw(food_menu_win, 9, 2, "Spoiled Food 🌶️: %d/5", player->spoiled_food_count);
    
    wattron(food_menu_win, COLOR_PAIR(3));
    mvwprintw(food_menu_win, height-2, 2, "Press 1-4 to eat, any other key to close");
    wattroff(food_menu_win, COLOR_PAIR(3));
    
    wrefresh(food_menu_win);
    
    int ch = wgetch(food_menu_win);
    bool ate_food = false;
    int health_change = 0;
    int hunger_change = 0;
    
    switch(ch) {
        case '1':  // Basic food
            if (player->basic_food_count > 0) {
                health_change = 10;
                hunger_change = 30;
                player->basic_food_count--;
                ate_food = true;
            }
            break;
        case '2':  // Premium food
            if (player->premium_food_count > 0) {
                health_change = 15;
                hunger_change = 45;
                player->premium_food_count--;
                ate_food = true;
            }
            break;
        case '3':  // Magical food
            if (player->magical_food_count > 0) {
                health_change = 25;
                hunger_change = 70;
                player->magical_food_count--;
                ate_food = true;
            }
            break;
        case '4':  // Spoiled food
            if (player->spoiled_food_count > 0) {
                health_change = -10;
                hunger_change = 10;
                player->spoiled_food_count--;
                ate_food = true;
            }
            break;
    }
    
    if (ate_food) {
        heal_health(&player->health, health_change);
        add_hunger(&player->health, hunger_change);
        show_some_message("Food consumed! Health %+d, Hunger %+d%%", 
            health_change, hunger_change);
    }
    
    werase(food_menu_win);
    wrefresh(food_menu_win);
    delwin(food_menu_win);
    refresh();
}

void place_spells(Floor *floor, Room *room, SpellItem *spell_list, int *spell_in_room) {
    int num_spells = random_generator(0, 3);
    
    for (int i = 0; i < num_spells; i++) {
        int spell_x = random_generator(room->x + 1, room->x + room->width - 2);
        int spell_y = random_generator(room->y + 1, room->y + room->height - 2);
        
        if (floor->map[spell_y][spell_x] == '.') {
            SpellType random_type = random_generator(0, 2);  // 0 to 2 for three spell types
            floor->spell_items[floor->spell_count] = (SpellItem){
                .type = random_type,
                .x = spell_x,
                .y = spell_y,
                .collected = false
            };
            switch (random_type) {
                case SPELL_HEALTH:
                    floor->map[spell_y][spell_x] = 'H';
                    break;
                case SPELL_SPEED:
                    floor->map[spell_y][spell_x] = 'V';
                    break;
                case SPELL_DAMAGE:
                    floor->map[spell_y][spell_x] = 'D';
                    break;
            }
            
            floor->spell_count++;
        } else {
            i--;
        }
    }
}

void collect_spell(Floor *floor, Player *player, SpellItem *spell_list, int *spell_count) {
    if (floor->floor_index >= 1 && floor->floor_index <= 2) {  // Floors 2-3
        for (int r = 2; r <= 4; r++) {
            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                player->x >= floor->rooms[r].x && player->x < floor->rooms[r].x + floor->rooms[r].width &&
                player->y >= floor->rooms[r].y && player->y < floor->rooms[r].y + floor->rooms[r].height) {
                show_message_for_2_seconds("It was just an illusion!");
                floor->map[player->y][player->x] = '.';
                return;
            }
        }
    }
    int i;
    for (i = 0; i < MAX_SPELLS_PLAYER; i++) {
        if (player->x == floor->spell_items[i].x && 
            player->y == floor->spell_items[i].y && 
            !floor->spell_items[i].collected) {
            floor->spell_items[i].collected = true;
            switch (floor->spell_items[i].type) {
                case SPELL_HEALTH:
                    player->health_spells++;
                    show_message_for_2_seconds("Health Spell Collected! (💉)");
                    break;
                case SPELL_SPEED:
                    player->speed_spells++;
                    show_message_for_2_seconds("Speed Spell Collected! (🔥)");
                    break;
                case SPELL_DAMAGE:
                    player->damage_spells++;
                    show_message_for_2_seconds("Damage Spell Collected! (💀)");
                    break;
            }
            
            player->collected_spells[player->spell_count] = floor->spell_items[i];
            player->spell_count++;
            
            floor->map[player->y][player->x] = '.';
            
            return;
        }
    }
}

void display_spell_menu(Player *player) {
    int height = 12, width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *spell_menu_win = newwin(height, width, start_y, start_x);
    box(spell_menu_win, 0, 0);
    
    wattron(spell_menu_win, COLOR_PAIR(6));
    mvwprintw(spell_menu_win, 1, (width - 16) / 2, "Collected Spells");
    wattroff(spell_menu_win, COLOR_PAIR(6));
    
    mvwprintw(spell_menu_win, 3, 2, "Health Spells (💉): %d %s", 
        player->health_spells,
        player->health.health_spell_active ? "[ACTIVE]" : "");
    mvwprintw(spell_menu_win, 4, 2, "Speed Spells  (🔥): %d %s", 
        player->speed_spells,
        player->health.speed_spell_active ? "[ACTIVE]" : "");
    mvwprintw(spell_menu_win, 5, 2, "Damage Spells (💀): %d %s",
        player->damage_spells,
        player->health.damage_spell_active ? "[ACTIVE]" : "");
    
    mvwprintw(spell_menu_win, 7, 2, "Total Spells: %d", player->spell_count);
    
    wattron(spell_menu_win, COLOR_PAIR(2));
    mvwprintw(spell_menu_win, height-3, 2, "Press 1-3 to activate spell");
    mvwprintw(spell_menu_win, height-2, 2, "Press any other key to close");
    wattroff(spell_menu_win, COLOR_PAIR(2));

    wrefresh(spell_menu_win);
    
    int ch = wgetch(spell_menu_win);
    switch(ch) {
        case '1':
            activate_health_spell(player);
            break;
        case '2':
            activate_speed_spell(player);
            break;
        case '3':
            activate_damage_spell(player);
            break;
    }
    
    werase(spell_menu_win);
    wrefresh(spell_menu_win);
    delwin(spell_menu_win);
    refresh();
}

void generate_all_floors(int scr_row, int scr_col) {
    for (int floor_index = 0; floor_index < 4; floor_index++) {
        initialize_floor(&floors[floor_index], scr_row, scr_col);
        floors[floor_index].floor_index = floor_index;
        srand(time(NULL) + floor_index);
        roommaker(&floors[floor_index]);
    }
}

void display_floor_name(int floor_index) {
    const char* floor_names[] = {
        "Floor 1: Welcome To Hell",
        "Floor 2: Iran",
        "Floor 3: Sharif Univerdity Of Technology", 
        "Floor 4: Computer Engineering Department"
    };

    int scr_row, scr_col;
    getmaxyx(stdscr, scr_row, scr_col);
    
    WINDOW *name_win = newwin(3, scr_col - 4, 1, 2);
    box(name_win, 0, 0);
    
    int name_length = strlen(floor_names[floor_index]);
    int start_x = (scr_col - name_length - 4) / 2;
    
    wattron(name_win, COLOR_PAIR(2));
    mvwprintw(name_win, 1, start_x, "%s", floor_names[floor_index]);
    wattroff(name_win, COLOR_PAIR(2));
    
    wrefresh(name_win);
    napms(2000);
    werase(name_win);
    wrefresh(name_win);
    delwin(name_win);
    refresh();
}

void place_weapons(Floor *floor, Room *room) {
    int num_weapons = random_generator(0, 2);
    
    for (int i = 0; i < num_weapons; i++) {
        int weapon_x = random_generator(room->x + 1, room->x + room->width - 2);
        int weapon_y = random_generator(room->y + 1, room->y + room->height - 2);
        
        if (floor->map[weapon_y][weapon_x] == '.') {
            WeaponType random_type = random_generator(0, 4);
            floor->weapon_items[floor->weapon_count] = (WeaponItem){
                .type = random_type,
                .x = weapon_x,
                .y = weapon_y,
                .collected = false
            };
            
            char weapon_char;
            switch (random_type) {
                 case MACE:
                    continue;
                case DAGGER:
                    weapon_char = 'K';
                    break;
                case MAGIC_WAND:
                    weapon_char = 'T'; 
                    break;
                case NORMAL_ARROW:
                    weapon_char = 'R';
                    break;
                case SWORD:
                    weapon_char = 'S';
                    break;
            }
            
            floor->map[weapon_y][weapon_x] = weapon_char;
            floor->weapon_count++;
        }
    }
}

void collect_weapon(Floor *floor, Player *player, int x, int y) {
    if (floor->floor_index >= 1 && floor->floor_index <= 2) {
        for (int r = 2; r <= 4; r++) {
            if (floor->rooms[r].theme == NIGHTMARE_ROOM &&
                x >= floor->rooms[r].x && x < floor->rooms[r].x + floor->rooms[r].width &&
                y >= floor->rooms[r].y && y < floor->rooms[r].y + floor->rooms[r].height) {
                show_message_for_2_seconds("It was just an illusion!");
                floor->map[y][x] = '.';
                return;
            }
        }
    }

    for (int i = 0; i < floor->weapon_count; i++) {
        if (floor->weapon_items[i].x == x && 
            floor->weapon_items[i].y == y && 
            !floor->weapon_items[i].collected) {
            
            floor->weapon_items[i].collected = true;
            
            if (floor->weapon_items[i].is_dropped) {
                switch (floor->weapon_items[i].type) {
                    case MACE:
                        break;
                    case DAGGER:
                        player->weapons.dagger_count++;
                        show_message_for_2_seconds("Dropped dagger collected! (🗡️) +1");
                        break;
                    case MAGIC_WAND:
                        player->weapons.wand_count++;
                        show_message_for_2_seconds("Dropped magic wand collected! (🔮) +1");
                        break;
                    case NORMAL_ARROW:
                        player->weapons.arrow_count++;
                        show_message_for_2_seconds("Dropped arrow collected! (🏹) +1");
                        break;
                    case SWORD:
                        break;
                }
            } else {
                switch (floor->weapon_items[i].type) {
                    case MACE:
                        break;
                    case DAGGER:
                        player->weapons.dagger_count += 10;
                        show_message_for_2_seconds("Dagger collected! (🗡️) +10");
                        break;
                    case MAGIC_WAND:
                        player->weapons.wand_count += 8;
                        show_message_for_2_seconds("Magic Wand collected! (🔮) +8");
                        break;
                    case NORMAL_ARROW:
                        player->weapons.arrow_count += 20;
                        show_message_for_2_seconds("Arrow collected! (🏹) +20");
                        break;
                    case SWORD:
                        if (!player->weapons.sword_count) {
                            player->weapons.sword_count = 1;
                            show_message_for_2_seconds("Sword collected! (🔪) [Permanent]");
                        } else {
                            show_message_for_2_seconds("You already have a sword!");
                        }
                        break;
                }
            }
            
            player->weapons.collected_weapons[player->weapons.weapon_count] = floor->weapon_items[i];
            player->weapons.weapon_count++;
            
            floor->map[y][x] = '.';
            break;
        }
    }
}

void display_weapon_menu(Player *player) {
    int height = 16, width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *weapon_menu_win = newwin(height, width, start_y, start_x);
    box(weapon_menu_win, 0, 0);
    
    // Title
    wattron(weapon_menu_win, COLOR_PAIR(6));
    mvwprintw(weapon_menu_win, 1, (width - 16) / 2, "Weapon Inventory");
    wattroff(weapon_menu_win, COLOR_PAIR(6));

    // Close Range Section
    wattron(weapon_menu_win, COLOR_PAIR(1));
    mvwprintw(weapon_menu_win, 3, 2, "=== Close Range Weapons ===");
    wattroff(weapon_menu_win, COLOR_PAIR(1));
    
    mvwprintw(weapon_menu_win, 4, 4, "Mace (m) 🎯:        %d %s", 
        player->weapons.mace_count,
        player->weapons.equipped_weapon == MACE ? "[EQUIPPED]" : "");
    
    mvwprintw(weapon_menu_win, 5, 4, "Sword (s) 🔪:       %s %s", 
        player->weapons.sword_count > 0 ? "Owned" : "None",
        player->weapons.equipped_weapon == SWORD ? "[EQUIPPED]" : "");

    // Long Range Section
    wattron(weapon_menu_win, COLOR_PAIR(1));
    mvwprintw(weapon_menu_win, 7, 2, "=== Long Range Weapons ===");
    wattroff(weapon_menu_win, COLOR_PAIR(1));
    
    mvwprintw(weapon_menu_win, 8, 4, "Dagger (d) 🗡️:      %d %s", 
        player->weapons.dagger_count,
        player->weapons.equipped_weapon == DAGGER ? "[EQUIPPED]" : "");
        
    mvwprintw(weapon_menu_win, 9, 4, "Magic Wand (w) 🔮:  %d %s", 
        player->weapons.wand_count,
        player->weapons.equipped_weapon == MAGIC_WAND ? "[EQUIPPED]" : "");
        
    mvwprintw(weapon_menu_win, 10, 4, "Arrow (a) 🏹:       %d %s", 
        player->weapons.arrow_count,
        player->weapons.equipped_weapon == NORMAL_ARROW ? "[EQUIPPED]" : "");

    // Stats Section
    wattron(weapon_menu_win, COLOR_PAIR(2));
    mvwprintw(weapon_menu_win, 12, 2, "Weapon Stats:");
    switch(player->weapons.equipped_weapon) {
        case MACE:
            mvwprintw(weapon_menu_win, 13, 4, "Damage: %d, Range: %d (Close range)", 
                     WEAPON_PROPERTIES[MACE].damage, WEAPON_PROPERTIES[MACE].range);
            break;
        case SWORD:
            mvwprintw(weapon_menu_win, 13, 4, "Damage: %d, Range: %d (Close range)", 
                     WEAPON_PROPERTIES[SWORD].damage, WEAPON_PROPERTIES[SWORD].range);
            break;
        case DAGGER:
            mvwprintw(weapon_menu_win, 13, 4, "Damage: %d, Range: %d (Throwable)", 
                     WEAPON_PROPERTIES[DAGGER].damage, WEAPON_PROPERTIES[DAGGER].range);
            break;
        case MAGIC_WAND:
            mvwprintw(weapon_menu_win, 13, 4, "Damage: %d, Range: %d (Magic)", 
                     WEAPON_PROPERTIES[MAGIC_WAND].damage, WEAPON_PROPERTIES[MAGIC_WAND].range);
            break;
        case NORMAL_ARROW:
            mvwprintw(weapon_menu_win, 13, 4, "Damage: %d, Range: %d (Ranged)", 
                     WEAPON_PROPERTIES[NORMAL_ARROW].damage, WEAPON_PROPERTIES[NORMAL_ARROW].range);
            break;
    }
    wattroff(weapon_menu_win, COLOR_PAIR(2));

    wattron(weapon_menu_win, COLOR_PAIR(3));
    mvwprintw(weapon_menu_win, height-2, 2, "Press weapon letter to equip, any other key to close");
    wattroff(weapon_menu_win, COLOR_PAIR(3));

    wrefresh(weapon_menu_win);
    
    int ch = wgetch(weapon_menu_win);
    switch(tolower(ch)) {
        case 'm':
            player->weapons.equipped_weapon = MACE;
            player->weapons.has_weapon_equipped = true;
            show_message_for_2_seconds("Mace equipped!");
            break;
        case 'd':
            if (player->weapons.dagger_count > 0) {
                player->weapons.equipped_weapon = DAGGER;
                player->weapons.has_weapon_equipped = true;
                show_message_for_2_seconds("Dagger equipped!");
            } else {
                show_message_for_2_seconds("No daggers available!");
            }
            break;
        case 'w':
            if (player->weapons.wand_count > 0) {
                player->weapons.equipped_weapon = MAGIC_WAND;
                player->weapons.has_weapon_equipped = true;
                show_message_for_2_seconds("Magic Wand equipped!");
            } else {
                show_message_for_2_seconds("No magic wands available!");
            }
            break;
        case 'a':
            if (player->weapons.arrow_count > 0) {
                player->weapons.equipped_weapon = NORMAL_ARROW;
                player->weapons.has_weapon_equipped = true;
                show_message_for_2_seconds("Normal Arrow equipped!");
            } else {
                show_message_for_2_seconds("No arrows available!");
            }
            break;
        case 's':
            if (player->weapons.sword_count > 0) {
                player->weapons.equipped_weapon = SWORD;
                player->weapons.has_weapon_equipped = true;
                show_message_for_2_seconds("Sword equipped!");
            } else {
                show_message_for_2_seconds("No sword available!");
            }
            break;
    }
    
    werase(weapon_menu_win);
    wrefresh(weapon_menu_win);
    delwin(weapon_menu_win);
    clear();
    render_map(&floors[current_floor]);
    mvaddch(player->y, player->x, '@');
    refresh();
}

void check_password_door(Floor *floor, Player *player, int new_x, int new_y) {
    if (floor->map[new_y][new_x] == 'P') {
        for (int i = 0; i < 6; i++) {
            PasswordDoor *door = &floor->password_doors[i];
            if (door->x == new_x && door->y == new_y && door->is_locked) {

                char input[5];
                int height = 3, width = 50;
                WINDOW *pass_win = newwin(height, width, 1, 1);
                box(pass_win, 0, 0);
                echo();

                switch(door->attempts_left) {
                    case 3: // First attempt
                        wattron(pass_win, COLOR_PAIR(6)); // Yellow
                        mvwprintw(pass_win, 1, 2, "Enter password: ");
                        wattroff(pass_win, COLOR_PAIR(6));
                        break;
                    case 2: // Second attempt
                        wattron(pass_win, COLOR_PAIR(8)); // Orange/Yellow
                        mvwprintw(pass_win, 1, 2, "Last chance! Enter password: ");
                        wattroff(pass_win, COLOR_PAIR(8));
                        break;
                    case 1: // Final attempt
                        wattron(pass_win, COLOR_PAIR(1)); // Red
                        mvwprintw(pass_win, 1, 2, "FINAL ATTEMPT! Enter password: ");
                        wattroff(pass_win, COLOR_PAIR(1));
                        break;
                }
                
                wrefresh(pass_win);
                wgetnstr(pass_win, input, 4);
                noecho();

                if (strcmp(input, door->password) == 0 || 
                    (current_floor >= 2 && check_reverse_password(input, door->password))) {
                    door->is_locked = 0;
                    floor->map[door->y][door->x] = '@';
                    attron(COLOR_PAIR(7));  // Green color
                    mvaddch(door->y, door->x, '@');
                    attroff(COLOR_PAIR(7));
                    show_message_for_2_seconds("Password correct! Door unlocked");
                } else {
                    door->attempts_left--;
                    
                    if (door->attempts_left == 0) {
                        door->is_locked = 1;
                        time(&door->password_time);
                        show_message_for_2_seconds("Too many wrong attempts! Door locked.");
                    } else {
                        show_message_for_2_seconds("Wrong password! Attempts left decreasing.");
                    }
                }

                werase(pass_win);
                wrefresh(pass_win);
                delwin(pass_win);
                return;
            }
        }
    }
}

bool check_reverse_password(const char *input, const char *password) {
    int len = strlen(password);
    char reversed[5];
    for (int i = 0; i < len; i++) {
        reversed[i] = password[len - 1 - i];
    }
    reversed[len] = '\0';
    return strcmp(input, reversed) == 0;
}

void place_ancient_key(Floor *floor, Room *room) {
    int key_x = random_generator(room->x + 1, room->x + room->width - 2);
    int key_y = random_generator(room->y + 1, room->y + room->height - 2);
    
    if (floor->map[key_y][key_x] == '.') {
        floor->ancient_key.x = key_x;
        floor->ancient_key.y = key_y;
        floor->ancient_key.collected = false;
        floor->ancient_key.is_broken = false;
        floor->map[key_y][key_x] = '%';  // Using % as the symbol for ancient key
    }
}

void collect_ancient_key(Player *player, Floor *floor, int x, int y) {
    if (floor->ancient_key.x == x && floor->ancient_key.y == y && !floor->ancient_key.collected) {
        floor->ancient_key.collected = true;
        player->ancient_keys++;
        floor->map[y][x] = '.';
        show_message_for_2_seconds("Ancient Key collected!");
    }
}

bool use_ancient_key(Floor *floor, Player *player, int door_x, int door_y) {
    if (player->ancient_keys > 0) {
        // 10% chance of breaking
        if (random_generator(1, 100) <= 10) {
            show_message_for_2_seconds("Ancient Key broke while opening the door!");
            player->ancient_keys--;
            player->broken_keys++;
            return false;
        }
        
        for (int i = 0; i < 6; i++) {
            PasswordDoor *door = &floor->password_doors[i];
            if (door->x == door_x && door->y == door_y) {
                door->is_locked = 0;
                floor->map[door->y][door->x] = '@';
                show_message_for_2_seconds("Door opened with Ancient Key!");
                player->ancient_keys--;
                return true;
            }
        }
    }
    return false;
}

void end_game(Player *player) {    
    int height = 10, width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *end_win = newwin(height, width, start_y, start_x);
    box(end_win, 0, 0);
    
    wattron(end_win, COLOR_PAIR(6));
    mvwprintw(end_win, 2, (width - 24) / 2, "🎉 CONGRATULATIONS! 🎉");
    mvwprintw(end_win, 4, 2, "You found the treasure room!");
    mvwprintw(end_win, 5, 2, "Final Score: %d", player->gold);
    mvwprintw(end_win, 7, 2, "Press any key to exit...");
    wattroff(end_win, COLOR_PAIR(6));
    
    wrefresh(end_win);
    wgetch(end_win);
    delwin(end_win);
    endwin();
    exit(0);
}

void place_monsters(Floor *floor, Room *room) {
    int num_monsters = random_generator(1, 3);  // 1-3 monsters per room
    
    for (int i = 0; i < num_monsters; i++) {
        
        int monster_x, monster_y;
        do {
            monster_x = random_generator(room->x + 1, room->x + room->width - 2);
            monster_y = random_generator(room->y + 1, room->y + room->height - 2);
        } while (floor->map[monster_y][monster_x] != '.');
        
        
        MonsterType type;
        if (room->theme == NIGHTMARE_ROOM) {
            
            type = random_generator(GIANT, UNDEAD);
        } else if (floor->floor_index >= 2) {
            
            type = random_generator(FIRE, UNDEAD);
        } else {
            
            type = random_generator(DEMON, GIANT);
        }
        
        Monster new_monster = create_monster(type, monster_x, monster_y);
        floor_monsters[floor->floor_index].monsters[floor_monsters[floor->floor_index].monster_count++] = new_monster;
        char monster_char;
        switch(type) {
            case DEMON: monster_char = 'd'; break;
            case FIRE: monster_char = 'F'; break;
            case GIANT: monster_char = 'X'; break;
            case SNAKE: monster_char = 'x'; break;
            case UNDEAD: monster_char = 'U'; break;
        }
        floor->map[monster_y][monster_x] = monster_char;
    }
}

bool is_valid_monster_move(Floor *floor, int x, int y, Player *player) {
    if (x == player->x && y == player->y) {
        return false;
    }
    return floor->map[y][x] == '.' || floor->map[y][x] == '#';
}

void get_direction_to_player(int monster_x, int monster_y, int player_x, int player_y, int *dx, int *dy) {
    *dx = (monster_x < player_x) ? 1 : (monster_x > player_x) ? -1 : 0;
    *dy = (monster_y < player_y) ? 1 : (monster_y > player_y) ? -1 : 0;
}

bool is_monster_in_visited_room(Floor *floor, Monster *monster) {
    for (int i = 0; i < 6; i++) {
        Room *room = &floor->rooms[i];
        if (monster->x >= room->x && monster->x < room->x + room->width &&
            monster->y >= room->y && monster->y < room->y + room->height) {
            return room->visited;
        }
    }
    return false;
}

bool is_within_range(Monster *monster, int new_x, int new_y) {
    if (monster->type == SNAKE) return true; 
    
    int distance_from_start = abs(new_x - monster->initial_x) + abs(new_y - monster->initial_y);
    return distance_from_start <= 5;
}

void update_monsters(Floor *floor, Player *player) {
    MonsterList *current_monsters = &floor_monsters[floor->floor_index];
    
    for (int i = 0; i < current_monsters->monster_count; i++) {
        Monster *monster = &current_monsters->monsters[i];
        if (!monster->is_alive) continue;
        if (!is_monster_in_visited_room(floor, monster)) continue;
        if (monster->is_paralyzed) continue; 

        int old_x = monster->x;
        int old_y = monster->y;
        
        bool should_follow = false;
        int distance = abs(monster->x - player->x) + abs(monster->y - player->y);
        
        if (distance <= 5) {
            switch (monster->type) {
                case SNAKE:
                    should_follow = true;
                    break;
                case GIANT:
                case UNDEAD:
                    if (distance <= 1) { 
                        should_follow = true;
                    }
                    break;
                case DEMON:
                case FIRE:
                    should_follow = (distance <= 3);
                    break;
            }
        }

        if (should_follow) {
            int dx, dy;
            get_direction_to_player(monster->x, monster->y, player->x, player->y, &dx, &dy);
            int new_x = monster->x + dx;
            int new_y = monster->y + dy;
            
            if (is_within_range(monster, new_x, new_y) && 
                is_valid_monster_move(floor, new_x, new_y, player)) {
                monster->x = new_x;
                monster->y = new_y;
            } else if (dx != 0 && 
                      is_within_range(monster, monster->x + dx, monster->y) && 
                      is_valid_monster_move(floor, monster->x + dx, monster->y, player)) {
                monster->x += dx;
            } else if (dy != 0 && 
                      is_within_range(monster, monster->x, monster->y + dy) && 
                      is_valid_monster_move(floor, monster->x, monster->y + dy, player)) {
                monster->y += dy;
            }
        } else {
            if (random_generator(0, 3) == 0) { 
                int dx = random_generator(-1, 1);
                int dy = random_generator(-1, 1);
                int new_x = monster->x + dx;
                int new_y = monster->y + dy;
                
                if (is_within_range(monster, new_x, new_y) && 
                    is_valid_monster_move(floor, new_x, new_y, player)) {
                    monster->x = new_x;
                    monster->y = new_y;
                }
            }
        }
        
        if (old_x != monster->x || old_y != monster->y) {
            floor->map[old_y][old_x] = '.';
            floor->map[monster->y][monster->x] = get_monster_char(monster->type);
        }
    }
}

char get_monster_char(MonsterType type) {
    switch(type) {
        case DEMON: return 'd';
        case FIRE: return 'F';
        case GIANT: return 'X';
        case SNAKE: return 'x';
        case UNDEAD: return 'U';
        default: return '?';
    }
}

void handle_mace_attack(Player *player, Floor *floor) {
    attron(COLOR_PAIR(1));
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            
            int attack_x = player->x + dx;
            int attack_y = player->y + dy;
            
            if (attack_x >= 0 && attack_x < MAP_COLS && 
                attack_y >= 0 && attack_y < MAP_ROWS) {
                mvaddch(attack_y, attack_x, '*');
            }
        }
    }
    refresh();
    napms(100);
    attroff(COLOR_PAIR(1));
    
    bool hit_something = false;
    for (int i = 0; i < floor_monsters[current_floor].monster_count; i++) {
        Monster *monster = &floor_monsters[current_floor].monsters[i];
        if (!monster->is_alive) continue;

        int dx = abs(monster->x - player->x);
        int dy = abs(monster->y - player->y);
        if (dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0)) {
            int damage = get_weapon_damage(MACE, player);
            monster->health -= damage;
            hit_something = true;
            
            if (monster->health <= 0) {
                monster->is_alive = false;
                floor->map[monster->y][monster->x] = '.';
                show_message_for_2_seconds("Monster defeated!");
            } else {
                char msg[50];
                snprintf(msg, sizeof(msg), "Hit! Monster health: %d", monster->health);
                show_message_for_2_seconds(msg);
            }
        }
    }
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
    
    if (!hit_something) {
        show_message_for_2_seconds("Attack missed!");
    }
}

bool is_monster_at_position(Floor *floor, int x, int y) {
    char tile = floor->map[y][x];
    return (tile == 'd' || tile == 'F' || tile == 'X' || 
            tile == 'x' || tile == 'U');
}

bool can_move_to_position(Floor *floor, int x, int y) {
    if (is_monster_at_position(floor, x, y)) {
        return false;
    }
    char tile = floor->map[y][x];
    return (tile == '.' || tile == '#' || tile == '+' || 
            tile == '^' || tile == '&' || tile == 'G' || 
            tile == '$' || tile == 'g' || tile == 'B' || 
            tile == 'M' || tile == 'A' || tile == 'H' || 
            tile == '<' || tile == '>' || tile == 'W' || 
            tile == 'K' || tile == 'T' || tile == 'R' || 
            tile == 'S' || tile == 'Q' || tile == '@' || tile == '=' );
}

void get_throw_direction(int key, int *dx, int *dy) {
    *dx = 0;
    *dy = 0;
    switch(key) {
        case 'j': case 'J': *dy = -1; break;  // Up
        case 'k': case 'K': *dy = 1;  break;  // Down
        case 'h': case 'H': *dx = -1; break;  // Left
        case 'l': case 'L': *dx = 1;  break;  // Right
        case 'y': case 'Y': *dy = -1; *dx = -1; break;  // Up-left
        case 'u': case 'U': *dy = -1; *dx = 1;  break;  // Up-right
        case 'b': case 'B': *dy = 1;  *dx = -1; break;  // Down-left
        case 'n': case 'N': *dy = 1;  *dx = 1;  break;  // Down-right
    }
}

void handle_dagger_attack(Player *player, Floor *floor) {
    show_message_for_2_seconds("Choose throw direction (hjklyubn)");
    
    int key = getch();
    int dx, dy;
    get_throw_direction(key, &dx, &dy);
    if (dx == 0 && dy == 0) {
        show_message_for_2_seconds("Invalid direction!");
        return;
    }
    if (player->weapons.dagger_count <= 0) {
        show_message_for_2_seconds("No daggers left!");
        return;
    }
    player->weapons.dagger_count--;
    
    int throw_x = player->x;
    int throw_y = player->y;
    bool hit_something = false;
    
    for (int dist = 1; dist <= 5; dist++) {
        throw_x += dx;
        throw_y += dy;
        if (throw_x < 0 || throw_x >= MAP_COLS || throw_y < 0 || throw_y >= MAP_ROWS) {
            break;
        }
        
        if (floor->map[throw_y][throw_x] == '*' || 
            floor->map[throw_y][throw_x] == '|' || 
            floor->map[throw_y][throw_x] == '-' || 
            floor->map[throw_y][throw_x] == '+') {
            break;
        }
        
        mvprintw(throw_y, throw_x, "🗡️");
        refresh();
        napms(50);
        
        for (int i = 0; i < floor_monsters[current_floor].monster_count; i++) {
            Monster *monster = &floor_monsters[current_floor].monsters[i];
            if (monster->is_alive && monster->x == throw_x && monster->y == throw_y) {
                int damage = get_weapon_damage(DAGGER, player);
                monster->health -= damage;
                hit_something = true;
                
                if (monster->health <= 0) {
                    monster->is_alive = false;
                    floor->map[monster->y][monster->x] = '.';
                    show_message_for_2_seconds("Monster defeated by dagger!");
                } else {
                    char msg[50];
                    snprintf(msg, sizeof(msg), "Dagger hit! Monster health: %d", monster->health);
                    show_message_for_2_seconds(msg);
                }
                goto end_throw;
            }
        }
    }
    
end_throw:
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
    
    if (!hit_something) {
        show_message_for_2_seconds("Dagger missed!");
    }
}

void handle_magic_wand_attack(Player *player, Floor *floor) {
    show_message_for_2_seconds("Choose throw direction (hjklyubn)");
    
    int key = getch();
    int dx, dy;
    get_throw_direction(key, &dx, &dy);
    
    if (dx == 0 && dy == 0) {
        show_message_for_2_seconds("Invalid direction!");
        return;
    }
    
    if (player->weapons.wand_count <= 0) {
        show_message_for_2_seconds("No magic wands left!");
        return;
    }
    
    player->weapons.wand_count--;
    
    int throw_x = player->x;
    int throw_y = player->y;
    bool hit_something = false;
    
    for (int dist = 1; dist <= 10; dist++) {
        throw_x += dx;
        throw_y += dy;
        
        if (throw_x < 0 || throw_x >= MAP_COLS || throw_y < 0 || throw_y >= MAP_ROWS) {
            if (dist > 1) {
                drop_weapon(floor, MAGIC_WAND, throw_x - dx, throw_y - dy);
                show_message_for_2_seconds("Magic wand hit wall and dropped!");
            }
            break;
        }
        
        if (floor->map[throw_y][throw_x] == '*' || 
            floor->map[throw_y][throw_x] == '|' || 
            floor->map[throw_y][throw_x] == '-' || 
            floor->map[throw_y][throw_x] == '+') {
            drop_weapon(floor, MAGIC_WAND, throw_x - dx, throw_y - dy);
            show_message_for_2_seconds("Magic wand hit wall and dropped!");
            break;
        }
        
        mvprintw(throw_y, throw_x, "🔮");
        refresh();
        napms(50);
        
         for (int i = 0; i < floor_monsters[current_floor].monster_count; i++) {
            Monster *monster = &floor_monsters[current_floor].monsters[i];
            if (monster->is_alive && monster->x == throw_x && monster->y == throw_y) {
                int damage = get_weapon_damage(MAGIC_WAND, player);
                monster->health -= damage;
                monster->is_paralyzed = true;
                hit_something = true;
                
                if (monster->health <= 0) {
                    monster->is_alive = false;
                    floor->map[monster->y][monster->x] = '.';
                    show_message_for_2_seconds("Monster defeated by magic wand!");
                } else {
                    char msg[50];
                    snprintf(msg, sizeof(msg), "Magic hit! Monster paralyzed! Health: %d", monster->health);
                    show_message_for_2_seconds(msg);
                }
                goto end_throw;
            }
        }
        
        if (dist == 10) {
            drop_weapon(floor, MAGIC_WAND, throw_x, throw_y);
            show_message_for_2_seconds("Magic wand fell to the ground!");
        }
    }
    
end_throw:
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
}

void handle_arrow_attack(Player *player, Floor *floor) {
    show_message_for_2_seconds("Choose throw direction (hjklyubn)");
    
    int key = getch();
    int dx, dy;
    get_throw_direction(key, &dx, &dy);
    
    if (dx == 0 && dy == 0) {
        show_message_for_2_seconds("Invalid direction!");
        return;
    }
    
    if (player->weapons.arrow_count <= 0) {
        show_message_for_2_seconds("No arrows left!");
        return;
    }
    
    player->weapons.arrow_count--;
    
    int throw_x = player->x;
    int throw_y = player->y;
    bool hit_something = false;
    
    for (int dist = 1; dist <= 5; dist++) {
        throw_x += dx;
        throw_y += dy;
        
        if (throw_x < 0 || throw_x >= MAP_COLS || throw_y < 0 || throw_y >= MAP_ROWS) {
            if (dist > 1) {
                drop_weapon(floor, NORMAL_ARROW, throw_x - dx, throw_y - dy);
                show_message_for_2_seconds("Arrow hit wall and dropped!");
            }
            break;
        }
        
        if (floor->map[throw_y][throw_x] == '*' || 
            floor->map[throw_y][throw_x] == '|' || 
            floor->map[throw_y][throw_x] == '-' || 
            floor->map[throw_y][throw_x] == '+') {
            drop_weapon(floor, NORMAL_ARROW, throw_x - dx, throw_y - dy);
            show_message_for_2_seconds("Arrow hit wall and dropped!");
            break;
        }
        
        mvprintw(throw_y, throw_x, "🏹");
        refresh();
        napms(50);
        
         for (int i = 0; i < floor_monsters[current_floor].monster_count; i++) {
            Monster *monster = &floor_monsters[current_floor].monsters[i];
            if (monster->is_alive && monster->x == throw_x && monster->y == throw_y) {
                int damage = get_weapon_damage(NORMAL_ARROW, player);
                monster->health -= damage;
                hit_something = true;
                
                if (monster->health <= 0) {
                    monster->is_alive = false;
                    floor->map[monster->y][monster->x] = '.';
                    show_message_for_2_seconds("Monster defeated by arrow!");
                } else {
                    char msg[50];
                    snprintf(msg, sizeof(msg), "Arrow hit! Monster health: %d", monster->health);
                    show_message_for_2_seconds(msg);
                }
                goto end_throw;
            }
        }
        
        if (dist == 5) {
            drop_weapon(floor, NORMAL_ARROW, throw_x, throw_y);
            show_message_for_2_seconds("Arrow fell to the ground!");
        }
    }
    
end_throw:
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
}

void handle_sword_attack(Player *player, Floor *floor) {
    bool hit_something = false;
    
    int dx[] = {-1, -1, -1,  0,  0,  1, 1, 1};
    int dy[] = {-1,  0,  1, -1,  1, -1, 0, 1};
    
    attron(COLOR_PAIR(1));
    for (int i = 0; i < 8; i++) {
        int attack_x = player->x + dx[i];
        int attack_y = player->y + dy[i];
        
        if (attack_x >= 0 && attack_x < MAP_COLS && 
            attack_y >= 0 && attack_y < MAP_ROWS) {
            mvaddch(attack_y, attack_x, '*');
        }
    }
    refresh();
    napms(100);
    attroff(COLOR_PAIR(1));
    
    for (int i = 0; i < 8; i++) {
        int attack_x = player->x + dx[i];
        int attack_y = player->y + dy[i];
        
        if (attack_x < 0 || attack_x >= MAP_COLS || 
            attack_y < 0 || attack_y >= MAP_ROWS) {
            continue;
        }
        
        for (int j = 0; j < floor_monsters[current_floor].monster_count; j++) {
            Monster *monster = &floor_monsters[current_floor].monsters[j];
            if (monster->is_alive && 
                monster->x == attack_x && 
                monster->y == attack_y) {
                int damage = get_weapon_damage(SWORD, player);
                monster->health -= damage;
                hit_something = true;
                
                if (monster->health <= 0) {
                    monster->is_alive = false;
                    floor->map[monster->y][monster->x] = '.';
                    show_message_for_2_seconds("Monster defeated by sword!");
                } else {
                    char msg[50];
                    snprintf(msg, sizeof(msg), "Sword hit! Monster health: %d", monster->health);
                    show_message_for_2_seconds(msg);
                }
            }
        }
    }
    
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
    
    if (!hit_something) {
        show_message_for_2_seconds("Sword attack missed!");
    }
}

void drop_weapon(Floor *floor, WeaponType type, int x, int y) {
    floor->weapon_items[floor->weapon_count] = (WeaponItem){
        .type = type,
        .x = x,
        .y = y,
        .collected = false,
        .is_dropped = true
    };
    char weapon_char;
    switch (type) {
        case DAGGER: weapon_char = 'K'; break;
        case MAGIC_WAND: weapon_char = 'T'; break;
        case NORMAL_ARROW: weapon_char = 'R'; break;
        default: return;
    }
    
    floor->map[y][x] = weapon_char;
    floor->weapon_count++;
}

void update_health_system(HealthSystem *health) {
    time_t current_time = time(NULL);
    if (health->health_spell_active && 
        current_time - health->health_spell_start_time >= 10) {
        health->health_spell_active = false;
        show_message_for_2_seconds("Health spell has worn off!");
    }
    if (health->speed_spell_active && 
        current_time - health->speed_spell_start_time >= 10) {
        health->speed_spell_active = false;
        show_message_for_2_seconds("Speed spell has worn off!");
    }
    if (health->damage_spell_active && 
        current_time - health->damage_spell_start_time >= 10) {
        health->damage_spell_active = false;
        show_message_for_2_seconds("Damage spell has worn off!");
    }

    if (current_time - health->last_hunger_update >= HUNGER_CHECK_INTERVAL) {
        health->hunger = MAX(0, health->hunger - HUNGER_DECREASE_RATE);
        health->last_hunger_update = current_time;
    }
    
    if (health->hunger > 70 && 
        current_time - health->last_damage_time >= HEALTH_REGEN_INTERVAL &&
        current_time - health->last_health_regen >= HEALTH_REGEN_INTERVAL &&
        health->current_health < health->max_health) {
        
        int regen_amount = HEALTH_REGEN_RATE;
        if (health->health_spell_active) {
            regen_amount *= 2;
        }
        
        health->current_health = MIN(health->current_health + regen_amount, health->max_health);
        health->last_health_regen = current_time;
        health->is_regenerating = true;
    } else {
        health->is_regenerating = false;
    }
}

void take_damage(HealthSystem *health, int damage) {
    health->current_health = MAX(0, health->current_health - damage);
    health->last_damage_time = time(NULL);
    health->is_regenerating = false;
}

void heal_health(HealthSystem *health, int amount) {
    health->current_health = MIN(health->max_health, health->current_health + amount);
}

void add_hunger(HealthSystem *health, int amount) {
    health->hunger = MIN(MAX_HUNGER, health->hunger + amount);
}

void check_food_spoilage(Player *player) {
    time_t current_time = time(NULL);
    
    // 30 seconds
    if (current_time - player->last_food_check < FOOD_SPOIL_CHECK_INTERVAL) {
        return;
    }
    
    player->last_food_check = current_time;
    
    // Check magical food -> basic food
    for (int i = 0; i < player->magical_food_count; i++) {
        if (current_time - player->magical_food[i].collection_time >= 120) { // 2 minutes
            if (player->basic_food_count < 5) {
                player->basic_food[player->basic_food_count++] = (FoodItem){
                    .type = FOOD_PREMIUM,
                    .healthRestore = 15,
                    .collection_time = current_time
                };
            }
            for (int j = i; j < player->magical_food_count - 1; j++) {
                player->magical_food[j] = player->magical_food[j + 1];
            }
            player->magical_food_count--;
            i--; 
            show_message_for_2_seconds("A magical food has degraded to premium!");
        }
    }
    
    // Check premium food -> basic food
    for (int i = 0; i < player->premium_food_count; i++) {
        if (current_time - player->premium_food[i].collection_time >= 180) { // 3 minutes
            if (player->basic_food_count < 5) {
                player->basic_food[player->basic_food_count++] = (FoodItem){
                    .type = FOOD_BASIC,
                    .healthRestore = 10,
                    .collection_time = current_time
                };
            }
            for (int j = i; j < player->premium_food_count - 1; j++) {
                player->premium_food[j] = player->premium_food[j + 1];
            }
            player->premium_food_count--;
            i--;
            show_message_for_2_seconds("A premium food has degraded to basic!");
        }
    }
    
    // Check basic food -> spoiled food
    for (int i = 0; i < player->basic_food_count; i++) {
        if (current_time - player->basic_food[i].collection_time >= 240) { // 4 minutes
            if (player->spoiled_food_count < 5) {
                player->spoiled_food[player->spoiled_food_count++] = (FoodItem){
                    .type = FOOD_SPOILED,
                    .healthRestore = -10,
                    .isSpoiled = true,
                    .collection_time = current_time
                };
            }
            for (int j = i; j < player->basic_food_count - 1; j++) {
                player->basic_food[j] = player->basic_food[j + 1];
            }
            player->basic_food_count--;
            i--;
            show_message_for_2_seconds("A basic food has spoiled!");
        }
    }
}

void activate_health_spell(Player *player) {
    if (player->health_spells <= 0) {
        show_message_for_2_seconds("No health spells available!");
        return;
    }
    
    if (player->health.health_spell_active) {
        show_message_for_2_seconds("Health spell already active!");
        return;
    }
    
    player->health_spells--;
    player->health.health_spell_active = true;
    player->health.health_spell_start_time = time(NULL);
    show_message_for_2_seconds("Health spell activated! (💉) Regeneration doubled!");
}

void init_health_system(HealthSystem *health) {
    int max_health;
    switch(difficulty) {
        case 1: // Easy
            max_health = HEALTH_EASY;
            break;
        case 2: // Medium
            max_health = HEALTH_MEDIUM;
            break;
        case 3: // Hard
            max_health = HEALTH_HARD;
            break;
        default:
            max_health = HEALTH_MEDIUM;
    }
    
    health->current_health = max_health;
    health->max_health = max_health;
    health->hunger = MAX_HUNGER;
    health->last_damage_time = time(NULL);
    health->last_health_regen = time(NULL);
    health->last_hunger_update = time(NULL);
    health->is_regenerating = false;
    health->health_spell_active = false;
    health->health_spell_start_time = 0;
    health->speed_spell_active = false;
    health->speed_spell_start_time = 0;
    health->damage_spell_active = false;
    health->damage_spell_start_time = 0;
    char msg[100];
    snprintf(msg, sizeof(msg), "Game initialized: Max Health = %d", max_health);
    show_message_for_2_seconds(msg);
}

void activate_speed_spell(Player *player) {
    if (player->speed_spells <= 0) {
        show_message_for_2_seconds("No speed spells available!");
        return;
    }
    
    if (player->health.speed_spell_active) {
        show_message_for_2_seconds("Speed spell already active!");
        return;
    }
    
    player->speed_spells--;
    player->health.speed_spell_active = true;
    player->health.speed_spell_start_time = time(NULL);
    show_message_for_2_seconds("Speed spell activated! (🔥) Movement speed doubled!");
}

void move_player_speed(int ch, Player *player, Floor *floor, int scr_row, int scr_col) {
    int dx = 0, dy = 0;
    
    if (ch == 'J' || ch == 'j') dy = -2;               // Up
    else if (ch == 'K' || ch == 'k') dy = 2;           // Down
    else if (ch == 'H' || ch == 'h') dx = -2;          // Left
    else if (ch == 'L' || ch == 'l') dx = 2;           // Right
    else if (ch == 'Y' || ch == 'y') { dx = -2; dy = -2; } // Up-left
    else if (ch == 'U' || ch == 'u') { dx = 2; dy = -2; }  // Up-right
    else if (ch == 'B' || ch == 'b') { dx = -2; dy = 2; }  // Down-left
    else if (ch == 'N' || ch == 'n') { dx = 2; dy = 2; }   // Down-right
    
    int new_x = player->x + dx;
    int new_y = player->y + dy;
    
    if (new_x < 1 || new_x >= MAP_COLS - 1 || new_y < 1 || new_y >= MAP_ROWS - 2) {
        return;
    }
    
    if (floor->map[player->y][player->x] != '#' && 
        floor->map[player->y][player->x] != '+' &&
        floor->map[player->y][player->x] != '@') {
        mvaddch(player->y, player->x, '.');
    } else {
        mvaddch(player->y, player->x, floor->map[player->y][player->x]);
    }
    
    if (can_move_to_position(floor, new_x, new_y)) {
        if (floor->map[new_y][new_x] == '$') {
            player->gold++;
            floor->map[new_y][new_x] = '.';
            show_message_for_2_seconds("Gold collected! Gold +1");
        }
        if (floor->map[new_y][new_x] == 'g') {
            player->gold += 5;
            floor->map[new_y][new_x] = '.';
            show_message_for_2_seconds("Black Gold collected! Gold +5");
        }
        
        player->x = new_x;
        player->y = new_y;
        mvaddch(player->y, player->x, '@');
    }
}

void activate_damage_spell(Player *player) {
    if (player->damage_spells <= 0) {
        show_message_for_2_seconds("No damage spells available!");
        return;
    }
    
    if (player->health.damage_spell_active) {
        show_message_for_2_seconds("Damage spell already active!");
        return;
    }
    
    player->damage_spells--;
    player->health.damage_spell_active = true;
    player->health.damage_spell_start_time = time(NULL);
    show_message_for_2_seconds("Damage spell activated! (💀) Weapon damage doubled!");
}

int  get_weapon_damage(WeaponType type, Player *player) {
    int base_damage = WEAPON_PROPERTIES[type].damage;
    return player->health.damage_spell_active ? base_damage * 2 : base_damage;
}

void place_question_mark(Floor *floor, Room *room) { // for treasure
    if (floor->floor_index != 3) return;
    
    int attempts = 0;
    int max_attempts = 50;
    while (attempts < max_attempts) {
        int x = random_generator(room->x + 1, room->x + room->width - 2);
        int y = random_generator(room->y + 1, room->y + room->height - 2);
        if (floor->map[y][x] == '.') {
            floor->map[y][x] = '?';
            break;
        }
        attempts++;
    }
}

void create_treasure_room(Floor *floor, Player *player) {
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            floor->map[i][j] = ' ';
            if (i == 0 || i == MAP_ROWS - 2 || j == 0 || j == MAP_COLS - 1) {
                floor->map[i][j] = '*';
            }
        }
    }

    const char* header[] = {
        "    |>>>                                                      |>>>",
        "    |                     |>>>          |>>>                  |",
        "    *                     |             |                     *",
        "   / \\                    *             *                    / \\",
        "  /___\\                 _/ \\           / \\_                 /___\\",
        "  [   ]                |/   \\_________/   \\|                [   ]",
        "  [ I ]                /     \\       /     \\                [ I ]",
        "  [   ]_ _ _          /       \\     /       \\          _ _ _[   ]",
        "  [   ] U U |        {#########}   {#########}        | U U [   ]",
        "  [   ]====/          \\=======/     \\=======/          \\====[   ]",
        "  [___]    |           |   I |_ _ _ _| I   |           |    [___]",
        "   \\=/     |===========|     |=======|     |===========|     \\=/"
    };
    
    for (int i = 0; i < 12 && i + 2 < MAP_ROWS; i++) {
        int len = strlen(header[i]);
        int start_header_x = (MAP_COLS - len) / 2;
        for (int j = 0; j < len && start_header_x + j < MAP_COLS - 1; j++) {
            if (start_header_x + j > 0) {
                floor->map[i + 2][start_header_x + j] = header[i][j];
            }
        }
    }

    int room_width = (MAP_COLS * 2) / 3;
    int room_height = (MAP_ROWS * 1) / 2; 
    int start_x = (MAP_COLS - room_width) / 2;
    int start_y = MAP_ROWS - room_height - 20;

    Room treasure_room = {
        .x = start_x,
        .y = start_y,
        .width = room_width,
        .height = room_height,
        .theme = TREASURE_ROOM,
        .visited = true
    };
    
    memset(&floor->rooms[0], 0, sizeof(Room)); 
    floor->rooms[0] = treasure_room;

    for (int y = start_y; y < start_y + room_height && y < MAP_ROWS - 1; y++) {
        for (int x = start_x; x < start_x + room_width && x < MAP_COLS - 1; x++) {
            if (y == start_y || y == start_y + room_height - 1) {
                floor->map[y][x] = '-';
            } else if (x == start_x || x == start_x + room_width - 1) {
                floor->map[y][x] = '|';
            } else {
                floor->map[y][x] = '.';
            }
        }
    }

    // entrance
    int entrance_x = start_x + (room_width / 2);
    int entrance_y = start_y + room_height - 1;
    if (entrance_y < MAP_ROWS - 1) {
        floor->map[entrance_y][entrance_x] = '+';
    }
    player->x = entrance_x;
    player->y = entrance_y;

    floor_monsters[floor->floor_index].monster_count = 0;

    int max_monsters = MIN(15, (room_width * room_height) / 20);
    int num_monsters = random_generator(10, max_monsters);
    
    for (int i = 0; i < num_monsters && 
         floor_monsters[floor->floor_index].monster_count < MAX_MONSTERS_PER_FLOOR; i++) {
        int x = random_generator(start_x + 1, start_x + room_width - 2);
        int y = random_generator(start_y + 1, start_y + room_height - 2);
        
        if (floor->map[y][x] == '.' &&
            (abs(x - entrance_x) > 2 || abs(y - entrance_y) > 2)) {
            MonsterType type = random_generator(DEMON, UNDEAD);
            Monster new_monster = create_monster(type, x, y);
            floor_monsters[floor->floor_index].monsters[floor_monsters[floor->floor_index].monster_count++] = new_monster;
            floor->map[y][x] = get_monster_char(type);
        }
    }

    int num_items = MIN(40, (room_width * room_height) / 15);
    for (int i = 0; i < num_items; i++) {
        int x = random_generator(start_x + 1, start_x + room_width - 2);
        int y = random_generator(start_y + 1, start_y + room_height - 2);
        if (floor->map[y][x] == '.') {
            int item_type = random_generator(1, 10);
            if (item_type <= 4) {
                floor->map[y][x] = '$';  // 40% regular gold
            } else if (item_type <= 6) {
                floor->map[y][x] = 'g';  // 20% black gold
            } else {
                floor->map[y][x] = '^';  // 40% traps
            }
        }
    }

    floor->reveal_all = true;
}

void check_treasure_room_status(Floor *floor, Player *player) {
    if (floor->floor_index == 3) {
        if (floor->map[player->y][player->x] == '?') {
            create_treasure_room(floor, player);
        }
    }
}

void check_win_condition(Floor *floor, Player *player) {
    if (floor->floor_index != 3 || !floor->reveal_all) {
        return; 
    }

    bool monsters_alive = false;
    for (int i = 0; i < floor_monsters[floor->floor_index].monster_count; i++) {
        if (floor_monsters[floor->floor_index].monsters[i].is_alive) {
            monsters_alive = true;
            break;
        }
    }

    if (!monsters_alive && floor_monsters[floor->floor_index].monster_count > 0) {
        int height = 10, width = 60;
        int start_y = (LINES - height) / 2;
        int start_x = (COLS - width) / 2;

        WINDOW *win_win = newwin(height, width, start_y, start_x);
        box(win_win, 0, 0);
        
        wattron(win_win, COLOR_PAIR(6)); // Yellow color for victory message
        mvwprintw(win_win, 2, (width - 23) / 2, "🎉 CONGRATULATIONS! 🎉");
        mvwprintw(win_win, 4, (width - 35) / 2, "You've defeated all monsters in the dungeon!");
        mvwprintw(win_win, 6, (width - 25) / 2, "Final Gold Collected: %d", player->gold);
        mvwprintw(win_win, 8, (width - 21) / 2, "Press any key to exit");
        wattroff(win_win, COLOR_PAIR(6));
        
        wrefresh(win_win);
        wgetch(win_win);
        delwin(win_win);
        endwin();
        exit(0);
    }
}

void display_game_over(Player *player) {
    int height = 14, width = 70;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *death_win = newwin(height, width, start_y, start_x);
    box(death_win, 0, 0);
    
    for (int i = 0; i < width-2; i++) {
        mvwaddch(death_win, 0, i+1, '=');
        mvwaddch(death_win, height-1, i+1, '=');
    }
    for (int i = 0; i < height-2; i++) {
        mvwaddch(death_win, i+1, 0, '|');
        mvwaddch(death_win, i+1, width-1, '|');
    }
    
    int final_score = (player->gold * 5) + player->health.current_health;
    
    wattron(death_win, COLOR_PAIR(1)); 
    mvwprintw(death_win, 2, (width - 12) / 2, "GAME OVER!");
    wattroff(death_win, COLOR_PAIR(1));
    
    wattron(death_win, COLOR_PAIR(2)); // Stats in different color
    mvwprintw(death_win, 5, 5, "Final Stats:");
    mvwprintw(death_win, 6, 5, "- Gold Collected: %d", player->gold);
    mvwprintw(death_win, 7, 5, "- Health: %d/%d", player->health.current_health, player->health.max_health);
    mvwprintw(death_win, 8, 5, "- Hunger Level: %d%%", player->health.hunger);
    wattroff(death_win, COLOR_PAIR(2));
    
    wattron(death_win, COLOR_PAIR(6)); // Yellow for score
    mvwprintw(death_win, 10, 5, "Final Score: %d (Gold × 5 + Health)", final_score);
    wattroff(death_win, COLOR_PAIR(6));
    
    wattron(death_win, COLOR_PAIR(1));
    mvwprintw(death_win, height-2, (width - 21) / 2, "Press any key to exit");
    wattroff(death_win, COLOR_PAIR(1));
    
    wrefresh(death_win);
    
    for (int i = 0; i < 5; i++) {
        wattron(death_win, A_BOLD);
        wrefresh(death_win);
        napms(200);
        wattroff(death_win, A_BOLD);
        wrefresh(death_win);
        napms(200);
    }
    
    wgetch(death_win);
    delwin(death_win);
    endwin();
    exit(0);
}

void save_game_state(Player *player, Floor floors[]) {
    FILE *save_file = fopen("game_save.bin", "wb");
    if (!save_file) {
        show_message_for_2_seconds("Error: Could not create save file!");
        return;
    }

    // Write player identification data
    if (fwrite(&current_player_data, sizeof(PlayerSaveData), 1, save_file) != 1) {
        fclose(save_file);
        show_message_for_2_seconds("Error saving player data!");
        return;
    }

    // Write current floor
    if (fwrite(&current_floor, sizeof(int), 1, save_file) != 1) {
        fclose(save_file);
        show_message_for_2_seconds("Error saving current floor!");
        return;
    }

    // Write player data
    if (fwrite(player, sizeof(Player), 1, save_file) != 1) {
        fclose(save_file);
        show_message_for_2_seconds("Error saving player stats!");
        return;
    }

    // Write floor data
    for (int i = 0; i < 4; i++) {
        // Save map data
        for (int y = 0; y < MAP_ROWS; y++) {
            if (fwrite(floors[i].map[y], sizeof(char), MAP_COLS, save_file) != MAP_COLS) {
                fclose(save_file);
                show_message_for_2_seconds("Error saving map data!");
                return;
            }
        }

        // Save visibility data
        for (int y = 0; y < MAP_ROWS; y++) {
            if (fwrite(floors[i].visibility[y], sizeof(bool), MAP_COLS, save_file) != MAP_COLS) {
                fclose(save_file);
                show_message_for_2_seconds("Error saving visibility data!");
                return;
            }
        }

        // Save floor metadata
        if (fwrite(&floors[i].floor_index, sizeof(int), 1, save_file) != 1 ||
            fwrite(&floors[i].food_count, sizeof(int), 1, save_file) != 1 ||
            fwrite(&floors[i].spell_count, sizeof(int), 1, save_file) != 1 ||
            fwrite(&floors[i].weapon_count, sizeof(int), 1, save_file) != 1 ||
            fwrite(&floors[i].reveal_all, sizeof(bool), 1, save_file) != 1) {
            fclose(save_file);
            show_message_for_2_seconds("Error saving floor metadata!");
            return;
        }

        // Save room data
        if (fwrite(floors[i].rooms, sizeof(Room), 6, save_file) != 6) {
            fclose(save_file);
            show_message_for_2_seconds("Error saving room data!");
            return;
        }

        // Save items data
        if (fwrite(floors[i].food_items, sizeof(FoodItem), MAX_FOOD_NUMBER * 6, save_file) != MAX_FOOD_NUMBER * 6 ||
            fwrite(floors[i].spell_items, sizeof(SpellItem), MAX_SPELLS_PLAYER, save_file) != MAX_SPELLS_PLAYER ||
            fwrite(floors[i].weapon_items, sizeof(WeaponItem), 20, save_file) != 20) {
            fclose(save_file);
            show_message_for_2_seconds("Error saving items data!");
            return;
        }

        // Save doors and key data
        if (fwrite(floors[i].password_doors, sizeof(PasswordDoor), 6, save_file) != 6 ||
            fwrite(&floors[i].ancient_key, sizeof(AncientKey), 1, save_file) != 1) {
            fclose(save_file);
            show_message_for_2_seconds("Error saving doors and keys!");
            return;
        }
    }

    // Save monster data for each floor
    for (int i = 0; i < 4; i++) {
        if (fwrite(&floor_monsters[i].monster_count, sizeof(int), 1, save_file) != 1 ||
            fwrite(floor_monsters[i].monsters, sizeof(Monster), MAX_MONSTERS_PER_FLOOR, save_file) != MAX_MONSTERS_PER_FLOOR) {
            fclose(save_file);
            show_message_for_2_seconds("Error saving monster data!");
            return;
        }
    }

    // Save collected spells and food
    if (fwrite(collected_spells, sizeof(SpellItem), MAX_SPELLS_PLAYER, save_file) != MAX_SPELLS_PLAYER ||
        fwrite(stored_food, sizeof(FoodItem), 6, save_file) != 6 ||
        fwrite(&spell_in_room, sizeof(int), 1, save_file) != 1 ||
        fwrite(&food_count, sizeof(int), 1, save_file) != 1) {
        fclose(save_file);
        show_message_for_2_seconds("Error saving collected items!");
        return;
    }

    fclose(save_file);
    show_message_for_2_seconds("Game saved successfully!");
}

bool load_game_state(Player *player, Floor floors[]) {
    FILE *save_file = fopen("game_save.bin", "rb");
    if (!save_file) {
        return false;
    }

    // Read player identification data
    PlayerSaveData loaded_player_data;
    if (fread(&loaded_player_data, sizeof(PlayerSaveData), 1, save_file) != 1) {
        fclose(save_file);
        return false;
    }

    // Verify if the loaded save belongs to the current player
    if (strcmp(loaded_player_data.username, current_player_data.username) != 0) {
        fclose(save_file);
        return false;
    }

    // Read current floor
    if (fread(&current_floor, sizeof(int), 1, save_file) != 1) {
        fclose(save_file);
        return false;
    }

    // Read player data
    if (fread(player, sizeof(Player), 1, save_file) != 1) {
        fclose(save_file);
        return false;
    }

    // Read floor data
    for (int i = 0; i < 4; i++) {
        // Allocate memory for the map if not already allocated
        if (!floors[i].map) {
            floors[i].map = malloc(MAP_ROWS * sizeof(char *));
            for (int y = 0; y < MAP_ROWS; y++) {
                floors[i].map[y] = malloc(MAP_COLS * sizeof(char));
            }
        }

        // Allocate memory for visibility if not already allocated
        if (!floors[i].visibility) {
            floors[i].visibility = malloc(MAP_ROWS * sizeof(bool *));
            for (int y = 0; y < MAP_ROWS; y++) {
                floors[i].visibility[y] = malloc(MAP_COLS * sizeof(bool));
            }
        }

        // Load map data
        for (int y = 0; y < MAP_ROWS; y++) {
            if (fread(floors[i].map[y], sizeof(char), MAP_COLS, save_file) != (size_t)MAP_COLS) {
                fclose(save_file);
                return false;
            }
        }

        // Load visibility data
        for (int y = 0; y < MAP_ROWS; y++) {
            if (fread(floors[i].visibility[y], sizeof(bool), MAP_COLS, save_file) != (size_t)MAP_COLS) {
                fclose(save_file);
                return false;
            }
        }

        // Load floor metadata
        size_t items_read = 0;
        items_read += fread(&floors[i].floor_index, sizeof(int), 1, save_file);
        items_read += fread(&floors[i].food_count, sizeof(int), 1, save_file);
        items_read += fread(&floors[i].spell_count, sizeof(int), 1, save_file);
        items_read += fread(&floors[i].weapon_count, sizeof(int), 1, save_file);
        items_read += fread(&floors[i].reveal_all, sizeof(bool), 1, save_file);
        
        if (items_read != 5) {
            fclose(save_file);
            return false;
        }

        // Load room data
        if (fread(floors[i].rooms, sizeof(Room), 6, save_file) != 6) {
            fclose(save_file);
            return false;
        }

        // Load items data
        if (fread(floors[i].food_items, sizeof(FoodItem), MAX_FOOD_NUMBER * 6, save_file) != MAX_FOOD_NUMBER * 6 ||
            fread(floors[i].spell_items, sizeof(SpellItem), MAX_SPELLS_PLAYER, save_file) != MAX_SPELLS_PLAYER ||
            fread(floors[i].weapon_items, sizeof(WeaponItem), 20, save_file) != 20) {
            fclose(save_file);
            return false;
        }

        // Load doors and key data
        if (fread(floors[i].password_doors, sizeof(PasswordDoor), 6, save_file) != 6 ||
            fread(&floors[i].ancient_key, sizeof(AncientKey), 1, save_file) != 1) {
            fclose(save_file);
            return false;
        }
    }

    // Load monster data for each floor
    for (int i = 0; i < 4; i++) {
        if (fread(&floor_monsters[i].monster_count, sizeof(int), 1, save_file) != 1 ||
            fread(floor_monsters[i].monsters, sizeof(Monster), MAX_MONSTERS_PER_FLOOR, save_file) != MAX_MONSTERS_PER_FLOOR) {
            fclose(save_file);
            return false;
        }
    }

    // Load collected spells and food
    if (fread(collected_spells, sizeof(SpellItem), MAX_SPELLS_PLAYER, save_file) != MAX_SPELLS_PLAYER ||
        fread(stored_food, sizeof(FoodItem), 6, save_file) != 6 ||
        fread(&spell_in_room, sizeof(int), 1, save_file) != 1 ||
        fread(&food_count, sizeof(int), 1, save_file) != 1) {
        fclose(save_file);
        return false;
    }

    fclose(save_file);
    show_message_for_2_seconds("Game loaded successfully!");
    return true;
}

void scan_area(Floor *floor, Player *player, int direction) {
    bool original_visibility[MAP_ROWS][MAP_COLS];
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            original_visibility[i][j] = floor->visibility[i][j];
        }
    }
    for (int dy = -8; dy <= 8; dy++) {
        for (int dx = -8; dx <= 8; dx++) {
            int new_y = player->y + dy;
            int new_x = player->x + dx;
            
            if (new_y >= 0 && new_y < MAP_ROWS && 
                new_x >= 0 && new_x < MAP_COLS) {
                double distance = sqrt(dx*dx + dy*dy);
                if (distance <= 8) {
                    floor->visibility[new_y][new_x] = true;
                }
            }
        }
    }
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
    napms(2000);
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            floor->visibility[i][j] = original_visibility[i][j];
        }
    }
    render_map(floor);
    mvaddch(player->y, player->x, '@');
    refresh();
}

void handle_scan_mode(int ch, Player *player, Floor *floor) {
    show_message_for_2_seconds("Choose direction to scan");
    int direction = getch();
    scan_area(floor, player, direction);
}

void display_ancient_key_menu(Player *player) {
    int height = 10, width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *key_menu_win = newwin(height, width, start_y, start_x);
    box(key_menu_win, 0, 0);
    
    wattron(key_menu_win, COLOR_PAIR(6));
    mvwprintw(key_menu_win, 1, (width - 16) / 2, "Ancient Keys Menu");
    wattroff(key_menu_win, COLOR_PAIR(6));
    
    mvwprintw(key_menu_win, 3, 2, "Working Ancient Keys: %d", player->ancient_keys);
    mvwprintw(key_menu_win, 4, 2, "Broken Ancient Keys: %d", player->broken_keys);
    
    if (player->broken_keys >= 2) {
        wattron(key_menu_win, COLOR_PAIR(2));
        mvwprintw(key_menu_win, 6, 2, "Press 'C' to combine 2 broken keys");
        wattroff(key_menu_win, COLOR_PAIR(2));
    }
    
    mvwprintw(key_menu_win, height-2, 2, "Press any other key to close");
    
    wrefresh(key_menu_win);
    
    int ch = wgetch(key_menu_win);
    if ((ch == 'c' || ch == 'C') && player->broken_keys >= 2) {
        player->broken_keys -= 2;
        player->ancient_keys++;
        show_message_for_2_seconds("Combined 2 broken keys into a working Ancient Key!");
    }
    
    werase(key_menu_win);
    wrefresh(key_menu_win);
    delwin(key_menu_win);
    refresh();
}

bool is_adjacent_to_password_door(Floor *floor, Player *player, int *door_x, int *door_y) {
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
    
    for (int i = 0; i < 8; i++) {
        int new_x = player->x + dx[i];
        int new_y = player->y + dy[i];
        
        if (floor->map[new_y][new_x] == 'P') {
            *door_x = new_x;
            *door_y = new_y;
            return true;
        }
    }
    return false;
}

void place_windows(Floor *floor, Room *room, int room_number) {
    if (floor->floor_index != 0 || room_number != 1) {
        return;
    }

    int window_spacing = room->height / 3;
    bool position_found = false;
    int attempts = 0;
    int window_y1 = 0, window_y2 = 0;
    
    while (!position_found && attempts < 10) {
        window_y1 = room->y + window_spacing;
        window_y2 = room->y + (2 * window_spacing);
        
        bool conflict = false;
        for (int i = 0; i < 4; i++) {
            if (room->door_coords[i][0] == room->x + room->width - 1 && 
                (room->door_coords[i][1] == window_y1 || 
                 room->door_coords[i][1] == window_y2)) {
                conflict = true;
                break;
            }
        }
        
        for (int i = 0; i < 6; i++) {
            if (floor->password_doors[i].x == room->x + room->width - 1 && 
                (floor->password_doors[i].y == window_y1 || 
                 floor->password_doors[i].y == window_y2)) {
                conflict = true;
                break;
            }
        }
        
        if (!conflict && 
            floor->map[window_y1][room->x + room->width - 1] == '|' && 
            floor->map[window_y2][room->x + room->width - 1] == '|') {
            position_found = true;
        } else {
            window_spacing--;
            attempts++;
        }
    }
    
    if (!position_found) {
        return;
    }

    room->windows[0].x = room->x + room->width - 1;
    room->windows[0].y = window_y1;
    room->windows[0].is_visible = false;
    floor->map[window_y1][room->x + room->width - 1] = '=';

    room->window_count = 1;
}

void check_windows(Floor *floor, Player *player) {
    for (int i = 0; i < 6; i++) {
        Room *room = &floor->rooms[i];
        if (player->x >= room->x && player->x < room->x + room->width &&
            player->y >= room->y && player->y < room->y + room->height) {
            
            if (floor->floor_index == 0 && i == 1) {
                if (floor->map[player->y][player->x] == '=') {
                    // Room 3 visible (right room)
                    Room *right_room = &floor->rooms[2];
                    
                    for (int y = right_room->y; y < right_room->y + right_room->height; y++) {
                        for (int x = right_room->x; x < right_room->x + right_room->width; x++) {
                            floor->visibility[y][x] = true;
                        }
                    }
                    
                    render_map(floor);
                    attron(COLOR_PAIR(character_color));
                    mvaddch(player->y, player->x, '@');
                    attroff(COLOR_PAIR(character_color));
                    refresh();
                    
                    show_message_for_2_seconds("You can see into the next room through the window!");
                }
            }
            return;
        }
    }
}

void make_corridor_visible(Floor *floor, int x, int y) {
    // Make the corridor and surrounding area visible
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int new_y = y + dy;
            int new_x = x + dx;
            
            if (new_y >= 0 && new_y < MAP_ROWS && 
                new_x >= 0 && new_x < MAP_COLS) {
                // If it's a corridor or surrounding area
                if (floor->map[new_y][new_x] == '#' ||
                    floor->map[new_y][new_x] == '+' ||
                    floor->map[new_y][new_x] == '=' ||
                    floor->map[new_y][new_x] == '.') {
                    floor->visibility[new_y][new_x] = true;
                }
            }
        }
    }
}

void update_room_visibility(Floor *floor, Player *player) {
    Room *current_room = NULL;
    int current_room_index = -1;
    
    for (int i = 0; i < 6; i++) {
        Room *room = &floor->rooms[i];
        if (player->x >= room->x && player->x < room->x + room->width &&
            player->y >= room->y && player->y < room->y + room->height) {
            current_room = room;
            current_room_index = i;
            break;
        }
    }
    if (current_room) {
        if (current_room->theme == NIGHTMARE_ROOM) {
            for (int y = 0; y < MAP_ROWS; y++) {
                for (int x = 0; x < MAP_COLS; x++) {
                    floor->visibility[y][x] = false;
                }
            }
            
            for (int dy = -4; dy <= 4; dy++) {
                for (int dx = -4; dx <= 4; dx++) {
                    int new_y = player->y + dy;
                    int new_x = player->x + dx;
                    
                    if (new_y >= 0 && new_y < MAP_ROWS && 
                        new_x >= 0 && new_x < MAP_COLS) {
                        double distance = sqrt(dx*dx + dy*dy);
                        if (distance <= 2) {
                            floor->visibility[new_y][new_x] = true;
                        }
                    }
                }
            }
            return;
        }
        
        if (!current_room->visited) {
            current_room->visited = true;
            for (int y = current_room->y; y < current_room->y + current_room->height; y++) {
                for (int x = current_room->x; x < current_room->x + current_room->width; x++) {
                    floor->visibility[y][x] = true;
                }
            }
            for (int i = 0; i < 4; i++) {
                if (current_room->door_coords[i][0] != 0) {
                    make_corridor_visible(floor, current_room->door_coords[i][0], 
                                       current_room->door_coords[i][1]);
                }
            }
        }
    }
    
    for (int i = 0; i < 6; i++) {
        Room *room = &floor->rooms[i];
        if (room->visited && room->theme != NIGHTMARE_ROOM) {
            for (int y = room->y; y < room->y + room->height; y++) {
                for (int x = room->x; x < room->x + room->width; x++) {
                    floor->visibility[y][x] = true;
                }
            }
            for (int j = 0; j < 4; j++) {
                if (room->door_coords[j][0] != 0) {
                    make_corridor_visible(floor, room->door_coords[j][0], 
                                       room->door_coords[j][1]);
                }
            }
        }
    }
}

bool show_collection_prompt(const char *item_name) {
    int height = 3, width = strlen(item_name) + 40;
    int start_y = 1;
    int start_x = 1;

    WINDOW *prompt_win = newwin(height, width, start_y, start_x);
    box(prompt_win, 0, 0);
    
    wattron(prompt_win, COLOR_PAIR(6));
    mvwprintw(prompt_win, 1, 2, "Collect %s? (y/n)", item_name);
    wattroff(prompt_win, COLOR_PAIR(6));
    
    wrefresh(prompt_win);
    
    int ch = wgetch(prompt_win);
    
    werase(prompt_win);
    wrefresh(prompt_win);
    delwin(prompt_win);
    refresh();
    
    return (ch == 'y' || ch == 'Y');
}

void check_and_collect_spell(Floor *floor, Player *player, int x, int y) {
    for (int i = 0; i < floor->spell_count; i++) {
        if (x == floor->spell_items[i].x && 
            y == floor->spell_items[i].y && 
            !floor->spell_items[i].collected) {
            
            const char* spell_name;
            switch (floor->map[y][x]) {
                case 'H': spell_name = "health spell"; break;
                case 'V': spell_name = "speed spell"; break;
                case 'D': spell_name = "damage spell"; break;
                default: spell_name = "spell";
            }

            if (show_collection_prompt(spell_name)) {
                floor->spell_items[i].collected = true;
                switch(floor->spell_items[i].type) {
                    case SPELL_HEALTH:
                        player->health_spells++;
                        show_message_for_2_seconds("Health Spell Collected! (💉)");
                        break;
                    case SPELL_SPEED:
                        player->speed_spells++;
                        show_message_for_2_seconds("Speed Spell Collected! (🔥)");
                        break;
                    case SPELL_DAMAGE:
                        player->damage_spells++;
                        show_message_for_2_seconds("Damage Spell Collected! (💀)");
                        break;
                }
                player->collected_spells[player->spell_count] = floor->spell_items[i];
                player->spell_count++;
                floor->map[y][x] = '.';
            }
            break;
        }
    }
}

void start_game() {
    int ch;
    int scr_row, scr_col;
    initscr();
    srand(time(NULL));
    curs_set(0);
    noecho();
    // keypad(stdscr, 1);
    getmaxyx(stdscr, scr_row, scr_col);
    start_color();
    colorspair();

    initialize_all_floors(scr_row, scr_col);
    generate_all_floors(scr_row, scr_col);
    
    Player player = {
        .username = "", 
        .email = "",
        .x = floors[0].rooms[0].x + 1,
        .y = floors[0].rooms[0].y + 1,
        .gold = 0,
        .traps = 0,
        .completed_games = 0,
        .basic_food_count = 0,
        .premium_food_count = 0,
        .magical_food_count = 0,
        .spoiled_food_count = 0,
        .spell_count = 0,
        .ancient_keys = 0,
        .broken_keys = 0,
        .weapons = {
            .mace_count = 1,
            .dagger_count = 0,
            .wand_count = 0,
            .arrow_count = 0,
            .sword_count = 0,
            .weapon_count = 1,
            .equipped_weapon = MACE,
            .has_weapon_equipped = true
        }
    };
    strncpy(player.username, current_player_data.username, sizeof(player.username) - 1);
    player.username[sizeof(player.username) - 1] = '\0';
    time_t session_start = time(NULL);

    init_health_system(&player.health);
    player.last_food_check = time(NULL);

    update_visibility(&floors[current_floor], &player);
    render_map(&floors[current_floor]);
    mvaddch(player.y, player.x, '@');
    
    time_t last_health_update = time(NULL);
    time_t last_food_spoil_check = time(NULL);
    
    do {
        time_t current_time = time(NULL);
        
        if (current_time - last_health_update >= 1) {
            update_health_system(&player.health);
            last_health_update = current_time;
        }
        
        if (current_time - last_food_spoil_check >= FOOD_SPOIL_CHECK_INTERVAL) {
            check_food_spoilage(&player);
            last_food_spoil_check = current_time;
        }
        
        attron(COLOR_PAIR(6));
        mvprintw(scr_row - 1, 0, "Player: %s | Health: %d/%d | Hunger: %d%% | Gold: %d%s", 
            player.username,
            player.health.current_health,
            player.health.max_health,
            player.health.hunger,
            player.gold,
            player.health.is_regenerating ? " | [Regenerating]" : "");
        attroff(COLOR_PAIR(6));
        
        MonsterList *current_monsters = &floor_monsters[current_floor];
        for (int i = 0; i < current_monsters->monster_count; i++) {
            Monster *monster = &current_monsters->monsters[i];
            if (monster->is_alive) {
                int dx = abs(monster->x - player.x);
                int dy = abs(monster->y - player.y);
                if (dx <= 1 && dy <= 1) {
                    take_damage(&player.health, monster->damage);
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Monster attack! -%d health", monster->damage);
                    show_message_for_2_seconds(msg);
                }
            }
        }

        ch = getch();
        if (ch == 32) { 
            switch(player.weapons.equipped_weapon) {
                case MACE:
                    handle_mace_attack(&player, &floors[current_floor]);
                    break;
                case DAGGER:
                    handle_dagger_attack(&player, &floors[current_floor]);
                    break;
                case MAGIC_WAND:
                    handle_magic_wand_attack(&player, &floors[current_floor]);
                    break;
                case NORMAL_ARROW:
                    handle_arrow_attack(&player, &floors[current_floor]);
                    break;
                case SWORD:
                    handle_sword_attack(&player, &floors[current_floor]);
                    break;
            }
        } else {
            move_player(ch, &player, &floors[current_floor], scr_row, scr_col);
        }
        
        update_room_visibility(&floors[current_floor], &player);
        update_monsters(&floors[current_floor], &player);
        check_win_condition(&floors[current_floor], &player);
        
        if (ch == 'Q') {
            display_spell_menu(&player);
        }
        if (ch == 'E') {
            display_food_menu(&player);
        }
        if (ch == 'I') {
            display_weapon_menu(&player);
        }
        if (ch == 'S') {
            save_game_state(&player, floors);
        }
        if (ch == 'L') {
            load_game_state(&player, floors);
            save_leaderboard_data(&player);

        }
        if (ch == 'A') {
            display_ancient_key_menu(&player);
        }
        if (player.health.current_health <= 0) {
            display_game_over(&player);
        }
        check_treasure_room_status(&floors[current_floor], &player);
        refresh();
    } while (ch != 27);  // ESC to exit

    time_t end_time = time(NULL);
    int total_playtime = (int)difftime(end_time, session_start);
    char filename[256];
    snprintf(filename, sizeof(filename), "player_data/%s_stats.txt", player.username);
    mkdir("player_data", 0777);
    FILE *player_file = fopen(filename, "a");
    if (player_file) {
        fprintf(player_file, "Session End: %s", ctime(&end_time));
        fprintf(player_file, "Playtime: %d minutes\n", total_playtime / 60);
        fprintf(player_file, "Final Gold: %d\n", player.gold);
        fprintf(player_file, "Final Health: %d/%d\n", 
                player.health.current_health, player.health.max_health);
        fprintf(player_file, "------------------\n");
        fclose(player_file);
    }
    free_all_floors(scr_row);
}

