#include "database_menu.h"
#include "ui.h"
#include "library.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化数据库菜单颜色
void initDatabaseColors() {
    if (has_colors()) {
        start_color();
        init_pair(3, COLOR_RED, COLOR_BLACK);    // 红色文本
        init_pair(4, COLOR_BLUE, COLOR_BLACK);   // 蓝色文本
        init_pair(5, COLOR_GREEN, COLOR_BLACK);  // 绿色文本
        init_pair(6, COLOR_CYAN, COLOR_BLACK);   // 青色文本
    }
}

// 显示数据库主菜单
void showDatabaseMenu() {
    WINDOW* win = newwin(20, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Database Viewer", 60);
    keypad(win, TRUE); // 启用方向键

    initDatabaseColors();

    int choice = 0;
    int highlight = 0;

    while (1) {
        // 清除窗口内容
        for (int i = 3; i < 19; i++) {
            for (int j = 1; j < 59; j++) {
                mvwaddch(win, i, j, ' ');
            }
        }

        // 显示菜单选项
        char* options[] = {
            "1. View Database Warehouses",
            "2. View Database Goods",
            "3. View Database Vehicles",
            "4. View Database Orders",
            "5. Show Database Statistics",
            "0. Return to Main Menu"
        };

        int numOptions = 6;
        int startY = 5;

        for (int i = 0; i < numOptions; i++) {
            if (i == highlight) {
                wattron(win, COLOR_PAIR(4) | A_BOLD);
                mvwprintw(win, startY + i * 2, 5, ">> %s", options[i]);
                wattroff(win, COLOR_PAIR(4) | A_BOLD);
            } else {
                mvwprintw(win, startY + i * 2, 8, "%s", options[i]);
            }
        }

        wattron(win, COLOR_PAIR(5));
        printCentered(win, 18, "Use UP/DOWN arrows and ENTER to select", 60);
        wattroff(win, COLOR_PAIR(5));

        wrefresh(win);

        int key = wgetch(win);

        switch (key) {
            case KEY_UP:
                highlight = (highlight - 1 + numOptions) % numOptions;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % numOptions;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                choice = highlight;
                switch (choice) {
                    case 0:
                        viewDatabaseWarehouses();
                        break;
                    case 1:
                        viewDatabaseGoods();
                        break;
                    case 2:
                        viewDatabaseVehicles();
                        break;
                    case 3:
                        viewDatabaseOrders();
                        break;
                    case 4:
                        showDatabaseStats();
                        break;
                    case 5:
                        delwin(win);
                        return;
                }
                break;
            case 27: // ESC key
            case 'q':
            case 'Q':
                delwin(win);
                return;
        }
    }
}

// 查看数据库中的仓库（支持滚动）
void viewDatabaseWarehouses() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Database Warehouses", 70);

    initDatabaseColors();
    keypad(win, TRUE); // 启用方向键

    // 检查数据库连接
    if (g_database.db == NULL) {
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 10, "Database not connected!", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // 先获取总记录数
    const char* count_sql = "SELECT COUNT(*) FROM warehouses";
    sqlite3_stmt* count_stmt;
    int total_records = 0;

    if (sqlite3_prepare_v2(g_database.db, count_sql, -1, &count_stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(count_stmt) == SQLITE_ROW) {
            total_records = sqlite3_column_int(count_stmt, 0);
        }
    }
    sqlite3_finalize(count_stmt);

    if (total_records == 0) {
        wattron(win, COLOR_PAIR(3));
        printCentered(win, 10, "No warehouse records in database", 70);
        wattroff(win, COLOR_PAIR(3));
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int offset = 0;
    int max_display = 10; // 每页显示10条记录
    int key;

    while (1) {
        // 清除数据区域
        for (int i = 4; i < 18; i++) {
            for (int j = 1; j < 69; j++) {
                mvwaddch(win, i, j, ' ');
            }
        }

        // 显示表头
        int startY = 4;
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win, startY, 2, "ID");
        mvwprintw(win, startY, 8, "Name");
        mvwprintw(win, startY, 28, "Location");
        mvwprintw(win, startY, 48, "Capacity");
        mvwprintw(win, startY, 58, "Created");
        wattroff(win, COLOR_PAIR(4) | A_BOLD);

        startY += 2;

        // 查询当前页的数据
        char sql[256];
        sprintf(sql, "SELECT id, name, location, capacity, created_at FROM warehouses ORDER BY id LIMIT %d OFFSET %d",
                max_display, offset);
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(g_database.db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            int row = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW && row < max_display) {
                int id = sqlite3_column_int(stmt, 0);
                const char* name = (const char*)sqlite3_column_text(stmt, 1);
                const char* location = (const char*)sqlite3_column_text(stmt, 2);
                int capacity = sqlite3_column_int(stmt, 3);
                const char* created = (const char*)sqlite3_column_text(stmt, 4);

                mvwprintw(win, startY + row, 2, "%d", id);
                mvwprintw(win, startY + row, 8, "%.19s", name ? name : "");
                mvwprintw(win, startY + row, 28, "%.19s", location ? location : "");
                mvwprintw(win, startY + row, 48, "%d", capacity);
                mvwprintw(win, startY + row, 58, "%.10s", created ? created : "");
                row++;
            }
        }
        sqlite3_finalize(stmt);

        // 显示分页信息和操作提示
        wattron(win, COLOR_PAIR(5));
        mvwprintw(win, 16, 2, "Records %d-%d of %d",
                  offset + 1,
                  (offset + max_display > total_records) ? total_records : offset + max_display,
                  total_records);
        wattroff(win, COLOR_PAIR(5));

        wattron(win, COLOR_PAIR(6));
        mvwprintw(win, 17, 2, "UP/DOWN: Scroll  PAGE UP/DOWN: Fast scroll  ESC/Q: Return");
        wattroff(win, COLOR_PAIR(6));

        wrefresh(win);

        // 处理用户输入
        key = wgetch(win);
        switch (key) {
            case KEY_UP:
                if (offset > 0) {
                    offset--;
                }
                break;
            case KEY_DOWN:
                if (offset + max_display < total_records) {
                    offset++;
                }
                break;
            case KEY_PPAGE: // Page Up
                offset -= max_display;
                if (offset < 0) offset = 0;
                break;
            case KEY_NPAGE: // Page Down
                offset += max_display;
                if (offset >= total_records) {
                    offset = total_records - max_display;
                    if (offset < 0) offset = 0;
                }
                break;
            case 27: // ESC
            case 'q':
            case 'Q':
                delwin(win);
                return;
        }
    }
}

