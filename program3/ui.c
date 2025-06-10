#include "ui.h"
#include "library.h"
#include <ncurses.h>
#include <string.h>

// 绘制窗口边框
void drawBorder(WINDOW* win) {
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
}

// 在窗口顶部打印标题
void printTitle(WINDOW* win, const char* title, int width) {
    int titleLen = strlen(title);
    wattron(win, A_BOLD | COLOR_PAIR(5));
    mvwprintw(win, 1, (width - titleLen) / 2, "%s", title);
    wattroff(win, A_BOLD | COLOR_PAIR(5));
    wattron(win, COLOR_PAIR(6));
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    wattroff(win, COLOR_PAIR(6));
}

// 在窗口中居中打印信息
void printCentered(WINDOW* win, int y, const char* msg, int width) {
    wattron(win, COLOR_PAIR(3));
    mvwprintw(win, y, (width - strlen(msg)) / 2, "%s", msg);
    wattroff(win, COLOR_PAIR(3));
}

// 显示主菜单
void showMainMenu() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();

    // 初始化颜色对
    init_pair(1, COLOR_BLACK, COLOR_WHITE);    // 选中项颜色
    init_pair(2, COLOR_WHITE, COLOR_BLUE);     // 标题颜色
    init_pair(3, COLOR_CYAN, COLOR_BLACK);     // 普通文本颜色
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // 高亮文本颜色
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);  // 标题颜色
    init_pair(6, COLOR_GREEN, COLOR_BLACK);    // 分隔线颜色
    init_pair(7, COLOR_RED, COLOR_BLACK);      // 警告文本颜色

    // 计算左右分区宽度
    int menu_width = COLS / 3;
    int preview_width = COLS * 2 / 3;

    WINDOW *menu_win = newwin(LINES, menu_width, 0, 0);
    WINDOW *preview_win = newwin(LINES, preview_width, 0, menu_width);

    keypad(menu_win, TRUE);
    keypad(preview_win, TRUE);

    // 预览文本内容
    const char *previews[] = {
        "Manage products in the system:\n- Add new products\n- View existing products\n- Edit product details\n- Remove products",
        "Manage transportation vehicles:\n- Add new vehicles\n- View vehicle list\n- Assign vehicles to routes\n- Track vehicle status",
        "Manage storage facilities:\n- Add new warehouses\n- View warehouse inventory\n- Manage stock levels\n- Track storage capacity",
        "Handle customer orders:\n- Create new orders\n- View order history\n- Process pending orders\n- Complete deliveries",
        "Exit the system and save all data"
    };

    int running = 1;
    int highlight = 0;

    while (running) {
        // 绘制菜单窗口
        wclear(menu_win);
        drawBorder(menu_win);
        printTitle(menu_win, "MAIN MENU", menu_width);

        char* main_menu[] = {
            "1. Product Management",
            "2. Vehicle Management",
            "3. Warehouse Management",
            "4. Order Management",
            "5. Exit System"
        };

        // 显示菜单项
        for(int i = 0; i < 5; i++) {
            if(i == highlight) {
                wattron(menu_win, COLOR_PAIR(1));
                mvwprintw(menu_win, 4 + i, 2, "> %s", main_menu[i]);
                wattroff(menu_win, COLOR_PAIR(1));
            } else {
                wattron(menu_win, COLOR_PAIR(3));
                mvwprintw(menu_win, 4 + i, 2, "  %s", main_menu[i]);
                wattroff(menu_win, COLOR_PAIR(3));
            }
        }

        // 绘制预览窗口
        wclear(preview_win);
        drawBorder(preview_win);
        printTitle(preview_win, "PREVIEW", preview_width);

        // 显示预览内容
        wattron(preview_win, COLOR_PAIR(5) | A_BOLD);
        mvwprintw(preview_win, 4, 2, "Selected Option: %s", main_menu[highlight]);
        wattroff(preview_win, COLOR_PAIR(5) | A_BOLD);

        wattron(preview_win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(preview_win, 6, 2, "Description:");
        wattroff(preview_win, COLOR_PAIR(4) | A_BOLD);

        // 分行显示预览文本
        char *preview_text = strdup(previews[highlight]);
        char *line = strtok(preview_text, "\n");
        int line_num = 8;

        while(line != NULL) {
            wattron(preview_win, COLOR_PAIR(3));
            mvwprintw(preview_win, line_num++, 4, "• %s", line);
            wattroff(preview_win, COLOR_PAIR(3));
            line = strtok(NULL, "\n");
        }
        free(preview_text);

        // 底部提示
        wattron(preview_win, COLOR_PAIR(6));
        mvwprintw(preview_win, LINES - 4, 2, "Navigate: ↑/↓ arrows");
        mvwprintw(preview_win, LINES - 3, 2, "Select: ENTER key");
        mvwprintw(preview_win, LINES - 2, 2, "Exit: F10 key");
        wattroff(preview_win, COLOR_PAIR(6));

        wrefresh(menu_win);
        wrefresh(preview_win);

        // 处理用户输入
        int c = wgetch(menu_win);
        switch(c) {
            case KEY_UP:
                if(highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if(highlight < 4) highlight++;
                break;
            case 10: // Enter
                // 处理菜单项选择
                switch (highlight) {
                    case 0: { // 产品管理
                        clear();
                        char* sub_menu[] = {
                            "1. Add Product",
                            "2. View Products",
                            "3. Search Products",
                            "4. Edit Product",
                            "5. Back to Main Menu"
                        };

                        // 预览子菜单项
                        const char *sub_previews[] = {
                            "Add a new product to the system",
                            "View all available products",
                            "Search for specific products",
                            "Edit existing product details",
                            "Return to main menu"
                        };

                        int sub_highlight = 0;
                        int sub_choice = -1;

                        while(sub_choice == -1) {
                            // 显示子菜单
                            clear();
                            attron(COLOR_PAIR(2));
                            mvprintw(1, 30, "PRODUCT MANAGEMENT");
                            attroff(COLOR_PAIR(2));

                            for(int i = 0; i < 5; i++) {
                                if(i == sub_highlight) {
                                    attron(COLOR_PAIR(1));
                                    mvprintw(10 + i, 10, "%s", sub_menu[i]);
                                    attroff(COLOR_PAIR(1));
                                } else {
                                    mvprintw(10 + i, 10, "%s", sub_menu[i]);
                                }
                            }

                            // 预览子菜单项
                            attron(COLOR_PAIR(4) | A_BOLD);
                            mvprintw(5, COLS/2 + 10, "Preview:");
                            attroff(COLOR_PAIR(4) | A_BOLD);

                            attron(COLOR_PAIR(3));
                            mvprintw(7, COLS/2 + 10, "%s", sub_previews[sub_highlight]);
                            attroff(COLOR_PAIR(3));

                            refresh();

                            // 处理子菜单输入
                            int ch = getch();
                            switch(ch) {
                                case KEY_UP:
                                    if(sub_highlight > 0) sub_highlight--;
                                    break;
                                case KEY_DOWN:
                                    if(sub_highlight < 4) sub_highlight++;
                                    break;
                                case 10: // Enter
                                    sub_choice = sub_highlight;
                                    break;
                            }
                        }

                        switch (sub_choice) {
                            case 0:
                                addGoods();
                                break;
                            case 1:
                                viewGoods();
                                break;
                            case 4:
                                // 返回主菜单
                                break;
                        }
                        break;
                    }
                    case 1: { // 车辆管理
                        clear();
                        char* sub_menu[] = {
                            "1. Add Vehicle",
                            "2. View Vehicles",
                            "3. Search Vehicles",
                            "4. Assign Vehicle",
                            "5. Back to Main Menu"
                        };
                        int subChoice = navigateMenu(stdscr, sub_menu, 5, 3, 10);

                        switch (subChoice) {
                            case 0:
                                addVehicle();
                                break;
                            case 1:
                                viewVehicles();
                                break;
                            case 4:
                                // 返回主菜单
                                break;
                        }
                        break;
                    }
                    case 2: { // 仓库管理
                        clear();
                        char* sub_menu[] = {
                            "1. Add Warehouse",
                            "2. View Warehouses",
                            "3. Manage Inventory",
                            "4. Check Capacity",
                            "5. Back to Main Menu"
                        };
                        int subChoice = navigateMenu(stdscr, sub_menu, 5, 3, 10);

                        switch (subChoice) {
                            case 0:
                                addWarehouse();
                                break;
                            case 1:
                                viewWarehouses();
                                break;
                            case 4:
                                // 返回主菜单
                                break;
                        }
                        break;
                    }
                    case 3: { // 订单管理
                        clear();
                        char* sub_menu[] = {
                            "1. Create Order",
                            "2. View Orders",
                            "3. Process Orders",
                            "4. Complete Orders",
                            "5. Order History",
                            "6. Back to Main Menu"
                        };
                        int subChoice = navigateMenu(stdscr, sub_menu, 6, 3, 10);

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
                            case 5:
                                // 返回主菜单
                                break;
                        }
                        break;
                    }
                    case 4: // 退出系统
                        running = 0;
                        break;
                }
                break;
            case KEY_F(10): // F10退出
                running = 0;
                break;
        }
    }

    delwin(menu_win);
    delwin(preview_win);
    endwin();
}

