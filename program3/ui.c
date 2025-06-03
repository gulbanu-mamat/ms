#include "ui.h"
#include "library.h"
#include <ncurses.h>
#include <string.h>

// drawBorder、printTitle、printCentered、navigateMenu、showMainMenu、showStartupScreen的实现

// 绘制窗口边框
void drawBorder(WINDOW* win) {
    box(win, 0, 0);  // 使用ncurses库的box函数绘制窗口边框
}

// 在窗口中打印标题
void printTitle(WINDOW* win, const char* title, int width) {
    int titleLen = strlen(title);  // 获取标题的长度
    // 将标题居中打印在窗口的第一行
    mvwprintw(win, 1, (width - titleLen) / 2, "%s", title);
    // 在标题下方绘制一条水平线
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
}

// 在窗口中居中打印消息
void printCentered(WINDOW* win, int y, const char* msg, int width) {
    // 将消息居中打印在指定的行
    mvwprintw(win, y, (width - strlen(msg)) / 2, "%s", msg);
}

// 显示主菜单
void showMainMenu() {
    initscr();  // 初始化ncurses库
    start_color();  // 启用颜色模式
    cbreak();  // 禁用行缓冲
    noecho();  // 禁用回显
    keypad(stdscr, TRUE);  // 启用功能键
    curs_set(0);  // 隐藏光标
    refresh();  // 刷新屏幕

    // 定义颜色对
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // 选中项的颜色
    init_pair(2, COLOR_WHITE, COLOR_BLUE);  // 标题的颜色

    int running = 1;
    while (running) {
        clear();  // 清除屏幕

        attron(COLOR_PAIR(2));  // 设置标题颜色
        mvprintw(1, 30, "LOGISTICS MANAGEMENT SYSTEM");  // 打印系统标题
        attroff(COLOR_PAIR(2));  // 恢复默认颜色

        char* main_menu[] = {
            "1. Product Management",
            "2. Vehicle Management",
            "3. Warehouse Management",
            "4. Order Management",
            "5. Exit System"
        };

        mvprintw(10, 10, "Use UP/DOWN arrows to navigate and ENTER to select");  // 显示导航提示

        // 显示并处理主菜单
        int choice = navigateMenu(stdscr, main_menu, 5, 3, 10);

        switch (choice) {
            case 0: { // 产品管理
                clear();  // 清除屏幕
                char* sub_menu[] = {
                    "1. Add Product",
                    "2. View Products",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addGoods();  // 添加商品
                        break;
                    case 1:
                        viewGoods();  // 查看商品列表
                        break;
                    case 2:
                        // 返回主菜单
                        break;
                }
                break;
            }
            case 1: { // 车辆管理
                clear();  // 清除屏幕
                char* sub_menu[] = {
                    "1. Add Vehicle",
                    "2. View Vehicles",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addVehicle();  // 添加车辆
                        break;
                    case 1:
                        viewVehicles();  // 查看车辆列表
                        break;
                    case 2:
                        // 返回主菜单
                        break;
                }
                break;
            }
            case 2: { // 仓库管理
                clear();  // 清除屏幕
                char* sub_menu[] = {
                    "1. Add Warehouse",
                    "2. View Warehouses",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addWarehouse();  // 添加仓库
                        break;
                    case 1:
                        viewWarehouses();  // 查看仓库列表
                        break;
                    case 2:
                        // 返回主菜单
                        break;
                }
                break;
            }
            case 3: { // 订单管理
                clear();  // 清除屏幕
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
                        createOrder();  // 创建订单
                        break;
                    case 1:
                        viewOrders();  // 查看订单列表
                        break;
                    case 2:
                        processOrders();  // 处理订单
                        break;
                    case 3:
                        completeOrders();  // 完成订单
                        break;
                    case 4:
                        // 返回主菜单
                        break;
                }
                break;
            }
            case 4: // 退出系统
                running = 0;
                break;
        }
    }

    endwin();  // 结束ncurses会话
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
    init_pair(1, COLOR_WHITE, COLOR_BLUE);    // 标题
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // 进度条
    init_pair(3, COLOR_WHITE, COLOR_BLACK);   // 文本
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // 边框

    // 获取屏幕尺寸
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // 显示标题
    attron(COLOR_PAIR(1));
    const char* title = "LOGISTICS MANAGEMENT SYSTEM";
    mvprintw(rows / 2 - 5, (cols - strlen(title)) / 2, "%s", title);
    attroff(COLOR_PAIR(1));

    // 绘制边框
    int box_width = 60;
    int box_height = 10;
    int box_y = rows / 2 - box_height / 2;
    int box_x = (cols - box_width) / 2;

    attron(COLOR_PAIR(4));
    // 绘制角
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

    // 绘制加载文本
    const char* loading_text = "LOADING...";
    mvprintw(box_y + 2, (cols - strlen(loading_text)) / 2, "%s", loading_text);

    // 绘制进度条框架
    mvprintw(box_y + 4, box_x + 2, "[");
    mvprintw(box_y + 4, box_x + box_width - 2, "]");
    attroff(COLOR_PAIR(4));

    // 进度条参数
    int bar_width = box_width - 6;
    int bar_y = box_y + 4;
    int bar_x = box_x + 3;

    // 显示初始进度条空白
    attron(COLOR_PAIR(3));
    for (int i = 0; i < bar_width; i++) {
        mvprintw(bar_y, bar_x + i, " ");
    }
    attroff(COLOR_PAIR(3));
    refresh();

    // 模拟加载进度
    for (int i = 0; i <= bar_width; i++) {
        // 填充进度条
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

    // 显示准备就绪消息
    const char* ready_text = "SYSTEM READY!";
    attron(COLOR_PAIR(2));
    mvprintw(box_y + 8, (cols - strlen(ready_text)) / 2, "%s", ready_text);
    attroff(COLOR_PAIR(2));
    refresh();

    // 等待用户输入
    napms(1000); // 等待1秒
    getch();

    // 清除屏幕并刷新
    clear();
    refresh();
}

// 导航菜单函数
int navigateMenu(WINDOW* win, char* choices[], int n_choices, int startY, int startX) {
    int highlight = 0;  // 当前选中项的索引
    int choice = 0;  // 用户选择的项的索引
    int c;  // 存储用户输入的字符

    // 初始化项的属性
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // 显示菜单
    while(1) {
        for(int i = 0; i < n_choices; i++) {
            if(i == highlight)
                wattron(win, COLOR_PAIR(1));  // 设置选中项的颜色
            mvwprintw(win, startY + i, startX, "%s", choices[i]);  // 打印菜单项
            if(i == highlight)
                wattroff(win, COLOR_PAIR(1));  // 恢复默认颜色
        }

        wrefresh(win);  // 刷新窗口
        c = wgetch(win);  // 获取用户输入

        switch(c) {
            case KEY_UP:
                if(highlight > 0)
                    highlight--;  // 向上移动选中项
                break;
            case KEY_DOWN:
                if(highlight < n_choices - 1)
                    highlight++;  // 向下移动选中项
                break;
            case 10: // Enter键
                choice = highlight;  // 用户按下Enter键，返回选中项的索引
                return choice;
            default:
                break;
        }
    }
}