// 查看数据库中的货物（支持滚动）
void viewDatabaseGoods() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Database Goods", 70);

    initDatabaseColors();
    keypad(win, TRUE);

    if (g_database.db == NULL) {
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 10, "Database not connected!", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // 获取总记录数
    const char* count_sql = "SELECT COUNT(*) FROM goods";
    sqlite3_stmt* count_stmt;
    int total_records = 0;

    if (sqlite3_prepare_v2(g_database.db, count_sql, -1, &count_stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(count_stmt) == SQLITE_ROW) {
            total_records = sqlite3_column_int(count_stmt, 0);
        }
    }
    sqlite3_finalize(count_stmt);

    if (total_records == 0) {
        wattron(win, COLOR_PAIR(3));
        printCentered(win, 10, "No goods records in database", 70);
        wattroff(win, COLOR_PAIR(3));
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int offset = 0;
    int max_display = 10;
    int key;

    while (1) {
        // 清除数据区域
        for (int i = 4; i < 18; i++) {
            for (int j = 1; j < 69; j++) {
                mvwaddch(win, i, j, ' ');
            }
        }

        // 显示表头
        int startY = 4;
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win, startY, 2, "ID");
        mvwprintw(win, startY, 8, "Name");
        mvwprintw(win, startY, 28, "Quantity");
        mvwprintw(win, startY, 38, "Price");
        mvwprintw(win, startY, 48, "Category");
        mvwprintw(win, startY, 58, "Created");
        wattroff(win, COLOR_PAIR(4) | A_BOLD);

        startY += 2;

        // 查询当前页数据
        char sql[256];
        sprintf(sql, "SELECT id, name, quantity, price, category, created_at FROM goods ORDER BY id LIMIT %d OFFSET %d",
                max_display, offset);
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(g_database.db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            int row = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW && row < max_display) {
                int id = sqlite3_column_int(stmt, 0);
                const char* name = (const char*)sqlite3_column_text(stmt, 1);
                int quantity = sqlite3_column_int(stmt, 2);
                double price = sqlite3_column_double(stmt, 3);
                const char* category = (const char*)sqlite3_column_text(stmt, 4);
                const char* created = (const char*)sqlite3_column_text(stmt, 5);

                mvwprintw(win, startY + row, 2, "%d", id);
                mvwprintw(win, startY + row, 8, "%.19s", name ? name : "");
                mvwprintw(win, startY + row, 28, "%d", quantity);
                mvwprintw(win, startY + row, 38, "%.2f", price);
                mvwprintw(win, startY + row, 48, "%.9s", category ? category : "");
                mvwprintw(win, startY + row, 58, "%.10s", created ? created : "");
                row++;
            }
        }
        sqlite3_finalize(stmt);

        // 显示分页信息
        wattron(win, COLOR_PAIR(5));
        mvwprintw(win, 16, 2, "Records %d-%d of %d",
                  offset + 1,
                  (offset + max_display > total_records) ? total_records : offset + max_display,
                  total_records);
        wattroff(win, COLOR_PAIR(5));

        wattron(win, COLOR_PAIR(6));
        mvwprintw(win, 17, 2, "UP/DOWN: Scroll  PAGE UP/DOWN: Fast scroll  ESC/Q: Return");
        wattroff(win, COLOR_PAIR(6));

        wrefresh(win);

        // 处理用户输入
        key = wgetch(win);
        switch (key) {
            case KEY_UP:
                if (offset > 0) offset--;
                break;
            case KEY_DOWN:
                if (offset + max_display < total_records) offset++;
                break;
            case KEY_PPAGE:
                offset -= max_display;
                if (offset < 0) offset = 0;
                break;
            case KEY_NPAGE:
                offset += max_display;
                if (offset >= total_records) {
                    offset = total_records - max_display;
                    if (offset < 0) offset = 0;
                }
                break;
            case 27: case 'q': case 'Q':
                delwin(win);
                return;
        }
    }
}

