#include "auth_system.h"
#include <sys/stat.h>
#include <sys/types.h>

// External declaration of current user (defined in main.cpp)
extern string currentUser;

// Load user data from file
int loadUsers(User users[], int *userCount) {
    ifstream file(USERS_FILE);
    if (!file.is_open()) {
        *userCount = 0;
        return 0;
    }
    
    *userCount = 0;
    string line;
    
    while (getline(file, line) && *userCount < MAX_USERS) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string username, password, status;
        
        if (getline(ss, username, ';') && 
            getline(ss, password, ';') && 
            getline(ss, status)) {
            
            strcpy(users[*userCount].username, username.c_str());
            strcpy(users[*userCount].password, password.c_str());
            strcpy(users[*userCount].status, status.c_str());
            (*userCount)++;
        }
    }
    
    file.close();
    return 1;
}

// Save user data to file
int saveUsers(User users[], int userCount) {
    ofstream file(USERS_FILE);
    if (!file.is_open()) {
        return 0;
    }
    
    for (int i = 0; i < userCount; i++) {
        file << users[i].username << ";" << users[i].password << ";" << users[i].status << endl;
    }
    
    file.close();
    return 1;
}

// Authenticate user credentials
int authenticateUser(const char *username, const char *password, User users[], int userCount, int *userIndex) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            *userIndex = i;
            return 1;
        }
    }
    return 0;
}

// Validate password format
bool isValidPassword(const char *password) {
    int len = strlen(password);
    if (len < 8 || len > 15) {
        return false;
    }
    
    bool hasLetter = false, hasDigit = false;
    for (int i = 0; i < len; i++) {
        if (isalpha(password[i])) {
            hasLetter = true;
        }
        if (isdigit(password[i])) {
            hasDigit = true;
        }
    }
    
    return hasLetter && hasDigit;
}

// Generate random password
void generateRandomPassword(char *password) {
    const char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *digits = "0123456789";
    
    srand(time(NULL));
    
    // Ensure at least one letter and one digit
    password[0] = letters[rand() % strlen(letters)];
    password[1] = digits[rand() % strlen(digits)];
    
    // Generate remaining characters
    for (int i = 2; i < 10; i++) {
        if (rand() % 2) {
            password[i] = letters[rand() % strlen(letters)];
        } else {
            password[i] = digits[rand() % strlen(digits)];
        }
    }
    
    password[10] = '\0';
}

// Create user directory
void createUserDirectory(const char *username) {
    char dirPath[100];
    sprintf(dirPath, "users/%s", username);
    
#ifdef _WIN32
    system("mkdir users 2>nul");
    char mkdirCmd[150];
    sprintf(mkdirCmd, "mkdir \"%s\" 2>nul", dirPath);
    system(mkdirCmd);
#else
    system("mkdir -p users");
    char mkdirCmd[150];
    sprintf(mkdirCmd, "mkdir -p \"%s\"", dirPath);
    system(mkdirCmd);
#endif
    
    // Create user-specific data files
    const char* files[] = {"orders.dat", "vehicles.dat", "warehouses.dat", "goods.dat"};
    for (int i = 0; i < 4; i++) {
        char filePath[150];
        sprintf(filePath, "%s/%s", dirPath, files[i]);
        FILE *file = fopen(filePath, "a");
        if (file) fclose(file);
    }
}

// Setup color pairs for ncurses
void setupColors() {
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);    // Selected item
    init_pair(2, COLOR_WHITE, COLOR_BLUE);     // Title
    init_pair(3, COLOR_CYAN, COLOR_BLACK);     // Normal text
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // Highlight text
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);  // Title text
    init_pair(6, COLOR_GREEN, COLOR_BLACK);    // Success text
    init_pair(7, COLOR_RED, COLOR_BLACK);      // Error text
}

