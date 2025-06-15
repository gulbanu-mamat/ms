// C++标准库头文件
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cctype>
#include <ncurses.h>

// 使用标准命名空间
using namespace std;

// Define maximum length constants
// 定义各种数据的最大长度常量
#define MAX_NAME_LEN 50      // 名称的最大长度
#define MAX_TYPE_LEN 30      // 类型的最大长度
#define MAX_ADDR_LEN 100     // 地址的最大长度
#define MAX_PHONE_LEN 20     // 电话号码的最大长度
#define MAX_TIME_LEN 30      // 时间的最大长度
#define MAX_PLATE_LEN 15     // 车牌号的最大长度
#define MAX_LOC_LEN 50       // 位置的最大长度
#define MAX_ITEMS 50         // 订单中商品的最大数量
#define MAX_GOODS 100        // 商品的最大数量
#define MAX_VEHICLES 50      // 车辆的最大数量
#define MAX_WAREHOUSES 20    // 仓库的最大数量
#define MAX_ORDERS 100       // 订单的最大数量

#include "library.h"  // 包含自定义的库头文件
#include "sqlite_db.h" // 包含SQLite数据库头文件
#include "auth_system.h" // 包含认证系统头文件

// 全局数据库连接
SQLiteDB g_database;

// 全局当前用户变量
string currentUser = "";

// Function to set user-specific data paths
void setUserDataPaths(const char* username) {
    currentUser = username;
    char userPath[200];
    sprintf(userPath, "users/%s", username);
    // Create user directory if it doesn't exist
    createUserDirectory(username);
}

// 主函数，程序的入口点
int main() {
    User users[MAX_USERS];
    int userCount = 0;
    int currentUserIndex = -1;
    
    // Initialize ncurses for authentication
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Setup colors for authentication system
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
            
            if (isAdmin) {
                // Admin menu loop
                while (true) {
                    int menuResult = showAdminMenuScreen(users, &userCount);
                    
                    if (menuResult == 1) {
                        // Logout - break to login screen
                        break;
                    }
                    // For menuResult == 0, continue showing admin menu
                }
            } else {
                // For regular users, directly enter warehouse system
                endwin(); // End ncurses mode for main system
                
                cout << "Initializing Warehouse Management System for user: " << username << endl;
                
                // 初始化SQLite数据库
                char dbPath[200];
                sprintf(dbPath, "users/%s/warehouse_system.db", username);
                if (sqlite_db_init(&g_database, dbPath) == 0) {
                    cout << "SQLite database initialized successfully" << endl;
                    sqlite_create_tables(&g_database);
                } else {
                    cout << "SQLite database initialization failed, using file storage only" << endl;
                }
                
                loadGoods();       // 从文件中加载商品信息
                loadVehicles();    // 从文件中加载车辆信息
                loadWarehouses();  // 从文件中加载仓库信息
                loadOrders();      // 从文件中加载订单信息
                showStartupScreen(); // 显示启动屏幕
                showMainMenu();      // 显示主菜单
                
                // 程序退出前再次保存数据
                saveGoods();       // 将商品信息保存到文件
                saveVehicles();    // 将车辆信息保存到文件
                saveWarehouses();  // 将仓库信息保存到文件
                saveOrders();      // 将订单信息保存到文件
                
                // 关闭SQLite数据库连接
                sqlite_db_close(&g_database);
                
                // After exiting warehouse system, show user menu
                initscr();
                cbreak();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                setupColors();
                
                // User menu loop (for password change and logout)
                while (true) {
                    int menuResult = showUserMenuScreen(users, userCount, currentUserIndex, username);
                    
                    if (menuResult == 1) {
                        // Exit system - break to login screen
                        break;
                    }
                    // For menuResult == 0 (password change), continue showing user menu
                }
            }
        }
    }
    
    // Cleanup and exit
    endwin();
    return 0;          // 返回程序退出状态码0
}