// 查看数据库中的车辆（支持滚动）
void viewDatabaseVehicles() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Database Vehicles", 70);

    initDatabaseColors();
    keypad(win, TRUE);

    if (g_database.db == NULL) {
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 10, "Database not connected!", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // 获取总记录数
    const char* count_sql = "SELECT COUNT(*) FROM vehicles";
    sqlite3_stmt* count_stmt;
    int total_records = 0;

    if (sqlite3_prepare_v2(g_database.db, count_sql, -1, &count_stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(count_stmt) == SQLITE_ROW) {
            total_records = sqlite3_column_int(count_stmt, 0);
        }
    }
    sqlite3_finalize(count_stmt);

    if (total_records == 0) {
        wattron(win, COLOR_PAIR(3));
        printCentered(win, 10, "No vehicle records in database", 70);
        wattroff(win, COLOR_PAIR(3));
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int offset = 0;
    int max_display = 10;
    int key;

    while (1) {
        // 清除数据区域
        for (int i = 4; i < 18; i++) {
            for (int j = 1; j < 69; j++) {
                mvwaddch(win, i, j, ' ');
            }
        }

        // 显示表头
        int startY = 4;
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win, startY, 2, "ID");
        mvwprintw(win, startY, 8, "Model");
        mvwprintw(win, startY, 28, "License Plate");
        mvwprintw(win, startY, 43, "Capacity");
        mvwprintw(win, startY, 53, "Created");
        wattroff(win, COLOR_PAIR(4) | A_BOLD);

        startY += 2;

        // 查询当前页数据
        char sql[256];
        sprintf(sql, "SELECT id, model, license_plate, capacity, created_at FROM vehicles ORDER BY id LIMIT %d OFFSET %d",
                max_display, offset);
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(g_database.db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            int row = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW && row < max_display) {
                int id = sqlite3_column_int(stmt, 0);
                const char* model = (const char*)sqlite3_column_text(stmt, 1);
                const char* plate = (const char*)sqlite3_column_text(stmt, 2);
                int capacity = sqlite3_column_int(stmt, 3);
                const char* created = (const char*)sqlite3_column_text(stmt, 4);

                mvwprintw(win, startY + row, 2, "%d", id);
                mvwprintw(win, startY + row, 8, "%.19s", model ? model : "");
                mvwprintw(win, startY + row, 28, "%.14s", plate ? plate : "");
                mvwprintw(win, startY + row, 43, "%d", capacity);
                mvwprintw(win, startY + row, 53, "%.15s", created ? created : "");
                row++;
            }
        }
        sqlite3_finalize(stmt);

        // 显示分页信息
        wattron(win, COLOR_PAIR(5));
        mvwprintw(win, 16, 2, "Records %d-%d of %d",
                  offset + 1,
                  (offset + max_display > total_records) ? total_records : offset + max_display,
                  total_records);
        wattroff(win, COLOR_PAIR(5));

        wattron(win, COLOR_PAIR(6));
        mvwprintw(win, 17, 2, "UP/DOWN: Scroll  PAGE UP/DOWN: Fast scroll  ESC/Q: Return");
        wattroff(win, COLOR_PAIR(6));

        wrefresh(win);

        // 处理用户输入
        key = wgetch(win);
        switch (key) {
            case KEY_UP:
                if (offset > 0) offset--;
                break;
            case KEY_DOWN:
                if (offset + max_display < total_records) offset++;
                break;
            case KEY_PPAGE:
                offset -= max_display;
                if (offset < 0) offset = 0;
                break;
            case KEY_NPAGE:
                offset += max_display;
                if (offset >= total_records) {
                    offset = total_records - max_display;
                    if (offset < 0) offset = 0;
                }
                break;
            case 27: case 'q': case 'Q':
                delwin(win);
                return;
        }
    }
}