// Draw frame around window
void drawFrame(WINDOW* win, const char* title) {
    int height, width;
    getmaxyx(win, height, width);
    
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    
    if (title) {
        int titleLen = strlen(title);
        wattron(win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(win, 1, (width - titleLen) / 2, "%s", title);
        wattroff(win, A_BOLD | COLOR_PAIR(2));
        
        wattron(win, COLOR_PAIR(6));
        mvwhline(win, 2, 1, ACS_HLINE, width - 2);
        wattroff(win, COLOR_PAIR(6));
    }
}

// Get secure input (with optional password masking)
int getSecureInput(char* buffer, int maxLen, bool isPassword) {
    int pos = 0;
    int ch;
    
    while ((ch = getch()) != '\n' && ch != '\r' && ch != KEY_ENTER) {
        if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (pos > 0) {
                pos--;
                addch('\b');
                addch(' ');
                addch('\b');
                refresh();
            }
        } else if (ch >= 32 && ch <= 126 && pos < maxLen - 1) {
            buffer[pos++] = ch;
            if (isPassword) {
                addch('*');
            } else {
                addch(ch);
            }
            refresh();
        }
    }
    
    buffer[pos] = '\0';
    return pos;
}

// Show welcome screen
void showWelcomeScreen() {
    // Skip welcome animation, go directly to login
    return;
}

// Show login screen
int showLoginScreen(User users[], int userCount) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int userIndex;
    
    while (true) {
        clear();
        
        // Create login window
        int height = 15, width = 60;
        int startY = (LINES - height) / 2;
        int startX = (COLS - width) / 2;
        
        WINDOW *loginWin = newwin(height, width, startY, startX);
        drawFrame(loginWin, "USER LOGIN");
        
        // Display login form
        wattron(loginWin, COLOR_PAIR(3));
        mvwprintw(loginWin, 5, 5, "Username: ");
        mvwprintw(loginWin, 7, 5, "Password: ");
        wattroff(loginWin, COLOR_PAIR(3));
        
        wattron(loginWin, COLOR_PAIR(4));
        mvwprintw(loginWin, 10, 5, "Default admin: admin / Admin123456");
        wattroff(loginWin, COLOR_PAIR(4));
        
        wrefresh(loginWin);
        
        // Get username
        wmove(loginWin, 5, 15);
        echo();
        curs_set(1);
        wgetnstr(loginWin, username, MAX_USERNAME_LEN - 1);
        
        // Get password
        wmove(loginWin, 7, 15);
        noecho();
        int pos = 0;
        int ch;
        while ((ch = wgetch(loginWin)) != '\n' && ch != '\r' && ch != KEY_ENTER) {
            if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (pos > 0) {
                    pos--;
                    wmove(loginWin, 7, 15 + pos);
                    waddch(loginWin, ' ');
                    wmove(loginWin, 7, 15 + pos);
                }
            } else if (ch >= 32 && ch <= 126 && pos < MAX_PASSWORD_LEN - 1) {
                password[pos++] = ch;
                waddch(loginWin, '*');
            }
            wrefresh(loginWin);
        }
        password[pos] = '\0';
        curs_set(0);
        
        // Authenticate user
        if (authenticateUser(username, password, users, userCount, &userIndex)) {
            currentUser = username;
            
            // Check if password change is required
            if (strcmp(users[userIndex].status, "initial") == 0) {
                wclear(loginWin);
                drawFrame(loginWin, "PASSWORD CHANGE REQUIRED");
                
                wattron(loginWin, COLOR_PAIR(7) | A_BOLD);
                mvwprintw(loginWin, 5, 5, "You must change your initial password!");
                wattroff(loginWin, COLOR_PAIR(7) | A_BOLD);
                
                wattron(loginWin, COLOR_PAIR(4));
                mvwprintw(loginWin, 7, 5, "Press any key to continue...");
                wattroff(loginWin, COLOR_PAIR(4));
                
                wrefresh(loginWin);
                wgetch(loginWin);
                
                if (showPasswordChangeScreen(users, userCount, userIndex) == 0) {
                    delwin(loginWin);
                    continue; // Return to login after password change
                }
            }
            
            // Create user directory
            createUserDirectory(username);
            
            delwin(loginWin);
            return userIndex; // Successful login
        } else {
            // Display error message
            wclear(loginWin);
            drawFrame(loginWin, "LOGIN ERROR");
            
            wattron(loginWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(loginWin, 6, 5, "Invalid username or password!");
            wattroff(loginWin, COLOR_PAIR(7) | A_BOLD);
            
            wattron(loginWin, COLOR_PAIR(4));
            mvwprintw(loginWin, 8, 5, "Press any key to try again...");
            wattroff(loginWin, COLOR_PAIR(4));
            
            wrefresh(loginWin);
            wgetch(loginWin);
        }
        
        delwin(loginWin);
    }
}

