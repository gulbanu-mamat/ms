#include "warehouse.h"
#include "fileio.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// addWarehouse、viewWarehouses的实现
void addWarehouse() {
    if (warehousesCount >= MAX_WAREHOUSES) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Warehouse storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Warehouse w;
    w.id = nextWarehouseId++;
    w.usedCapacity = 0.0;

    WINDOW* win = newwin(20, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Add New Warehouse", 60);

    mvwprintw(win, 4, 2, "Warehouse Name: ");
    echo();
    mvwgetnstr(win, 4, 18, w.name, MAX_NAME_LEN - 1);

    mvwprintw(win, 5, 2, "Location: ");
    mvwgetnstr(win, 5, 12, w.location, MAX_LOC_LEN - 1);

    char buffer[20];
    mvwprintw(win, 6, 2, "Capacity(m³): ");
    mvwgetnstr(win, 6, 16, buffer, 19);
    w.capacity = atof(buffer);
    noecho();

    warehousesList[warehousesCount++] = w;
    saveWarehouses();

    printCentered(win, 8, "Warehouse added successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewWarehouses() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Warehouse List", 70);

    if (warehousesCount == 0) {
        printCentered(win, 10, "No warehouse records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 5, "Name");
    mvwprintw(win, startY, 25, "Location");
    mvwprintw(win, startY, 45, "Capacity(m³)");
    mvwprintw(win, startY, 58, "Used(m³)");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < warehousesCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", warehousesList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%.19s", warehousesList[i].name);
        mvwprintw(win, startY + displayCount, 25, "%.19s", warehousesList[i].location);
        mvwprintw(win, startY + displayCount, 45, "%.2f", warehousesList[i].capacity);
        mvwprintw(win, startY + displayCount, 58, "%.2f", warehousesList[i].usedCapacity);
        displayCount++;
    }

    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}
