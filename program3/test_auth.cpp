#include "auth_system.h"

int main() {
    // Initialize ncurses
    initscr();
    if (has_colors()) {
        setupColors();
    }
    noecho();
    cbreak();
    curs_set(0);
    
    User users[MAX_USERS];
    int userCount = 0;
    
    // Load existing users
    if (!loadUsers(users, &userCount)) {
        endwin();
        printf("Error loading users!\n");
        return 1;
    }
    
    printf("Loaded %d users\n", userCount);
    
    while (true) {
        clear();
        
        // Show title
        mvprintw(2, 10, "=== ENHANCED AUTHENTICATION SYSTEM TEST ===");
        mvprintw(4, 10, "Features:");
        mvprintw(5, 12, "1. Password failure tracking (3 attempts)");
        mvprintw(6, 12, "2. Account freezing for 5 seconds");
        mvprintw(7, 12, "3. Admin user deletion capability");
        mvprintw(9, 10, "Press L to Login, A for Admin Menu, Q to Quit");
        
        refresh();
        
        int ch = getch();
        
        switch (ch) {
            case 'L':
            case 'l': {
                int loginResult = showLoginScreen(users, userCount);
                if (loginResult >= 0) {
                    if (strcmp(users[loginResult].username, "admin") == 0) {
                        // Admin user
                        while (true) {
                            int adminResult = showAdminMenuScreen(users, &userCount);
                            if (adminResult == 1) break; // Logout
                        }
                    } else {
                        // Regular user
                        while (true) {
                            int userResult = showUserMenuScreen(users, userCount, loginResult, users[loginResult].username);
                            if (userResult == 1) break; // Exit
                        }
                    }
                }
                break;
            }
            case 'A':
            case 'a': {
                // Direct admin menu access for testing
                while (true) {
                    int adminResult = showAdminMenuScreen(users, &userCount);
                    if (adminResult == 1) break; // Logout
                }
                break;
            }
            case 'Q':
            case 'q':
                endwin();
                return 0;
        }
    }
    
    endwin();
    return 0;
} 