// Show password change screen
int showPasswordChangeScreen(User users[], int userCount, int userIndex) {
    char oldPassword[MAX_PASSWORD_LEN];
    char newPassword[MAX_PASSWORD_LEN];
    char confirmPassword[MAX_PASSWORD_LEN];
    
    while (true) {
        clear();
        
        // Create password change window
        int height = 18, width = 70;
        int startY = (LINES - height) / 2;
        int startX = (COLS - width) / 2;
        
        WINDOW *passWin = newwin(height, width, startY, startX);
        drawFrame(passWin, "CHANGE PASSWORD");
        
        // Display form
        wattron(passWin, COLOR_PAIR(3));
        mvwprintw(passWin, 5, 5, "Current Password: ");
        mvwprintw(passWin, 7, 5, "New Password: ");
        mvwprintw(passWin, 9, 5, "Confirm Password: ");
        wattroff(passWin, COLOR_PAIR(3));
        
        wattron(passWin, COLOR_PAIR(4));
        mvwprintw(passWin, 12, 5, "Password Requirements:");
        mvwprintw(passWin, 13, 5, "• 8-15 characters long");
        mvwprintw(passWin, 14, 5, "• Must contain letters and numbers");
        mvwprintw(passWin, 15, 5, "• Cannot be the same as current password");
        wattroff(passWin, COLOR_PAIR(4));
        
        wrefresh(passWin);
        
        // Get current password
        wmove(passWin, 5, 23);
        noecho();
        curs_set(1);
        int pos = 0;
        int ch;
        while ((ch = wgetch(passWin)) != '\n' && ch != '\r' && ch != KEY_ENTER) {
            if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (pos > 0) {
                    pos--;
                    wmove(passWin, 5, 23 + pos);
                    waddch(passWin, ' ');
                    wmove(passWin, 5, 23 + pos);
                }
            } else if (ch >= 32 && ch <= 126 && pos < MAX_PASSWORD_LEN - 1) {
                oldPassword[pos++] = ch;
                waddch(passWin, '*');
            }
            wrefresh(passWin);
        }
        oldPassword[pos] = '\0';
        
        // Verify current password
        if (strcmp(oldPassword, users[userIndex].password) != 0) {
            wattron(passWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(passWin, 16, 5, "Current password is incorrect!");
            wattroff(passWin, COLOR_PAIR(7) | A_BOLD);
            wrefresh(passWin);
            napms(2000);
            delwin(passWin);
            continue;
        }
        
        // Get new password
        wmove(passWin, 7, 19);
        pos = 0;
        while ((ch = wgetch(passWin)) != '\n' && ch != '\r' && ch != KEY_ENTER) {
            if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (pos > 0) {
                    pos--;
                    wmove(passWin, 7, 19 + pos);
                    waddch(passWin, ' ');
                    wmove(passWin, 7, 19 + pos);
                }
            } else if (ch >= 32 && ch <= 126 && pos < MAX_PASSWORD_LEN - 1) {
                newPassword[pos++] = ch;
                waddch(passWin, '*');
            }
            wrefresh(passWin);
        }
        newPassword[pos] = '\0';
        
        // Validate new password
        if (!isValidPassword(newPassword)) {
            wattron(passWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(passWin, 16, 5, "Password doesn't meet requirements!");
            wattroff(passWin, COLOR_PAIR(7) | A_BOLD);
            wrefresh(passWin);
            napms(2000);
            delwin(passWin);
            continue;
        }
        
        if (strcmp(newPassword, users[userIndex].password) == 0) {
            wattron(passWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(passWin, 16, 5, "New password cannot be the same as current!");
            wattroff(passWin, COLOR_PAIR(7) | A_BOLD);
            wrefresh(passWin);
            napms(2000);
            delwin(passWin);
            continue;
        }
        
        // Get password confirmation
        wmove(passWin, 9, 23);
        pos = 0;
        while ((ch = wgetch(passWin)) != '\n' && ch != '\r' && ch != KEY_ENTER) {
            if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (pos > 0) {
                    pos--;
                    wmove(passWin, 9, 23 + pos);
                    waddch(passWin, ' ');
                    wmove(passWin, 9, 23 + pos);
                }
            } else if (ch >= 32 && ch <= 126 && pos < MAX_PASSWORD_LEN - 1) {
                confirmPassword[pos++] = ch;
                waddch(passWin, '*');
            }
            wrefresh(passWin);
        }
        confirmPassword[pos] = '\0';
        curs_set(0);
        
        // Check password confirmation
        if (strcmp(newPassword, confirmPassword) != 0) {
            wattron(passWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(passWin, 16, 5, "Password confirmation doesn't match!");
            wattroff(passWin, COLOR_PAIR(7) | A_BOLD);
            wrefresh(passWin);
            napms(2000);
            delwin(passWin);
            continue;
        }
        
        // Update password
        strcpy(users[userIndex].password, newPassword);
        strcpy(users[userIndex].status, "normal");
        
        if (saveUsers(users, userCount)) {
            wattron(passWin, COLOR_PAIR(6) | A_BOLD);
            mvwprintw(passWin, 16, 5, "Password changed successfully!");
            wattroff(passWin, COLOR_PAIR(6) | A_BOLD);
        } else {
            wattron(passWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(passWin, 16, 5, "Failed to save password!");
            wattroff(passWin, COLOR_PAIR(7) | A_BOLD);
        }
        
        wrefresh(passWin);
        napms(2000);
        delwin(passWin);
        return 1;
    }
}

// Register new user
void registerUser(User users[], int *userCount) {
    if (*userCount >= MAX_USERS) {
        clear();
        attron(COLOR_PAIR(7) | A_BOLD);
        mvprintw(LINES/2, (COLS - 30)/2, "Maximum number of users reached!");
        attroff(COLOR_PAIR(7) | A_BOLD);
        refresh();
        napms(2000);
        return;
    }
    
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    
    clear();
    
    // Create registration window
    int height = 15, width = 60;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;
    
    WINDOW *regWin = newwin(height, width, startY, startX);
    drawFrame(regWin, "REGISTER NEW USER");
    
    // Get username
    wattron(regWin, COLOR_PAIR(3));
    mvwprintw(regWin, 5, 5, "Username: ");
    wattroff(regWin, COLOR_PAIR(3));
    
    wmove(regWin, 5, 15);
    echo();
    curs_set(1);
    wgetnstr(regWin, username, MAX_USERNAME_LEN - 1);
    curs_set(0);
    noecho();
    
    // Check if username already exists
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            wattron(regWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(regWin, 7, 5, "Username already exists!");
            wattroff(regWin, COLOR_PAIR(7) | A_BOLD);
            wrefresh(regWin);
            napms(2000);
            delwin(regWin);
            return;
        }
    }
    
    // Generate random password
    generateRandomPassword(password);
    
    // Add new user
    strcpy(users[*userCount].username, username);
    strcpy(users[*userCount].password, password);
    strcpy(users[*userCount].status, "initial");
    (*userCount)++;
    
    // Create user directory
    createUserDirectory(username);
    
    // Save users
    if (saveUsers(users, *userCount)) {
        wattron(regWin, COLOR_PAIR(6) | A_BOLD);
        mvwprintw(regWin, 7, 5, "User registered successfully!");
        wattroff(regWin, COLOR_PAIR(6) | A_BOLD);
        
        wattron(regWin, COLOR_PAIR(4));
        mvwprintw(regWin, 9, 5, "Username: %s", username);
        mvwprintw(regWin, 10, 5, "Initial Password: %s", password);
        mvwprintw(regWin, 11, 5, "User must change password on first login");
        wattroff(regWin, COLOR_PAIR(4));
    } else {
        wattron(regWin, COLOR_PAIR(7) | A_BOLD);
        mvwprintw(regWin, 7, 5, "Failed to register user!");
        wattroff(regWin, COLOR_PAIR(7) | A_BOLD);
        (*userCount)--;
    }
    
    wrefresh(regWin);
    napms(4000);
    delwin(regWin);
}

// Reset user password
void resetUserPassword(User users[], int userCount) {
    char username[MAX_USERNAME_LEN];
    
    clear();
    
    // Create reset window
    int height = 12, width = 60;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;
    
    WINDOW *resetWin = newwin(height, width, startY, startX);
    drawFrame(resetWin, "RESET USER PASSWORD");
    
    // Get username
    wattron(resetWin, COLOR_PAIR(3));
    mvwprintw(resetWin, 5, 5, "Username to reset: ");
    wattroff(resetWin, COLOR_PAIR(3));
    
    wmove(resetWin, 5, 24);
    echo();
    curs_set(1);
    wgetnstr(resetWin, username, MAX_USERNAME_LEN - 1);
    curs_set(0);
    noecho();
    
    // Find user
    int userIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            userIndex = i;
            break;
        }
    }
    
    if (userIndex == -1) {
        wattron(resetWin, COLOR_PAIR(7) | A_BOLD);
        mvwprintw(resetWin, 7, 5, "User not found!");
        wattroff(resetWin, COLOR_PAIR(7) | A_BOLD);
    } else if (strcmp(users[userIndex].username, "admin") == 0) {
        wattron(resetWin, COLOR_PAIR(7) | A_BOLD);
        mvwprintw(resetWin, 7, 5, "Cannot reset admin password!");
        wattroff(resetWin, COLOR_PAIR(7) | A_BOLD);
    } else {
        // Generate new random password
        char newPassword[MAX_PASSWORD_LEN];
        generateRandomPassword(newPassword);
        
        strcpy(users[userIndex].password, newPassword);
        strcpy(users[userIndex].status, "initial");
        
        if (saveUsers(users, userCount)) {
            wattron(resetWin, COLOR_PAIR(6) | A_BOLD);
            mvwprintw(resetWin, 7, 5, "Password reset successfully!");
            wattroff(resetWin, COLOR_PAIR(6) | A_BOLD);
            
            wattron(resetWin, COLOR_PAIR(4));
            mvwprintw(resetWin, 8, 5, "New Password: %s", newPassword);
            mvwprintw(resetWin, 9, 5, "User must change password on next login");
            wattroff(resetWin, COLOR_PAIR(4));
        } else {
            wattron(resetWin, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(resetWin, 7, 5, "Failed to reset password!");
            wattroff(resetWin, COLOR_PAIR(7) | A_BOLD);
        }
    }
    
    wrefresh(resetWin);
    napms(3000);
    delwin(resetWin);
}

