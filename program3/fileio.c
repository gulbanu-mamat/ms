#include "fileio.h"
#include <stdio.h>

// saveGoods、loadGoods、saveVehicles、loadVehicles、saveWarehouses、loadWarehouses、saveOrders、loadOrders的实现

// 将商品信息保存到文件
void saveGoods() {
    FILE* fp = fopen("goods.dat", "wb");  // 以二进制写入模式打开文件
    if (fp) {
        fwrite(&goodsCount, sizeof(int), 1, fp);  // 写入商品数量
        fwrite(goodsList, sizeof(Goods), goodsCount, fp);  // 写入商品列表
        fclose(fp);  // 关闭文件
    }
}

// 从文件中加载商品信息
void loadGoods() {
    FILE* fp = fopen("goods.dat", "rb");  // 以二进制读取模式打开文件
    if (fp) {
        fread(&goodsCount, sizeof(int), 1, fp);  // 读取商品数量
        fread(goodsList, sizeof(Goods), goodsCount, fp);  // 读取商品列表
        fclose(fp);  // 关闭文件
        nextGoodsId = 1;
        // 找出最大的商品ID，并更新下一个商品的ID
        for (int i = 0; i < goodsCount; i++)
            if (goodsList[i].id >= nextGoodsId)
                nextGoodsId = goodsList[i].id + 1;
    }
}

// 将车辆信息保存到文件
void saveVehicles() {
    FILE* fp = fopen("vehicles.dat", "wb");  // 以二进制写入模式打开文件
    if (fp) {
        fwrite(&vehiclesCount, sizeof(int), 1, fp);  // 写入车辆数量
        fwrite(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);  // 写入车辆列表
        fclose(fp);  // 关闭文件
    }
}

// 从文件中加载车辆信息
void loadVehicles() {
    FILE* fp = fopen("vehicles.dat", "rb");  // 以二进制读取模式打开文件
    if (fp) {
        fread(&vehiclesCount, sizeof(int), 1, fp);  // 读取车辆数量
        fread(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);  // 读取车辆列表
        fclose(fp);  // 关闭文件
        nextVehicleId = 1;
        // 找出最大的车辆ID，并更新下一个车辆的ID
        for (int i = 0; i < vehiclesCount; i++)
            if (vehiclesList[i].id >= nextVehicleId)
                nextVehicleId = vehiclesList[i].id + 1;
    }
}

// 将仓库信息保存到文件
void saveWarehouses() {
    FILE* fp = fopen("warehouses.dat", "wb");  // 以二进制写入模式打开文件
    if (fp) {
        fwrite(&warehousesCount, sizeof(int), 1, fp);  // 写入仓库数量
        fwrite(warehousesList, sizeof(Warehouse), warehousesCount, fp);  // 写入仓库列表
        fclose(fp);  // 关闭文件
    }
}

// 从文件中加载仓库信息
void loadWarehouses() {
    FILE* fp = fopen("warehouses.dat", "rb");  // 以二进制读取模式打开文件
    if (fp) {
        fread(&warehousesCount, sizeof(int), 1, fp);  // 读取仓库数量
        fread(warehousesList, sizeof(Warehouse), warehousesCount, fp);  // 读取仓库列表
        fclose(fp);  // 关闭文件
        nextWarehouseId = 1;
        // 找出最大的仓库ID，并更新下一个仓库的ID
        for (int i = 0; i < warehousesCount; i++)
            if (warehousesList[i].id >= nextWarehouseId)
                nextWarehouseId = warehousesList[i].id + 1;
    }
}

// 将订单信息保存到文件
void saveOrders() {
    FILE* fp = fopen("orders.dat", "wb");  // 以二进制写入模式打开文件
    if (fp) {
        fwrite(&ordersCount, sizeof(int), 1, fp);  // 写入订单数量
        fwrite(ordersList, sizeof(Order), ordersCount, fp);  // 写入订单列表
        fclose(fp);  // 关闭文件
    }
}

// 从文件中加载订单信息
void loadOrders() {
    FILE* fp = fopen("orders.dat", "rb");  // 以二进制读取模式打开文件
    if (fp) {
        fread(&ordersCount, sizeof(int), 1, fp);  // 读取订单数量
        fread(ordersList, sizeof(Order), ordersCount, fp);  // 读取订单列表
        fclose(fp);  // 关闭文件
        nextOrderId = 1;
        // 找出最大的订单ID，并更新下一个订单的ID
        for (int i = 0; i < ordersCount; i++)
            if (ordersList[i].id >= nextOrderId)
                nextOrderId = ordersList[i].id + 1;
    }
}
