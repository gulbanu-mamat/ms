#include "goods.h"
#include "fileio.h"
#include "library.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// addGoods、viewGoods、findGoodsById的实现
void addGoods() {
    if (goodsCount >= MAX_GOODS) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Goods storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Goods g;
    g.id = nextGoodsId++;

    WINDOW* win = newwin(22, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Add New Product", 60);

    // 显示已存在商品的id和名称
    mvwprintw(win, 3, 2, "Existing Products:");
    int showCount = goodsCount < 10 ? goodsCount : 10; // 最多显示10个
    for (int i = 0; i < showCount; i++) {
        mvwprintw(win, 4 + i, 4, "ID:%d  Name:%s", goodsList[i].id, goodsList[i].name);
    }

    int inputRow = 4 + showCount + 1;

    mvwprintw(win, inputRow, 2, "Product Name: ");
    echo();
    mvwgetnstr(win, inputRow, 16, g.name, MAX_NAME_LEN - 1);

    mvwprintw(win, inputRow + 1, 2, "Product Type: ");
    mvwgetnstr(win, inputRow + 1, 16, g.type, MAX_TYPE_LEN - 1);

    char buffer[20];
    mvwprintw(win, inputRow + 2, 2, "Weight(kg): ");
    mvwgetnstr(win, inputRow + 2, 14, buffer, 19);
    g.weight = atof(buffer);

    mvwprintw(win, inputRow + 3, 2, "Volume(m³): ");
    mvwgetnstr(win, inputRow + 3, 14, buffer, 19);
    g.volume = atof(buffer);

    mvwprintw(win, inputRow + 4, 2, "Price($): ");
    mvwgetnstr(win, inputRow + 4, 14, buffer, 19);
    g.price = atof(buffer);

    mvwprintw(win, inputRow + 5, 2, "Quantity: ");
    mvwgetnstr(win, inputRow + 5, 14, buffer, 19);
    g.quantity = atoi(buffer);
    noecho();

    goodsList[goodsCount++] = g;

    printCentered(win, inputRow + 7, "Product added successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewGoods() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Product List", 70);

    if (goodsCount == 0) {
        printCentered(win, 10, "No product records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 6, "Name");
    mvwprintw(win, startY, 22, "Type");
    mvwprintw(win, startY, 35, "Weight(kg)");
    mvwprintw(win, startY, 46, "Volume(m³)");
    mvwprintw(win, startY, 57, "Price($)");
    mvwprintw(win, startY, 68, "Qty");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < goodsCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", goodsList[i].id);
        mvwprintw(win, startY + displayCount, 6, "%.15s", goodsList[i].name);
        mvwprintw(win, startY + displayCount, 22, "%.12s", goodsList[i].type);
        mvwprintw(win, startY + displayCount, 35, "%.2f", goodsList[i].weight);
        mvwprintw(win, startY + displayCount, 46, "%.2f", goodsList[i].volume);
        mvwprintw(win, startY + displayCount, 57, "%.2f", goodsList[i].price);
        mvwprintw(win, startY + displayCount, 68, "%d", goodsList[i].quantity);
        displayCount++;
    }

    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

int findGoodsById(int id) {
    for (int i = 0; i < goodsCount; i++) {
        if (goodsList[i].id == id) {
            return i;
        }
    }
    return -1;
}