// 查看数据库中的订单（支持滚动）
void viewDatabaseOrders() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Database Orders", 70);

    initDatabaseColors();
    keypad(win, TRUE);

    if (g_database.db == NULL) {
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 10, "Database not connected!", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // 获取总记录数
    const char* count_sql = "SELECT COUNT(*) FROM orders";
    sqlite3_stmt* count_stmt;
    int total_records = 0;

    if (sqlite3_prepare_v2(g_database.db, count_sql, -1, &count_stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(count_stmt) == SQLITE_ROW) {
            total_records = sqlite3_column_int(count_stmt, 0);
        }
    }
    sqlite3_finalize(count_stmt);

    if (total_records == 0) {
        wattron(win, COLOR_PAIR(3));
        printCentered(win, 10, "No order records in database", 70);
        wattroff(win, COLOR_PAIR(3));
        printCentered(win, 18, "Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int offset = 0;
    int max_display = 10;
    int key;

    while (1) {
        // 清除数据区域
        for (int i = 4; i < 18; i++) {
            for (int j = 1; j < 69; j++) {
                mvwaddch(win, i, j, ' ');
            }
        }

        // 显示表头
        int startY = 4;
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win, startY, 2, "ID");
        mvwprintw(win, startY, 8, "Order ID");
        mvwprintw(win, startY, 18, "Customer");
        mvwprintw(win, startY, 33, "Goods");
        mvwprintw(win, startY, 48, "Qty");
        mvwprintw(win, startY, 53, "Total");
        mvwprintw(win, startY, 60, "Created");
        wattroff(win, COLOR_PAIR(4) | A_BOLD);

        startY += 2;

        // 查询当前页数据
        char sql[256];
        sprintf(sql, "SELECT id, order_id, customer, goods_name, quantity, total_price, created_at FROM orders ORDER BY id LIMIT %d OFFSET %d",
                max_display, offset);
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(g_database.db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            int row = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW && row < max_display) {
                int id = sqlite3_column_int(stmt, 0);
                const char* order_id = (const char*)sqlite3_column_text(stmt, 1);
                const char* customer = (const char*)sqlite3_column_text(stmt, 2);
                const char* goods = (const char*)sqlite3_column_text(stmt, 3);
                int quantity = sqlite3_column_int(stmt, 4);
                double total = sqlite3_column_double(stmt, 5);
                const char* created = (const char*)sqlite3_column_text(stmt, 6);

                mvwprintw(win, startY + row, 2, "%d", id);
                mvwprintw(win, startY + row, 8, "%.9s", order_id ? order_id : "");
                mvwprintw(win, startY + row, 18, "%.14s", customer ? customer : "");
                mvwprintw(win, startY + row, 33, "%.14s", goods ? goods : "");
                mvwprintw(win, startY + row, 48, "%d", quantity);
                mvwprintw(win, startY + row, 53, "%.2f", total);
                mvwprintw(win, startY + row, 60, "%.8s", created ? created : "");
                row++;
            }
        }
        sqlite3_finalize(stmt);

        // 显示分页信息
        wattron(win, COLOR_PAIR(5));
        mvwprintw(win, 16, 2, "Records %d-%d of %d",
                  offset + 1,
                  (offset + max_display > total_records) ? total_records : offset + max_display,
                  total_records);
        wattroff(win, COLOR_PAIR(5));

        wattron(win, COLOR_PAIR(6));
        mvwprintw(win, 17, 2, "UP/DOWN: Scroll  PAGE UP/DOWN: Fast scroll  ESC/Q: Return");
        wattroff(win, COLOR_PAIR(6));

        wrefresh(win);

        // 处理用户输入
        key = wgetch(win);
        switch (key) {
            case KEY_UP:
                if (offset > 0) offset--;
                break;
            case KEY_DOWN:
                if (offset + max_display < total_records) offset++;
                break;
            case KEY_PPAGE:
                offset -= max_display;
                if (offset < 0) offset = 0;
                break;
            case KEY_NPAGE:
                offset += max_display;
                if (offset >= total_records) {
                    offset = total_records - max_display;
                    if (offset < 0) offset = 0;
                }
                break;
            case 27: case 'q': case 'Q':
                delwin(win);
                return;
        }
    }
}

