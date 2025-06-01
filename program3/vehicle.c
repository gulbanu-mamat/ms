#include "vehicle.h"
#include "fileio.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// addVehicle、viewVehicles、findVehicleById的实现
void addVehicle() {
    if (vehiclesCount >= MAX_VEHICLES) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Vehicle storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Vehicle v;
    v.id = nextVehicleId++;
    v.available = 1; // Default to available

    WINDOW* win = newwin(20, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Add New Vehicle", 60);

    mvwprintw(win, 4, 2, "License Plate: ");
    echo();
    mvwgetnstr(win, 4, 16, v.plateNumber, MAX_PLATE_LEN - 1);

    mvwprintw(win, 5, 2, "Vehicle Type: ");
    mvwgetnstr(win, 5, 16, v.type, MAX_TYPE_LEN - 1);

    char buffer[20];
    mvwprintw(win, 6, 2, "Max Load(kg): ");
    mvwgetnstr(win, 6, 17, buffer, 19);
    v.maxLoad = atof(buffer);

    mvwprintw(win, 7, 2, "Max Volume(m³): ");
    mvwgetnstr(win, 7, 17, buffer, 19);
    v.maxVolume = atof(buffer);

    mvwprintw(win, 8, 2, "Current Location: ");
    mvwgetnstr(win, 8, 20, v.currentLocation, MAX_LOC_LEN - 1);
    noecho();

    vehiclesList[vehiclesCount++] = v;
    saveVehicles();

    printCentered(win, 10, "Vehicle added successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewVehicles() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Vehicle List", 70);

    if (vehiclesCount == 0) {
        printCentered(win, 10, "No vehicle records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 5, "Plate");
    mvwprintw(win, startY, 15, "Type");
    mvwprintw(win, startY, 30, "Load(kg)");
    mvwprintw(win, startY, 41, "Volume(m³)");
    mvwprintw(win, startY, 52, "Status");
    mvwprintw(win, startY, 60, "Location");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < vehiclesCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", vehiclesList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%s", vehiclesList[i].plateNumber);
        mvwprintw(win, startY + displayCount, 15, "%.14s", vehiclesList[i].type);
        mvwprintw(win, startY + displayCount, 30, "%.2f", vehiclesList[i].maxLoad);
        mvwprintw(win, startY + displayCount, 41, "%.2f", vehiclesList[i].maxVolume);
        mvwprintw(win, startY + displayCount, 52, "%s", vehiclesList[i].available ? "Available" : "In use");
        mvwprintw(win, startY + displayCount, 60, "%.10s", vehiclesList[i].currentLocation);
        displayCount++;
    }

    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

int findVehicleById(int id) {
    for (int i = 0; i < vehiclesCount; i++) {
        if (vehiclesList[i].id == id) {
            return i;
        }
    }
    return -1;
}

