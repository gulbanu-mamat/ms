#ifndef LOGIN_SYSTEM_H
#define LOGIN_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 20
#define MAX_USERS 100
#define USERS_FILE "users.txt"

// 用户结构体
typedef struct {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char status[10]; // "initial" 或 "normal"
} User;

// 函数声明
int load_users(User users[], int *user_count);
int save_users(User users[], int user_count);
int authenticate_user(const char *username, const char *password, User users[], int user_count, int *user_index);
int is_valid_password(const char *password);
void generate_random_password(char *password);
void change_password(User users[], int user_count, int user_index);
void admin_menu(User users[], int *user_count);
void user_menu(User users[], int user_count, int user_index);
void register_user(User users[], int *user_count);
void reset_user_password(User users[], int user_count);
void clear_input_buffer();
void create_user_directory(const char *username);

#endif 