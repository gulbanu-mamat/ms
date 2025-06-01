#include "fileio.h"
#include <stdio.h>

// saveGoods¡¢loadGoods¡¢saveVehicles¡¢loadVehicles¡¢saveWarehouses¡¢loadWarehouses¡¢saveOrders¡¢loadOrdersµÄÊµÏÖ
void saveGoods() {
    FILE* fp = fopen("goods.dat", "wb");
    if (fp) {
        fwrite(&goodsCount, sizeof(int), 1, fp);
        fwrite(goodsList, sizeof(Goods), goodsCount, fp);
        fclose(fp);
    }
}
void loadGoods() {
    FILE* fp = fopen("goods.dat", "rb");
    if (fp) {
        fread(&goodsCount, sizeof(int), 1, fp);
        fread(goodsList, sizeof(Goods), goodsCount, fp);
        fclose(fp);
        nextGoodsId = 1;
        for (int i = 0; i < goodsCount; i++)
            if (goodsList[i].id >= nextGoodsId)
                nextGoodsId = goodsList[i].id + 1;
    }
}
void saveVehicles() {
    FILE* fp = fopen("vehicles.dat", "wb");
    if (fp) {
        fwrite(&vehiclesCount, sizeof(int), 1, fp);
        fwrite(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);
        fclose(fp);
    }
}
void loadVehicles() {
    FILE* fp = fopen("vehicles.dat", "rb");
    if (fp) {
        fread(&vehiclesCount, sizeof(int), 1, fp);
        fread(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);
        fclose(fp);
        nextVehicleId = 1;
        for (int i = 0; i < vehiclesCount; i++)
            if (vehiclesList[i].id >= nextVehicleId)
                nextVehicleId = vehiclesList[i].id + 1;
    }
}
void saveWarehouses() {
    FILE* fp = fopen("warehouses.dat", "wb");
    if (fp) {
        fwrite(&warehousesCount, sizeof(int), 1, fp);
        fwrite(warehousesList, sizeof(Warehouse), warehousesCount, fp);
        fclose(fp);
    }
}
void loadWarehouses() {
    FILE* fp = fopen("warehouses.dat", "rb");
    if (fp) {
        fread(&warehousesCount, sizeof(int), 1, fp);
        fread(warehousesList, sizeof(Warehouse), warehousesCount, fp);
        fclose(fp);
        nextWarehouseId = 1;
        for (int i = 0; i < warehousesCount; i++)
            if (warehousesList[i].id >= nextWarehouseId)
                nextWarehouseId = warehousesList[i].id + 1;
    }
}
void saveOrders() {
    FILE* fp = fopen("orders.dat", "wb");
    if (fp) {
        fwrite(&ordersCount, sizeof(int), 1, fp);
        fwrite(ordersList, sizeof(Order), ordersCount, fp);
        fclose(fp);
    }
}
void loadOrders() {
    FILE* fp = fopen("orders.dat", "rb");
    if (fp) {
        fread(&ordersCount, sizeof(int), 1, fp);
        fread(ordersList, sizeof(Order), ordersCount, fp);
        fclose(fp);
        nextOrderId = 1;
        for (int i = 0; i < ordersCount; i++)
            if (ordersList[i].id >= nextOrderId)
                nextOrderId = ordersList[i].id + 1;
    }
}
