#include "goods.h"
#include "fileio.h"
#include "library.h"
#include "ui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化颜色和属性
void initCustomColors() {
    if (has_colors() && can_change_color()) {
        start_color();
        // 定义颜色对：红色文本搭配黑色背景
        init_pair(3, COLOR_RED, COLOR_BLACK);
        // 定义颜色对：蓝色文本搭配黑色背景
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
    }
}

// 添加新商品
void addGoods() {
    // 检查商品存储是否已满
    if (goodsCount >= MAX_GOODS) {
        WINDOW* popup = newwin(5, 40, 10, 20);  // 创建弹出窗口
        box(popup, 0, 0);  // 绘制窗口边框

        initCustomColors(); // 初始化自定义颜色
        wattron(popup, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性
        mvwprintw(popup, 2, 5, "Warehouse storage full, cannot add more!");  // 显示提示信息
        wattroff(popup, COLOR_PAIR(3) | A_BOLD); // 禁用属性

        wrefresh(popup);  // 刷新窗口
        wgetch(popup);  // 等待用户输入
        delwin(popup);  // 删除窗口
        return;
    }

    Goods g;
    g.id = nextGoodsId++;  // 分配商品ID

    WINDOW* win = newwin(22, 60, 2, 10);  // 创建新窗口
    drawBorder(win);  // 绘制窗口边框
    printTitle(win, "Add New Product", 60);  // 打印窗口标题

    // 显示已存在商品的id和名称
    mvwprintw(win, 3, 2, "Existing Products:");
    int showCount = goodsCount < 10 ? goodsCount : 10; // 最多显示10个
    for (int i = 0; i < showCount; i++) {
        // 显示已存在商品的信息
        mvwprintw(win, 4 + i, 4, "ID:%d  Name:%s", goodsList[i].id, goodsList[i].name);
    }

    int inputRow = 4 + showCount + 1;

    mvwprintw(win, inputRow, 2, "Product Name: ");  // 显示提示信息
    echo();  // 开启回显模式
    mvwgetnstr(win, inputRow, 16, g.name, MAX_NAME_LEN - 1);  // 获取用户输入的商品名称

    mvwprintw(win, inputRow + 1, 2, "Product Type: ");  // 显示提示信息
    mvwgetnstr(win, inputRow + 1, 16, g.type, MAX_TYPE_LEN - 1);  // 获取用户输入的商品类型

    char buffer[20];
    mvwprintw(win, inputRow + 2, 2, "Weight(kg): ");  // 显示提示信息
    mvwgetnstr(win, inputRow + 2, 14, buffer, 19);  // 获取用户输入的商品重量
    g.weight = atof(buffer);  // 将输入的商品重量转换为浮点数

    mvwprintw(win, inputRow + 3, 2, "Volume(m³): ");  // 显示提示信息
    mvwgetnstr(win, inputRow + 3, 14, buffer, 19);  // 获取用户输入的商品体积
    g.volume = atof(buffer);  // 将输入的商品体积转换为浮点数

    mvwprintw(win, inputRow + 4, 2, "Price($): ");  // 显示提示信息
    mvwgetnstr(win, inputRow + 4, 14, buffer, 19);  // 获取用户输入的商品价格
    g.price = atof(buffer);  // 将输入的商品价格转换为浮点数

    mvwprintw(win, inputRow + 5, 2, "Quantity: ");  // 显示提示信息
    mvwgetnstr(win, inputRow + 5, 14, buffer, 19);  // 获取用户输入的商品数量
    g.quantity = atoi(buffer);  // 将输入的商品数量转换为整数
    noecho();  // 关闭回显模式

    goodsList[goodsCount++] = g;  // 将商品添加到商品列表
    
    // 同时保存到SQLite数据库
    if (sqlite_insert_goods(&g_database, g.name, g.quantity, g.price, g.type) != 0) {
        printf("警告：商品保存到SQLite数据库失败\n");
    }

    // 商品添加成功，显示提示信息

   // 使用蓝色加粗显示成功提示
    initCustomColors(); // 初始化自定义颜色
    wattron(win, COLOR_PAIR(4) | A_BOLD); // 启用蓝色和粗体属性
    printCentered(win, 20, "Product added successfully! Press any key to return...", 60);
    wattroff(win, COLOR_PAIR(4) | A_BOLD); // 禁用属性


    wrefresh(win);  // 刷新窗口
    wgetch(win);  // 等待用户输入
    delwin(win);  // 删除窗口
}

// 查看商品列表
void viewGoods() {
    WINDOW* win = newwin(20, 70, 2, 5);  // 创建新窗口
    drawBorder(win);  // 绘制窗口边框
    printTitle(win, "Product List", 70);  // 打印窗口标题

    if (goodsCount == 0) {
        // 没有仓库记录，使用红色加粗显示提示信息
        wattron(win, COLOR_PAIR(3) | A_BOLD); // 启用红色和粗体属性
        printCentered(win, 20, "No warehouse records! Press any key to return...", 70);
        wattroff(win, COLOR_PAIR(3) | A_BOLD); // 禁用属性

        wrefresh(win);  // 刷新窗口
        wgetch(win);  // 等待用户输入
        delwin(win);  // 删除窗口
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");  // 显示列标题
    mvwprintw(win, startY, 6, "Name");  // 显示列标题
    mvwprintw(win, startY, 22, "Type");  // 显示列标题
    mvwprintw(win, startY, 35, "Weight(kg)");  // 显示列标题
    mvwprintw(win, startY, 46, "Volume(m³)");  // 显示列标题
    mvwprintw(win, startY, 57, "Price($)");  // 显示列标题
    mvwprintw(win, startY, 68, "Qty");  // 显示列标题
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < goodsCount && startY + displayCount < 18; i++) {
        // 显示商品信息
        mvwprintw(win, startY + displayCount, 1, "%d", goodsList[i].id);
        mvwprintw(win, startY + displayCount, 6, "%.15s", goodsList[i].name);
        mvwprintw(win, startY + displayCount, 22, "%.12s", goodsList[i].type);
        mvwprintw(win, startY + displayCount, 35, "%.2f", goodsList[i].weight);
        mvwprintw(win, startY + displayCount, 46, "%.2f", goodsList[i].volume);
        mvwprintw(win, startY + displayCount, 57, "%.2f", goodsList[i].price);
        mvwprintw(win, startY + displayCount, 68, "%d", goodsList[i].quantity);
        displayCount++;
    }

    // 显示返回提示信息
    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);  // 刷新窗口
    wgetch(win);  // 等待用户输入
    delwin(win);  // 删除窗口
}

// 根据商品ID查找商品索引
int findGoodsById(int id) {
    for (int i = 0; i < goodsCount; i++) {
        if (goodsList[i].id == id) {
            return i;  // 找到商品，返回索引
        }
    }
    return -1;  // 未找到商品，返回-1
}