// 显示启动屏幕
void showStartupScreen() {
    // 初始化ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0); // 隐藏光标
    start_color(); // 启用颜色模式

    // 初始化颜色对
    init_pair(1, COLOR_WHITE, COLOR_BLUE);     // 背景
    init_pair(2, COLOR_GREEN, COLOR_BLACK);    // 高亮
    init_pair(3, COLOR_WHITE, COLOR_BLACK);    // 普通文本
    init_pair(4, COLOR_CYAN, COLOR_BLACK);     // 边框
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);   // 系统标题
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);  // 特殊效果
    init_pair(7, COLOR_RED, COLOR_BLACK);      // 警告文本

    // 获取屏幕尺寸
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // 显示标题动画
    const char* title = "LOGISTICS MANAGEMENT SYSTEM";
    int title_len = strlen(title);
    int title_x = (cols - title_len) / 2;
    int title_y = rows / 2 - 8;

    // 标题淡入效果
    for(int i = 0; i < title_len; i++) {
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(title_y, title_x + i, "%c", title[i]);
        attroff(COLOR_PAIR(1) | A_BOLD);
        refresh();
        napms(50);
    }

    // 显示版本信息
    attron(COLOR_PAIR(3) | A_BOLD);
    const char* version = "Version 2.0";
    mvprintw(rows / 2 - 6, (cols - strlen(version)) / 2, "%s", version);
    attroff(COLOR_PAIR(3) | A_BOLD);

    // 绘制边框
    int box_width = 60;
    int box_height = 10;
    int box_y = rows / 2 - box_height / 2;
    int box_x = (cols - box_width) / 2;

    // 边框动画效果
    attron(COLOR_PAIR(4));
    // 绘制四个角
    mvprintw(box_y, box_x, "+");
    mvprintw(box_y, box_x + box_width, "+");
    mvprintw(box_y + box_height, box_x, "+");
    mvprintw(box_y + box_height, box_x + box_width, "+");
    refresh();
    napms(100);

    // 绘制上下边框
    for (int i = 1; i < box_width; i++) {
        mvprintw(box_y, box_x + i, "-");
        mvprintw(box_y + box_height, box_x + i, "-");
        refresh();
        napms(10);
    }

    // 绘制左右边框
    for (int i = 1; i < box_height; i++) {
        mvprintw(box_y + i, box_x, "|");
        mvprintw(box_y + i, box_x + box_width, "|");
        refresh();
        napms(20);
    }
    attroff(COLOR_PAIR(4));

    // 绘制进度条标题
    const char* loading_text = "INITIALIZING SYSTEM...";
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(box_y + 2, (cols - strlen(loading_text)) / 2, "%s", loading_text);
    attroff(COLOR_PAIR(5) | A_BOLD);

    // 绘制进度条边框
    attron(COLOR_PAIR(4));
    mvprintw(box_y + 4, box_x + 2, "[");
    mvprintw(box_y + 4, box_x + box_width - 2, "]");
    attroff(COLOR_PAIR(4));

    // 绘制进度条
    int bar_width = box_width - 6;
    int bar_y = box_y + 4;
    int bar_x = box_x + 3;

    // 绘制进度条初始状态
    attron(COLOR_PAIR(3));
    for (int i = 0; i < bar_width; i++) {
        mvprintw(bar_y, bar_x + i, " ");
    }
    attroff(COLOR_PAIR(3));
    refresh();

    // 模拟进度条动画
    for (int i = 0; i <= bar_width; i++) {
        // 绘制进度条
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
        napms(30); // 延迟30毫秒
    }

    // 显示系统准备就绪信息
    const char* ready_text = "SYSTEM READY! PRESS ANY KEY";
    attron(COLOR_PAIR(6) | A_BOLD | A_BLINK);
    mvprintw(box_y + 8, (cols - strlen(ready_text)) / 2, "%s", ready_text);
    attroff(COLOR_PAIR(6) | A_BOLD | A_BLINK);
    refresh();

    // 等待用户输入
    getch();

    // 清除屏幕
    clear();
    refresh();
}

