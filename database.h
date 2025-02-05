#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

extern bool db_enabled;

int init_database(void);
int store_player_db(const char *username, const char *email);
int update_player_stats_db(const char *username, int gold, int completed_games, 
                          int current_health, int max_health);
int load_player_stats_db(const char *username, int *gold, int *completed_games,
                        int *current_health, int *max_health);
void close_database(void);

#endif