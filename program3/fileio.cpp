#include "fileio.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

// External variable for current user
extern string currentUser;

// Helper function to get user-specific file path
void getUserFilePath(char* filepath, const char* filename) {
    if (!currentUser.empty()) {
        sprintf(filepath, "users/%s/%s", currentUser.c_str(), filename);
    } else {
        strcpy(filepath, filename);
    }
}

// Save goods information to file
void saveGoods() {
    char filepath[200];
    getUserFilePath(filepath, "goods.dat");
    FILE* fp = fopen(filepath, "wb");
    if (fp) {
        fwrite(&goodsCount, sizeof(int), 1, fp);
        fwrite(goodsList, sizeof(Goods), goodsCount, fp);
        fclose(fp);
    }
}

// Load goods information from file
void loadGoods() {
    char filepath[200];
    getUserFilePath(filepath, "goods.dat");
    FILE* fp = fopen(filepath, "rb");
    if (fp) {
        fread(&goodsCount, sizeof(int), 1, fp);
        fread(goodsList, sizeof(Goods), goodsCount, fp);
        fclose(fp);
        nextGoodsId = 1;
        // Find the maximum goods ID and set the next goods ID
        for (int i = 0; i < goodsCount; i++)
            if (goodsList[i].id >= nextGoodsId)
                nextGoodsId = goodsList[i].id + 1;
    }
}

// Save vehicles information to file
void saveVehicles() {
    char filepath[200];
    getUserFilePath(filepath, "vehicles.dat");
    FILE* fp = fopen(filepath, "wb");
    if (fp) {
        fwrite(&vehiclesCount, sizeof(int), 1, fp);
        fwrite(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);
        fclose(fp);
    }
}

// Load vehicles information from file
void loadVehicles() {
    char filepath[200];
    getUserFilePath(filepath, "vehicles.dat");
    FILE* fp = fopen(filepath, "rb");
    if (fp) {
        fread(&vehiclesCount, sizeof(int), 1, fp);
        fread(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);
        fclose(fp);
        nextVehicleId = 1;
        // Find the maximum vehicle ID and set the next vehicle ID
        for (int i = 0; i < vehiclesCount; i++)
            if (vehiclesList[i].id >= nextVehicleId)
                nextVehicleId = vehiclesList[i].id + 1;
    }
}

// Save warehouses information to file
void saveWarehouses() {
    char filepath[200];
    getUserFilePath(filepath, "warehouses.dat");
    FILE* fp = fopen(filepath, "wb");
    if (fp) {
        fwrite(&warehousesCount, sizeof(int), 1, fp);
        fwrite(warehousesList, sizeof(Warehouse), warehousesCount, fp);
        fclose(fp);
    }
}

// Load warehouses information from file
void loadWarehouses() {
    char filepath[200];
    getUserFilePath(filepath, "warehouses.dat");
    FILE* fp = fopen(filepath, "rb");
    if (fp) {
        fread(&warehousesCount, sizeof(int), 1, fp);
        fread(warehousesList, sizeof(Warehouse), warehousesCount, fp);
        fclose(fp);
        nextWarehouseId = 1;
        // Find the maximum warehouse ID and set the next warehouse ID
        for (int i = 0; i < warehousesCount; i++)
            if (warehousesList[i].id >= nextWarehouseId)
                nextWarehouseId = warehousesList[i].id + 1;
    }
}

// Save orders information to file
void saveOrders() {
    char filepath[200];
    getUserFilePath(filepath, "orders.dat");
    FILE* fp = fopen(filepath, "wb");
    if (fp) {
        fwrite(&ordersCount, sizeof(int), 1, fp);
        fwrite(ordersList, sizeof(Order), ordersCount, fp);
        fclose(fp);
    }
}

// Load orders information from file
void loadOrders() {
    char filepath[200];
    getUserFilePath(filepath, "orders.dat");
    FILE* fp = fopen(filepath, "rb");
    if (fp) {
        fread(&ordersCount, sizeof(int), 1, fp);
        fread(ordersList, sizeof(Order), ordersCount, fp);
        fclose(fp);
        nextOrderId = 1;
        // Find the maximum order ID and set the next order ID
        for (int i = 0; i < ordersCount; i++)
            if (ordersList[i].id >= nextOrderId)
                nextOrderId = ordersList[i].id + 1;
    }
}
