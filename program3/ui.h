#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED
#include "library.h"
#include "database_menu.h"
void drawBorder(WINDOW* win);
void printTitle(WINDOW* win, const char* title, int width);
void printCentered(WINDOW* win, int y, const char* msg, int width);
int navigateMenu(WINDOW* win, char* choices[], int n_choices, int startY, int startX);
void showMainMenu();
void showStartupScreen();

#endif // UI_H_INCLUDED
