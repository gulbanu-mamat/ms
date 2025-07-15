#include "warehouse.h"
#include "fileio.h"
#include "ui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initWarehouseColors() {
    if (has_colors() && can_change_color()) {
        start_color();
        // 定义颜色对：红色文本搭配黑色背景
        init_pair(3, COLOR_RED, COLOR_BLACK);
        // 定义颜色对：蓝色文本搭配黑色背景
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
    }
}

// 添加新仓库
void addWarehouse() {
    // 检查仓库存储是否已满
    if (warehousesCount >= MAX_WAREHOUSES) {
        WINDOW* popup = newwin(5, 40, 10, 20);  // 创建弹出窗口
        box(popup, 0, 0);  // 绘制窗口边框

        initWarehouseColors();
        wattron(popup, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性
        mvwprintw(popup, 2, 5, "Warehouse storage full, cannot add more!");  // 显示提示信息
        wattroff(popup, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性

        wrefresh(popup);  // 刷新窗口
        wgetch(popup);  // 等待用户输入
        delwin(popup);  // 删除窗口
        return;
    }

    Warehouse w;
    w.id = nextWarehouseId++;  // 分配仓库ID
    w.usedCapacity = 0.0;  // 初始化已使用容量为0

    WINDOW* win = newwin(20, 60, 2, 10);  // 创建新窗口
    drawBorder(win);  // 绘制窗口边框
    printTitle(win, "Add New Warehouse", 60);  // 打印窗口标题

    mvwprintw(win, 4, 2, "Warehouse Name: ");  // 显示提示信息
    echo();  // 开启回显模式
    mvwgetnstr(win, 4, 18, w.name, MAX_NAME_LEN - 1);  // 获取用户输入的仓库名称

    mvwprintw(win, 5, 2, "Location: ");  // 显示提示信息
    mvwgetnstr(win, 5, 12, w.location, MAX_LOC_LEN - 1);  // 获取用户输入的仓库位置

    char buffer[20];
    mvwprintw(win, 6, 2, "Capacity(m³): ");  // 显示提示信息
    mvwgetnstr(win, 6, 16, buffer, 19);  // 获取用户输入的仓库容量
    w.capacity = atof(buffer);  // 将输入的容量转换为浮点数
    noecho();  // 关闭回显模式

    warehousesList[warehousesCount++] = w;  // 将仓库添加到仓库列表
    saveWarehouses();  // 保存仓库信息

    // 同时保存到SQLite数据库
    if (sqlite_insert_warehouse(&g_database, w.name, w.location, (int)w.capacity) != 0) {
        printf("警告：仓库保存到SQLite数据库失败\n");
    }

    // 仓库添加成功，显示提示信息
    initWarehouseColors(); // 初始化自定义颜色
    wattron(win, COLOR_PAIR(4) | A_BOLD); // 启用蓝色和粗体属性
    printCentered(win, 10, "Warehouse added successfully! Press any key to return...", 60);
    wattroff(win, COLOR_PAIR(4) | A_BOLD); // 禁用属性

    wrefresh(win);  // 刷新窗口
    wgetch(win);  // 等待用户输入
    delwin(win);  // 删除窗口
}

// 查看仓库列表
void viewWarehouses() {
    WINDOW* win = newwin(20, 70, 2, 5);  // 创建新窗口
    drawBorder(win);  // 绘制窗口边框
    printTitle(win, "Warehouse List", 70);  // 打印窗口标题

    if (warehousesCount == 0) {
        // 没有仓库记录，显示提示信息
        initWarehouseColors();
        wattron(win, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性
        printCentered(win, 20, "No warehouse records! Press any key to return...", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性

        wrefresh(win);  // 刷新窗口
        wgetch(win);  // 等待用户输入
        delwin(win);  // 删除窗口
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");  // 显示列标题
    mvwprintw(win, startY, 5, "Name");  // 显示列标题
    mvwprintw(win, startY, 25, "Location");  // 显示列标题
    mvwprintw(win, startY, 45, "Capacity(m³)");  // 显示列标题
    mvwprintw(win, startY, 58, "Used(m³)");  // 显示列标题
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < warehousesCount && startY + displayCount < 18; i++) {
        // 显示仓库信息
        mvwprintw(win, startY + displayCount, 1, "%d", warehousesList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%.19s", warehousesList[i].name);
        mvwprintw(win, startY + displayCount, 25, "%.19s", warehousesList[i].location);
        mvwprintw(win, startY + displayCount, 45, "%.2f", warehousesList[i].capacity);
        mvwprintw(win, startY + displayCount, 58, "%.2f", warehousesList[i].usedCapacity);
        displayCount++;
    }

    // 显示返回提示信息
    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);  // 刷新窗口
    wgetch(win);  // 等待用户输入
    delwin(win);  // 删除窗口
}
