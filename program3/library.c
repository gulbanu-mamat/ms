#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量定义
Goods goodsList[MAX_GOODS];
int goodsCount = 0;
int nextGoodsId = 1;

Vehicle vehiclesList[MAX_VEHICLES];
int vehiclesCount = 0;
int nextVehicleId = 1;

Warehouse warehousesList[MAX_WAREHOUSES];
int warehousesCount = 0;
int nextWarehouseId = 1;

Order ordersList[MAX_ORDERS];
int ordersCount = 0;
int nextOrderId = 1;


// 通用工具函数实现
void getCurrentDateTime(char* buffer) {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, MAX_TIME_LEN, "%Y-%m-%d %H:%M:%S", timeinfo);
}