// Show admin menu screen
int showAdminMenuScreen(User users[], int *userCount) {
    const char* menuItems[] = {
        "1. Register New User",
        "2. Reset User Password",
        "3. View All Users",
        "4. Change Admin Password",
        "5. Logout"
    };
    
    int highlight = 0;
    int choice = -1;
    
    while (choice == -1) {
        clear();
        
        // Create admin menu window
        int height = 18, width = 60;
        int startY = (LINES - height) / 2;
        int startX = (COLS - width) / 2;
        
        WINDOW *adminWin = newwin(height, width, startY, startX);
        keypad(adminWin, TRUE);  // Enable keyboard input
        drawFrame(adminWin, "ADMIN PANEL");
        
        // Display menu items
        for (int i = 0; i < 5; i++) {
            if (i == highlight) {
                wattron(adminWin, COLOR_PAIR(1));
                mvwprintw(adminWin, 5 + i, 5, "> %s", menuItems[i]);
                wattroff(adminWin, COLOR_PAIR(1));
            } else {
                wattron(adminWin, COLOR_PAIR(3));
                mvwprintw(adminWin, 5 + i, 5, "  %s", menuItems[i]);
                wattroff(adminWin, COLOR_PAIR(3));
            }
        }
        
        wattron(adminWin, COLOR_PAIR(4));
        mvwprintw(adminWin, 13, 5, "Use ↑/↓ arrows to navigate, ENTER to select");
        wattroff(adminWin, COLOR_PAIR(4));
        
        wrefresh(adminWin);
        
        // Handle input
        int ch = wgetch(adminWin);
        switch (ch) {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 4) highlight++;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                choice = highlight;
                break;
        }
        
        delwin(adminWin);
    }
    
    // Handle menu selection
    switch (choice) {
        case 0: // Register New User
            registerUser(users, userCount);
            return 0;
        case 1: // Reset User Password
            resetUserPassword(users, *userCount);
            return 0;
        case 2: { // View All Users with scrolling support
            int startIndex = 0;
            int maxVisible = LINES - 8; // Leave space for header and footer
            int ch;
            
            while (true) {
                clear();
                
                // Header
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(1, 2, "USER LIST - Total: %d users", *userCount);
                attroff(COLOR_PAIR(2) | A_BOLD);
                
                // Show scroll indicator if needed
                if (*userCount > maxVisible) {
                    attron(COLOR_PAIR(4));
                    mvprintw(2, 2, "Showing users %d-%d of %d", 
                            startIndex + 1, 
                            (startIndex + maxVisible > *userCount) ? *userCount : startIndex + maxVisible,
                            *userCount);
                    attroff(COLOR_PAIR(4));
                }
                
                // Draw separator line
                attron(COLOR_PAIR(6));
                mvhline(3, 2, ACS_HLINE, COLS - 4);
                attroff(COLOR_PAIR(6));
                
                // Display users
                int endIndex = (startIndex + maxVisible > *userCount) ? *userCount : startIndex + maxVisible;
                for (int i = startIndex; i < endIndex; i++) {
                    attron(COLOR_PAIR(3));
                    mvprintw(5 + (i - startIndex), 2, "%2d. Username: %-20s Status: %-8s", 
                            i + 1, users[i].username, users[i].status);
                    attroff(COLOR_PAIR(3));
                }
                
                // Footer with instructions
                attron(COLOR_PAIR(4));
                mvprintw(LINES - 3, 2, "Navigation:");
                if (*userCount > maxVisible) {
                    mvprintw(LINES - 2, 2, "↑/↓: Scroll, ESC/Q: Back to admin menu");
                } else {
                    mvprintw(LINES - 2, 2, "ESC/Q: Back to admin menu");
                }
                attroff(COLOR_PAIR(4));
                
                refresh();
                
                // Enable keyboard input for navigation
                keypad(stdscr, TRUE);
                
                // Handle input
                ch = getch();
                switch (ch) {
                    case KEY_UP:
                        if (startIndex > 0) {
                            startIndex--;
                        }
                        break;
                    case KEY_DOWN:
                        if (startIndex + maxVisible < *userCount) {
                            startIndex++;
                        }
                        break;
                    case 'q':
                    case 'Q':
                    case 27: // ESC key
                        return 0;
                    default:
                        if (ch == '\n' || ch == '\r' || ch == ' ') {
                            return 0;
                        }
                        break;
                }
            }
        }
        case 3: // Change Admin Password
            showPasswordChangeScreen(users, *userCount, 0);
            return 0;
        case 4: // Logout
            return 1;
    }
    
    return 0;
}

