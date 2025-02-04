#ifndef GAME_H
#define GAME_H

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
#define _XOPEN_SOURCE_EXTENDED 1
#define MAP_ROWS LINES
#define MAP_COLS COLS
#define MAX_FOOD_NUMBER 3
#define MAX_SPELLS_PLAYER 50
#define MAX_MONSTERS_PER_FLOOR 20
#define MAX_HEALTH 100
#define MAX_HUNGER 100
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define HEALTH_REGEN_RATE 2
#define HEALTH_REGEN_INTERVAL 5 // seconds
#define FOOD_SPOIL_CHECK_INTERVAL 30 // seconds
#define HUNGER_DECREASE_RATE 1
#define HUNGER_CHECK_INTERVAL 10 // seconds
#define TRAP_DAMAGE_EASY 3
#define TRAP_DAMAGE_MEDIUM 5
#define TRAP_DAMAGE_HARD 10

#define DIFFICULTY_EASY 1
#define DIFFICULTY_MEDIUM 2
#define DIFFICULTY_HARD 3

// Health values for different difficulties
#define HEALTH_EASY 150
#define HEALTH_MEDIUM 100
#define HEALTH_HARD 75

// Trap damage for different difficulties
#define TRAP_DAMAGE_EASY 3
#define TRAP_DAMAGE_MEDIUM 5
#define TRAP_DAMAGE_HARD 10

typedef struct {
    int rank;
    char username[50];
    int totalPoints;
    int totalGold;
    int gamesCompleted;
    int timeSinceFirstGame;
    int isCurrentUser;
    char title[10];
    char emoji[10];
} LeaderboardEntry;

typedef struct {
    LeaderboardEntry entries[100];
    int totalEntries;
    int currentPage;
} LeaderboardData;


typedef struct {
    int current_health;
    int max_health;
    int hunger;
    time_t last_damage_time;
    time_t last_health_regen;
    time_t last_hunger_update;
    bool is_regenerating;
    bool health_spell_active;
    time_t health_spell_start_time;
    bool speed_spell_active;
    time_t speed_spell_start_time;
    bool damage_spell_active;
    time_t damage_spell_start_time;
} HealthSystem;

typedef struct {
    int x;
    int y;
    char password[5];
    char second_password[5];
    bool second_password_active;
    bool is_locked;
    int attempts_left;
    time_t password_time;
} PasswordDoor;

typedef struct {
    int x;
    int y;
    bool is_visible;
} Window;

typedef struct {
    int x;
    int y;
    bool collected;
    bool is_broken;
} AncientKey;

typedef enum {
    NORMAL_ROOM,
    ENCHANTED_ROOM,
    TREASURE_ROOM,
    NIGHTMARE_ROOM
} RoomTheme;

typedef enum {
    FOOD_BASIC,
    FOOD_PREMIUM,
    FOOD_MAGICAL,
    FOOD_SPOILED
} FoodType;

typedef struct {
    FoodType type;
    int x; int y;
    int healthRestore;
    bool isSpoiled;
    bool collected;
    time_t collection_time;
} FoodItem;

typedef enum {
    SPELL_HEALTH,
    SPELL_SPEED,
    SPELL_DAMAGE
} SpellType;
typedef struct {
    SpellType type;
    int x;
    int y;
    bool collected;
} SpellItem;


typedef struct Room {
    int x, y, width, height;
    int door_coords[4][2];
    int staircase_x;
    int staircase_y;
    RoomTheme theme;
    bool visited;
    Window windows[4];
    int window_count;
} Room;

typedef enum {
    MACE,        // گرز
    DAGGER,      // خنجر
    MAGIC_WAND,  // عصای جادویی
    NORMAL_ARROW,
    SWORD 
} WeaponType;

typedef struct {
    WeaponType type;
    int x, y;
    bool collected;
    bool is_dropped;
} WeaponItem;

typedef struct {
    int damage;
    int range;
    int quantity;
    int max_quantity;
    bool is_throwable;
    bool is_permanent;
    char menu_char;
} WeaponStats;


typedef struct {
    int mace_count;
    int dagger_count;
    int wand_count;
    int arrow_count;
    int sword_count;
    WeaponItem collected_weapons[50];
    int weapon_count;
    WeaponType equipped_weapon; 
    bool has_weapon_equipped;
} WeaponInventory;


typedef enum {
    DEMON,      // D - Daemien
    FIRE,       // F - Fire Breathing
    GIANT,      // G - Giant 
    SNAKE,      // S - Snake (follows until death)
    UNDEAD      // U - Strongest (follows for 5 steps)
} MonsterType;

typedef struct {
    MonsterType type;
    int x, y; 
    int initial_x;
    int initial_y;
    int health;
    int max_health;
    int damage;
    bool is_alive;
    int follow_steps;
    bool is_following;
    bool is_paralyzed;
} Monster;

typedef struct {
    Monster monsters[MAX_MONSTERS_PER_FLOOR];
    int monster_count;
} MonsterList;



typedef struct Floor {
    char **map;
    Room rooms[6];
    int floor_index;
    FoodItem food_items[MAX_FOOD_NUMBER * 6];
    SpellItem spell_items[MAX_SPELLS_PLAYER];
    int food_count;
    int spell_count;
    WeaponItem weapon_items[20];
    int weapon_count;
    PasswordDoor password_doors[6];
    AncientKey ancient_key;
    bool **visibility;
    bool reveal_all; 
} Floor;