// 处理菜单输入
int navigateMenu(WINDOW* win, char* choices[], int n_choices, int startY, int startX) {
    int highlight = 0;  // 当前选中的选项
    int choice = -1;    // 用户选择的选项
    int c;              // 存储用户输入的字符

    // 初始化颜色
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // 显示菜单项
    while(choice == -1) {
        for(int i = 0; i < n_choices; i++) {
            if(i == highlight)
                wattron(win, COLOR_PAIR(1));  // 选中选项颜色
            mvwprintw(win, startY + i, startX, "%s", choices[i]);  // 打印菜单项
            if(i == highlight)
                wattroff(win, COLOR_PAIR(1));  // 恢复默认颜色
        }

        wrefresh(win);  // 刷新屏幕
        c = wgetch(win);  // 获取用户输入

        switch(c) {
            case KEY_UP:
                if(highlight > 0)
                    highlight--;  // 向上移动选中选项
                break;
            case KEY_DOWN:
                if(highlight < n_choices - 1)
                    highlight++;  // 向下移动选中选项
                break;
            case 10: // Enter
                choice = highlight;  // 用户Enter键选择选中选项
                break;
            case KEY_F(10): // F10退出
                choice = n_choices - 1; // 选择最后一项（通常是退出）
                break;
        }
    }

    return choice;
}