// 显示数据库统计信息
void showDatabaseStats() {
    WINDOW* win = newwin(15, 50, 5, 15);
    drawBorder(win);
    printTitle(win, "Database Statistics", 50);

    initDatabaseColors();

    if (g_database.db == NULL) {
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 7, "Database not connected!", 50);
        wattroff(win, COLOR_PAIR(3) | A_BOLD);
        printCentered(win, 12, "Press any key to return...", 50);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // 获取各表的记录数
    int warehouseCount = sqlite_get_table_count(&g_database, "warehouses");
    int goodsCount = sqlite_get_table_count(&g_database, "goods");
    int vehicleCount = sqlite_get_table_count(&g_database, "vehicles");
    int orderCount = sqlite_get_table_count(&g_database, "orders");

    int startY = 4;
    wattron(win, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(win, startY++, 5, "Database Records Summary:");
    wattroff(win, COLOR_PAIR(4) | A_BOLD);

    startY++;
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, startY++, 8, "Warehouses: %d", warehouseCount);
    mvwprintw(win, startY++, 8, "Goods:      %d", goodsCount);
    mvwprintw(win, startY++, 8, "Vehicles:   %d", vehicleCount);
    mvwprintw(win, startY++, 8, "Orders:     %d", orderCount);
    wattroff(win, COLOR_PAIR(5));

    startY++;
    wattron(win, COLOR_PAIR(4));
    mvwprintw(win, startY, 8, "Total Records: %d",
              warehouseCount + goodsCount + vehicleCount + orderCount);
    wattroff(win, COLOR_PAIR(4));

    printCentered(win, 12, "Press any key to return...", 50);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}
