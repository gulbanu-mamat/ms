#include "ui.h"
#include "library.h"
#include <ncurses.h>
#include <string.h>

// drawBorder、printTitle、printCentered、navigateMenu、showMainMenu、showStartupScreen的实现
void drawBorder(WINDOW* win) {
    box(win, 0, 0);
}

void printTitle(WINDOW* win, const char* title, int width) {
    int titleLen = strlen(title);
    mvwprintw(win, 1, (width - titleLen) / 2, "%s", title);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
}

void printCentered(WINDOW* win, int y, const char* msg, int width) {
    mvwprintw(win, y, (width - strlen(msg)) / 2, "%s", msg);
}

void showMainMenu() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();

    // Define color pairs
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // Selected item
    init_pair(2, COLOR_WHITE, COLOR_BLUE);  // Title

    int running = 1;
    while (running) {
        clear();

        attron(COLOR_PAIR(2));
        mvprintw(1, 30, "LOGISTICS MANAGEMENT SYSTEM");
        attroff(COLOR_PAIR(2));

        char* main_menu[] = {
            "1. Product Management",
            "2. Vehicle Management",
            "3. Warehouse Management",
            "4. Order Management",
            "5. Exit System"
        };

        mvprintw(10, 10, "Use UP/DOWN arrows to navigate and ENTER to select");

        // Display and handle main menu
        int choice = navigateMenu(stdscr, main_menu, 5, 3, 10);

        switch (choice) {
            case 0: { // Product Management
                clear();
                char* sub_menu[] = {
                    "1. Add Product",
                    "2. View Products",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addGoods();
                        break;
                    case 1:
                        viewGoods();
                        break;
                    case 2:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 1: { // Vehicle Management
                clear();
                char* sub_menu[] = {
                    "1. Add Vehicle",
                    "2. View Vehicles",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addVehicle();
                        break;
                    case 1:
                        viewVehicles();
                        break;
                    case 2:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 2: { // Warehouse Management
                clear();
                char* sub_menu[] = {
                    "1. Add Warehouse",
                    "2. View Warehouses",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addWarehouse();
                        break;
                    case 1:
                        viewWarehouses();
                        break;
                    case 2:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 3: { // Order Management
                clear();
                char* sub_menu[] = {
                    "1. Create Order",
                    "2. View Orders",
                    "3. Process Orders",
                    "4. Complete Orders",
                    "5. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 5, 3, 10);

                switch (subChoice) {
                    case 0:
                        createOrder();
                        break;
                    case 1:
                        viewOrders();
                        break;
                    case 2:
                        processOrders();
                        break;
                    case 3:
                        completeOrders();
                        break;
                    case 4:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 4: // Exit System
                running = 0;
                break;
        }
    }

    endwin();
}

void showStartupScreen() {
    // 初始化ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0); // 隐藏光标
    start_color(); // 启用颜色

    // 定义颜色对
    init_pair(1, COLOR_WHITE, COLOR_BLUE);    // 标题
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // 进度条
    init_pair(3, COLOR_WHITE, COLOR_BLACK);   // 文本
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // 加载框

    // 获取窗口尺寸
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // 显示标题
    attron(COLOR_PAIR(1));
    const char* title = "LOGISTICS MANAGEMENT SYSTEM";
    mvprintw(rows / 2 - 5, (cols - strlen(title)) / 2, "%s", title);
    attroff(COLOR_PAIR(1));

    // 绘制加载框
    int box_width = 60;
    int box_height = 10;
    int box_y = rows / 2 - box_height / 2;
    int box_x = (cols - box_width) / 2;

    attron(COLOR_PAIR(4));
    // 绘制边框
    mvprintw(box_y, box_x, "+");
    mvprintw(box_y, box_x + box_width, "+");
    mvprintw(box_y + box_height, box_x, "+");
    mvprintw(box_y + box_height, box_x + box_width, "+");

    // 绘制横线
    for (int i = 1; i < box_width; i++) {
        mvprintw(box_y, box_x + i, "-");
        mvprintw(box_y + box_height, box_x + i, "-");
    }

    // 绘制竖线
    for (int i = 1; i < box_height; i++) {
        mvprintw(box_y + i, box_x, "|");
        mvprintw(box_y + i, box_x + box_width, "|");
    }

    // 绘制标题
    const char* loading_text = "LOADING...";
    mvprintw(box_y + 2, (cols - strlen(loading_text)) / 2, "%s", loading_text);

    // 绘制进度条区域
    mvprintw(box_y + 4, box_x + 2, "[");
    mvprintw(box_y + 4, box_x + box_width - 2, "]");
    attroff(COLOR_PAIR(4));

    // 进度条参数
    int bar_width = box_width - 6;
    int bar_y = box_y + 4;
    int bar_x = box_x + 3;

    // 显示初始进度条（空）
    attron(COLOR_PAIR(3));
    for (int i = 0; i < bar_width; i++) {
        mvprintw(bar_y, bar_x + i, " ");
    }
    attroff(COLOR_PAIR(3));
    refresh();

    // 模拟加载过程
    for (int i = 0; i <= bar_width; i++) {
        // 更新进度条
        attron(COLOR_PAIR(2));
        for (int j = 0; j < i; j++) {
            mvprintw(bar_y, bar_x + j, "=");
        }
        attroff(COLOR_PAIR(2));

        // 显示百分比
        int percent = (i * 100) / bar_width;
        attron(COLOR_PAIR(3));
        mvprintw(box_y + 6, (cols - 4) / 2, "%3d%%", percent);
        attroff(COLOR_PAIR(3));

        refresh();
        napms(50); // 延迟50毫秒
    }

    // 显示完成消息
    const char* ready_text = "SYSTEM READY!";
    attron(COLOR_PAIR(2));
    mvprintw(box_y + 8, (cols - strlen(ready_text)) / 2, "%s", ready_text);
    attroff(COLOR_PAIR(2));
    refresh();

    // 等待用户按键
    napms(1000); // 等待1秒
    getch();

    // 清屏准备进入主程序
    clear();
    refresh();
}

int navigateMenu(WINDOW* win, char* choices[], int n_choices, int startY, int startX) {

    int highlight = 0;
    int choice = 0;
    int c;

    // Initialize item attributes
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // Display menu
    while(1) {
        for(int i = 0; i < n_choices; i++) {
            if(i == highlight)
                wattron(win, COLOR_PAIR(1));
            mvwprintw(win, startY + i, startX, "%s", choices[i]);
            if(i == highlight)
                wattroff(win, COLOR_PAIR(1));
        }

        wrefresh(win);
        c = wgetch(win);

        switch(c) {
            case KEY_UP:
                if(highlight > 0)
                    highlight--;
                break;
            case KEY_DOWN:
                if(highlight < n_choices - 1)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                return choice;
            default:
                break;
        }
    }
}