// Show user menu (for regular users)
int showUserMenuScreen(User users[], int userCount, int userIndex, const char *username) {
    const char* menuItems[] = {
        "1. Change Password",
        "2. Exit System"
    };
    
    int highlight = 0;
    int choice = -1;
    
    while (choice == -1) {
        clear();
        
        // Create user menu window
        int height = 12, width = 50;
        int startY = (LINES - height) / 2;
        int startX = (COLS - width) / 2;
        
        WINDOW *userWin = newwin(height, width, startY, startX);
        keypad(userWin, TRUE);  // Enable keyboard input
        
        char titleBuffer[100];
        sprintf(titleBuffer, "USER MENU - %s", username);
        drawFrame(userWin, titleBuffer);
        
        // Display menu items
        for (int i = 0; i < 2; i++) {
            if (i == highlight) {
                wattron(userWin, COLOR_PAIR(1));
                mvwprintw(userWin, 5 + i, 5, "> %s", menuItems[i]);
                wattroff(userWin, COLOR_PAIR(1));
            } else {
                wattron(userWin, COLOR_PAIR(3));
                mvwprintw(userWin, 5 + i, 5, "  %s", menuItems[i]);
                wattroff(userWin, COLOR_PAIR(3));
            }
        }
        
        wattron(userWin, COLOR_PAIR(4));
        mvwprintw(userWin, 9, 5, "Use ↑/↓ arrows, ENTER to select");
        wattroff(userWin, COLOR_PAIR(4));
        
        wrefresh(userWin);
        
        // Handle input
        int ch = wgetch(userWin);
        switch (ch) {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 1) highlight++;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                choice = highlight;
                break;
        }
        
        delwin(userWin);
    }
    
    // Handle menu selection
    switch (choice) {
        case 0: // Change Password
            showPasswordChangeScreen(users, userCount, userIndex);
            return 0;
        case 1: // Exit System
            return 1;
    }
    
    return 0;
} 