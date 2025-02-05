#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/stat.h>


static sqlite3 *db = NULL;
bool db_enabled = false;

int init_database() {
    mkdir("database", 0777);
    
    int rc = sqlite3_open("database/game.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        db_enabled = false;
        return 0;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS players ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "username TEXT UNIQUE NOT NULL,"
                     "email TEXT UNIQUE NOT NULL,"
                     "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                     "last_login DATETIME)";

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        db_enabled = false;
        return 0;
    }
    
    sql = "CREATE TABLE IF NOT EXISTS player_stats ("
          "player_id INTEGER PRIMARY KEY,"
          "total_gold INTEGER DEFAULT 0,"
          "completed_games INTEGER DEFAULT 0,"
          "current_health INTEGER DEFAULT 100,"
          "max_health INTEGER DEFAULT 100,"
          "FOREIGN KEY(player_id) REFERENCES players(id))";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        db_enabled = false;
        return 0;
    }

    db_enabled = true;
    return 1;
}

int store_player_db(const char *username, const char *email) {
    if (!db_enabled || !db) {
        return 0;
    }

    char *sql = sqlite3_mprintf("INSERT INTO players (username, email, last_login) "
                               "VALUES (%Q, %Q, CURRENT_TIMESTAMP)", 
                               username, email);
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    sqlite3_int64 player_id = sqlite3_last_insert_rowid(db);
    
    sql = sqlite3_mprintf("INSERT INTO player_stats (player_id) VALUES (%lld)",
                         player_id);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    return 1;
}

int update_player_stats_db(const char *username, int gold, int completed_games, 
                          int current_health, int max_health) {
    if (!db_enabled || !db) {
        return 0;
    }

    char *sql = sqlite3_mprintf(
        "UPDATE player_stats SET "
        "total_gold = %d, "
        "completed_games = %d, "
        "current_health = %d, "
        "max_health = %d "
        "WHERE player_id = (SELECT id FROM players WHERE username = %Q)",
        gold, completed_games, current_health, max_health, username);
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    return 1;
}

int load_player_stats_db(const char *username, int *gold, int *completed_games,
                        int *current_health, int *max_health) {
    if (!db_enabled || !db) {
        return 0;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT ps.total_gold, ps.completed_games, "
                     "ps.current_health, ps.max_health "
                     "FROM player_stats ps "
                     "JOIN players p ON ps.player_id = p.id "
                     "WHERE p.username = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch player stats: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        *gold = sqlite3_column_int(stmt, 0);
        *completed_games = sqlite3_column_int(stmt, 1);
        *current_health = sqlite3_column_int(stmt, 2);
        *max_health = sqlite3_column_int(stmt, 3);
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

void close_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}