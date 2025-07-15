// Integrated Main Program with Authentication System
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cctype>
#include <ncurses.h>

// Using standard namespace
using namespace std;

// Include existing headers
#include "library.h"  
#include "sqlite_db.h" 
#include "auth_system.h"

// Global database connection
SQLiteDB g_database;

// External current user variable
extern string currentUser;

// Function to initialize user-specific data paths
void setUserDataPaths(const char* username) {
    // This function could modify global variables or pass user-specific paths
    // to your existing functions that handle data files
    
    // Example: Set environment variable or global path variable
    char userPath[200];
    sprintf(userPath, "users/%s", username);
    
    // You could modify your file I/O functions to use user-specific paths
    // For now, we'll just print the user directory
    printf("User data directory: %s\n", userPath);
}

// Main function with integrated authentication
int main() {
    User users[MAX_USERS];
    int userCount = 0;
    int currentUserIndex = -1;
    
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Setup colors
    setupColors();
    
    // Load users (create default admin if no users exist)
    if (!loadUsers(users, &userCount)) {
        // Create default admin user
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "Admin123456");
        strcpy(users[0].status, "normal");
        userCount = 1;
        saveUsers(users, userCount);
    }
    
    // Show welcome screen
    showWelcomeScreen();
    
    // Main authentication loop
    while (true) {
        // Show login screen
        currentUserIndex = showLoginScreen(users, userCount);
        
        if (currentUserIndex >= 0) {
            // Successful login
            const char* username = users[currentUserIndex].username;
            bool isAdmin = (strcmp(username, "admin") == 0);
            
            // Set user-specific data paths
            setUserDataPaths(username);
            
            // User menu loop
            while (true) {
                int menuResult;
                
                if (isAdmin) {
                    menuResult = showAdminMenuScreen(users, &userCount);
                } else {
                    menuResult = showUserMenuScreen(users, userCount, currentUserIndex, username);
                }
                
                if (menuResult == 1) {
                    // Logout
                    break;
                } else if (menuResult == 2) {
                    // Enter main warehouse system
                    endwin(); // End ncurses mode temporarily
                    
                    // Initialize your existing system components
                    cout << "Initializing Warehouse Management System for user: " << username << endl;
                    
                    // Initialize SQLite database
                    if (sqlite_db_init(&g_database, "warehouse_system.db") == 0) {
                        cout << "SQLite database initialized successfully" << endl;
                        sqlite_create_tables(&g_database);
                    } else {
                        cout << "SQLite database initialization failed, using file storage only" << endl;
                    }
                    
                    // Load data from files
                    loadGoods();       
                    loadVehicles();    
                    loadWarehouses();  
                    loadOrders();      
                    
                    // Show startup screen
                    showStartupScreen(); 
                    
                    // Enter main menu of your existing system
                    showMainMenu();      
                    
                    // Save data when returning from main system
                    saveGoods();       
                    saveVehicles();    
                    saveWarehouses();  
                    saveOrders();      
                    
                    // Close SQLite database connection
                    sqlite_db_close(&g_database);
                    
                    // Reinitialize ncurses for auth system
                    initscr();
                    cbreak();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    setupColors();
                    
                    // Continue in user menu
                    continue;
                }
                // For menuResult == 0, continue showing the same menu
            }
        }
    }
    
    // Cleanup and exit
    endwin();
    return 0;
} 