#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

// C++标准库头文件
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <ncurses.h>

// 使用标准命名空间
using namespace std;

// 
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

// �ṹ�嶨��
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char type[MAX_TYPE_LEN];
    double weight;
    double volume;
    double price;
    int quantity;
} Goods;

typedef struct {
    int id;
    char plateNumber[MAX_PLATE_LEN];
    char type[MAX_TYPE_LEN];
    double maxLoad;
    double maxVolume;
    int available; // 1 means available, 0 means unavailable
    char currentLocation[MAX_LOC_LEN];
} Vehicle;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char location[MAX_LOC_LEN];
    double capacity;
    double usedCapacity;
} Warehouse;

typedef struct {
    int goodsId;
    int quantity;
} OrderItem;

typedef struct {
    int id;
    char customerName[MAX_NAME_LEN];
    char customerPhone[MAX_PHONE_LEN];
    char sourceAddress[MAX_ADDR_LEN];
    char destinationAddress[MAX_ADDR_LEN];
    char status[20]; // "Pending", "Shipping", "Completed"
    char createTime[MAX_TIME_LEN];
    char deliveryTime[MAX_TIME_LEN];
    OrderItem items[MAX_ITEMS];
    int itemCount;
    int assignedVehicle; // -1 means unassigned
} Order;

// 全局变量声明
extern Goods goodsList[MAX_GOODS];
extern int goodsCount, nextGoodsId;
extern Vehicle vehiclesList[MAX_VEHICLES];
extern int vehiclesCount, nextVehicleId;
extern Warehouse warehousesList[MAX_WAREHOUSES];
extern int warehousesCount, nextWarehouseId;
extern Order ordersList[MAX_ORDERS];
extern int ordersCount, nextOrderId;

// SQLite数据库全局变量
#include "sqlite_db.h"
extern SQLiteDB g_database;

// ���ߺ���
void getCurrentDateTime(char* buffer);
void drawBorder(WINDOW* win); // 修正函数声明，添加WINDOW*参数
void printTitle(WINDOW* win, const char* title, int width); // 修正函数声明，添加参数
void printCentered(WINDOW* win, int y, const char* msg, int width);
void loadGoods();
void loadVehicles();
void loadWarehouses();
void loadOrders();
void showStartupScreen();
void showMainMenu();
void saveGoods();
void saveVehicles();
void saveWarehouses();
void saveOrders();
void addGoods();
void viewGoods();
void addVehicle();
void viewVehicles();
void addWarehouse();
void viewWarehouses();
void createOrder();
void viewOrders();
void processOrders();
void completeOrders();

#endif // LIBRARY_H_INCLUDED
