#include "vehicle.h"
#include "fileio.h"
#include "ui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initCarsColors() {
    if (has_colors() && can_change_color()) {
        start_color();
        // 定义颜色对：红色文本搭配黑色背景
        init_pair(3, COLOR_RED, COLOR_BLACK);
        // 定义颜色对：蓝色文本搭配黑色背景
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
    }
}

// 添加新车辆
void addVehicle() {
    // 检查车辆存储是否已满
    if (vehiclesCount >= MAX_VEHICLES) {
        WINDOW* popup = newwin(5, 40, 10, 20);  // 创建弹出窗口
        box(popup, 0, 0);  // 绘制窗口边框

        initCarsColors();
        wattron(popup, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性
        mvwprintw(popup, 2, 5, "Vehicle storage full, cannot add more!");  // 显示提示信息
        wattroff(popup, COLOR_PAIR(3) | A_BOLD); // 禁用属性

        wrefresh(popup);  // 刷新窗口
        wgetch(popup);  // 等待用户输入
        delwin(popup);  // 删除窗口
        return;
    }

    Vehicle v;
    v.id = nextVehicleId++;  // 分配车辆ID
    v.available = 1; // 默认车辆可用

    WINDOW* win = newwin(20, 60, 2, 10);  // 创建新窗口
    drawBorder(win);  // 绘制窗口边框
    printTitle(win, "Add New Vehicle", 60);  // 打印窗口标题

    mvwprintw(win, 4, 2, "License Plate: ");  // 显示提示信息
    echo();  // 开启回显模式
    mvwgetnstr(win, 4, 16, v.plateNumber, MAX_PLATE_LEN - 1);  // 获取用户输入的车牌号码

    mvwprintw(win, 5, 2, "Vehicle Type: ");  // 显示提示信息
    mvwgetnstr(win, 5, 16, v.type, MAX_TYPE_LEN - 1);  // 获取用户输入的车辆类型

    char buffer[20];
    mvwprintw(win, 6, 2, "Max Load(kg): ");  // 显示提示信息
    mvwgetnstr(win, 6, 17, buffer, 19);  // 获取用户输入的最大载重
    v.maxLoad = atof(buffer);  // 将输入的最大载重转换为浮点数

    mvwprintw(win, 7, 2, "Max Volume(m³): ");  // 显示提示信息
    mvwgetnstr(win, 7, 17, buffer, 19);  // 获取用户输入的最大容积
    v.maxVolume = atof(buffer);  // 将输入的最大容积转换为浮点数

    mvwprintw(win, 8, 2, "Current Location: ");  // 显示提示信息
    mvwgetnstr(win, 8, 20, v.currentLocation, MAX_LOC_LEN - 1);  // 获取用户输入的当前位置
    noecho();  // 关闭回显模式

    vehiclesList[vehiclesCount++] = v;  // 将车辆添加到车辆列表
    saveVehicles();  // 保存车辆信息
    
    // 同时保存到SQLite数据库
    if (sqlite_insert_vehicle(&g_database, v.type, v.plateNumber, (int)v.maxLoad) != 0) {
        printf("警告：车辆保存到SQLite数据库失败\n");
    }

    // 车辆添加成功，显示提示信息
     initCarsColors(); // 初始化自定义颜色
    wattron(win, COLOR_PAIR(4) | A_BOLD); // 启用蓝色和粗体属性
    printCentered(win, 20, "Vehicle added successfully! Press any key to return...", 60);
    wattroff(win, COLOR_PAIR(4) | A_BOLD); // 禁用属性

    wrefresh(win);  // 刷新窗口
    wgetch(win);  // 等待用户输入
    delwin(win);  // 删除窗口
}

// 查看车辆列表
void viewVehicles() {
    WINDOW* win = newwin(20, 70, 2, 5);  // 创建新窗口
    drawBorder(win);  // 绘制窗口边框
    printTitle(win, "Vehicle List", 70);  // 打印窗口标题

    if (vehiclesCount == 0) {
        // 没有车辆记录，显示提示信息
        initCarsColors();
        wattron(win, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性
        printCentered(win, 20, "No vehicle records! Press any key to return...", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD); // 禁用属性

        wrefresh(win);  // 刷新窗口
        wgetch(win);  // 等待用户输入
        delwin(win);  // 删除窗口
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");  // 显示列标题
    mvwprintw(win, startY, 5, "Plate");  // 显示列标题
    mvwprintw(win, startY, 15, "Type");  // 显示列标题
    mvwprintw(win, startY, 30, "Load(kg)");  // 显示列标题
    mvwprintw(win, startY, 41, "Volume(m³)");  // 显示列标题
    mvwprintw(win, startY, 52, "Status");  // 显示列标题
    mvwprintw(win, startY, 60, "Location");  // 显示列标题
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < vehiclesCount && startY + displayCount < 18; i++) {
        // 显示车辆信息
        mvwprintw(win, startY + displayCount, 1, "%d", vehiclesList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%s", vehiclesList[i].plateNumber);
        mvwprintw(win, startY + displayCount, 15, "%.14s", vehiclesList[i].type);
        mvwprintw(win, startY + displayCount, 30, "%.2f", vehiclesList[i].maxLoad);
        mvwprintw(win, startY + displayCount, 41, "%.2f", vehiclesList[i].maxVolume);
        mvwprintw(win, startY + displayCount, 52, "%s", vehiclesList[i].available ? "Available" : "In use");
        mvwprintw(win, startY + displayCount, 60, "%.10s", vehiclesList[i].currentLocation);
        displayCount++;
    }

    // 显示返回提示信息
    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);  // 刷新窗口
    wgetch(win);  // 等待用户输入
    delwin(win);  // 删除窗口
}

// 根据车辆ID查找车辆索引
int findVehicleById(int id) {
    for (int i = 0; i < vehiclesCount; i++) {
        if (vehiclesList[i].id == id) {
            return i;  // 找到车辆，返回索引
        }
    }
    return -1;  // 未找到车辆，返回-1
}
