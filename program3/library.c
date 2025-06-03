#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量声明
// 商品列表，存储所有商品信息
Goods goodsList[MAX_GOODS];
int goodsCount = 0;   //当前
int nextGoodsId = 1;

// 车辆列表，存储所有车辆信息
Vehicle vehiclesList[MAX_VEHICLES];
int vehiclesCount = 0;
int nextVehicleId = 1;

// 仓库列表，存储所有仓库信息
Warehouse warehousesList[MAX_WAREHOUSES];
int warehousesCount = 0;
int nextWarehouseId = 1;

// 订单列表，存储所有订单信息
Order ordersList[MAX_ORDERS];
int ordersCount = 0;
int nextOrderId = 1;


// 获取当前日期和时间，并将其存储在缓冲区中
void getCurrentDateTime(char* buffer) {
    time_t now = time(0);  // 获取当前时间
    struct tm* timeinfo = localtime(&now);  // 将时间转换为本地时间
    // 格式化时间并存储在缓冲区中
    strftime(buffer, MAX_TIME_LEN, "%Y-%m-%d %H:%M:%S", timeinfo);
}
