#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <ncurses.h>

// Define maximum length constants
#define MAX_NAME_LEN 50
#define MAX_TYPE_LEN 30
#define MAX_ADDR_LEN 100
#define MAX_PHONE_LEN 20
#define MAX_TIME_LEN 30
#define MAX_PLATE_LEN 15
#define MAX_LOC_LEN 50
#define MAX_ITEMS 50
#define MAX_GOODS 100
#define MAX_VEHICLES 50
#define MAX_WAREHOUSES 20
#define MAX_ORDERS 100
#include "library.h"



int main() {
    loadGoods();
    loadVehicles();
    loadWarehouses();
    loadOrders();
    showStartupScreen();
    showMainMenu();
    // 退出前再次保存
    saveGoods();
    saveVehicles();
    saveWarehouses();
    saveOrders();
    return 0;
}
