#ifndef AUTH_SYSTEM_H
#define AUTH_SYSTEM_H

// C++标准库头文件
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ncurses.h>

// 使用标准命名空间
using namespace std;

#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 20
#define MAX_USERS 100
#define USERS_FILE "users.txt"

// User structure
typedef struct {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char status[10]; // "initial" or "normal"
} User;

// Authentication system functions
int loadUsers(User users[], int *userCount);
int saveUsers(User users[], int userCount);
int authenticateUser(const char *username, const char *password, User users[], int userCount, int *userIndex);
bool isValidPassword(const char *password);
void generateRandomPassword(char *password);
void changePassword(User users[], int userCount, int userIndex);
void adminMenu(User users[], int *userCount);
void userMenu(User users[], int userCount, int userIndex, const char *username);
void registerUser(User users[], int *userCount);
void resetUserPassword(User users[], int userCount);
void clearInputBuffer();
void createUserDirectory(const char *username);

// User interface functions with ncurses
int showLoginScreen(User users[], int userCount);
void showWelcomeScreen();
void displayMessage(const char *message, bool isError = false);
void waitForKey();
int showPasswordChangeScreen(User users[], int userCount, int userIndex);
int showAdminMenuScreen(User users[], int *userCount);
int showUserMenuScreen(User users[], int userCount, int userIndex, const char *username);

// Utility functions
void setupColors();
void drawFrame(WINDOW* win, const char* title);
int getSecureInput(char* buffer, int maxLen, bool isPassword = false);

#endif 