typedef struct Player {
    char username[50];
    char email[100];
    int x;
    int y;
    int gold;
    int traps;
    int completed_games;
    HealthSystem health;
    FoodItem basic_food[5];
    int basic_food_count;
    FoodItem premium_food[5];
    int premium_food_count;
    FoodItem magical_food[5];
    int magical_food_count;
    FoodItem spoiled_food[5];
    int spoiled_food_count;
    int spell_count;
    int health_spells;
    int speed_spells;
    int damage_spells;
    SpellItem collected_spells[MAX_SPELLS_PLAYER];
    WeaponInventory weapons;
    int ancient_keys;
    int broken_keys;
    time_t last_food_check;
} Player;

typedef struct PlayerSaveData {
    char username[50];
    bool is_guest;
    time_t last_save_time;
} PlayerSaveData;


extern const WeaponStats WEAPON_PROPERTIES[];
extern Floor floors[4];
extern int current_floor;
extern MonsterList floor_monsters[4];
extern int spell_in_room; 
extern SpellItem collected_spells[MAX_SPELLS_PLAYER]; 
extern int character_color;
extern FoodItem stored_food[6];
extern int food_count;
extern Player current_player;
extern PlayerSaveData current_player_data;
extern int difficulty;


void display_profile_menu(void);
void colorspair();
int  random_generator(int start, int end) ;
void place_staircase(Floor *floor, Room *room, char staircase_type) ;
void initialize_floor(Floor *floor, int rows, int cols);
void initialize_all_floors(int rows, int cols);
void free_all_floors(int rows);void render_map(Floor *floor);
void generate_room(int max_xi, int max_yi, int max_x, int max_y, int *x, int *y, int *width, int *height);
void draw_room(Floor *floor, Room *room);
void place_doors(Floor *floor, Room *room, int is_edge_room, int room_id);
void create_corridor(Floor *floor, int x1, int y1, int x2, int y2);
void roommaker(Floor *floor);
void move_player(int ch, Player *player,Floor *floor, int scr_row, int scr_col);
void place_traps(Floor *floor, Room *room);
void place_password_doors(Floor *floor,Room *room, PasswordDoor *door);
void generate_password(PasswordDoor *door);
void place_password_button(Floor *floor,Room *room);
void display_password_for_30_seconds(Floor *floor, PasswordDoor *password);
void place_food(Floor *floor, Room *room, FoodItem *food_list, int *food_count);
void place_gold(Floor *floor, Room *room);
void place_black_gold(Floor *floor, Room *room) ;
void show_message_for_2_seconds(const char *message);
void collect_food(Player *player, Floor *floor, int x, int y);
void place_spells(Floor *floor, Room *room, SpellItem *spell_list, int *spell_count);
void collect_spell(Floor *floor, Player *player, SpellItem *spell_list, int *spell_count);
void display_floor_name(int floor_index);
void place_weapons(Floor *floor, Room *room);
void collect_weapon(Floor *floor, Player *player, int x, int y);
void display_weapon_menu(Player *player);
void check_secret_doors(Floor *floor, Player *player);
void check_password_door(Floor *floor, Player *player, int new_x, int new_y);
void place_ancient_key(Floor *floor, Room *room);
void collect_ancient_key(Player *player, Floor *floor, int x, int y) ;
bool use_ancient_key(Floor *floor, Player *player, int door_x, int door_y);
void end_game(Player *player);
void place_monsters(Floor *floor, Room *room);
void update_monsters(Floor *floor, Player *player);
bool is_valid_monster_move(Floor *floor, int x, int y, Player *player);
Monster create_monster(MonsterType type, int x, int y);
char get_monster_char(MonsterType type);
bool can_move_to_position(Floor *floor, int x, int y);
bool is_monster_at_position(Floor *floor, int x, int y) ;
void handle_mace_attack(Player *player, Floor *floor);
void handle_dagger_attack(Player *player, Floor *floor);
void get_throw_direction(int key, int *dx, int *dy);
void handle_magic_wand_attack(Player *player, Floor *floor);
void drop_weapon(Floor *floor, WeaponType type, int x, int y);
void init_health_system(HealthSystem *health);
void update_health_system(HealthSystem *health);
void take_damage(HealthSystem *health, int damage);
void heal_health(HealthSystem *health, int amount);
void add_hunger(HealthSystem *health, int amount);
void check_food_spoilage(Player *player);
void activate_health_spell(Player *player);
void init_health_system(HealthSystem *health);
void move_player_speed(int ch, Player *player, Floor *floor, int scr_row, int scr_col);
void activate_speed_spell(Player *player) ;
void activate_damage_spell(Player *player);
int  get_weapon_damage(WeaponType type, Player *player);
void place_question_mark(Floor *floor, Room *room);
void create_treasure_room(Floor *floor, Player *player);
void check_treasure_room_status(Floor *floor, Player *player);
void check_win_condition(Floor *floor, Player *player);
void display_game_over(Player *player);
void save_game_state(Player *player, Floor floors[]);
bool load_game_state(Player *player, Floor floors[]);
void start_game();
void initialize_player(Player *player, const char *username, const char *email);
void save_leaderboard_data(Player *player);
void scan_area(Floor *floor, Player *player, int direction);
void handle_scan_mode(int ch, Player *player, Floor *floor);
bool check_reverse_password(const char *input, const char *password);
void display_ancient_key_menu(Player *player);
bool is_adjacent_to_password_door(Floor *floor, Player *player, int *door_x, int *door_y);
void place_windows(Floor *floor, Room *room, int room_number);
void check_windows(Floor *floor, Player *player);
void update_room_visibility(Floor *floor, Player *player);
void make_corridor_visible(Floor *floor, int x, int y);
bool show_collection_prompt(const char *item_name);
void check_and_collect_spell(Floor *floor, Player *player, int x, int y);
int  play_room_music(RoomTheme theme);
void run_anime_intro();
void loadLeaderboard(LeaderboardData *data);
int  find_user_by_email(const char* email, char* username);

#endif