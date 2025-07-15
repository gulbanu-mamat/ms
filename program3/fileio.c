#include "fileio.h"
#include <stdio.h>

// saveGoods��loadGoods��saveVehicles��loadVehicles��saveWarehouses��loadWarehouses��saveOrders��loadOrders��ʵ��

// ����Ʒ��Ϣ���浽�ļ�
void saveGoods() {
    FILE* fp = fopen("goods.dat", "wb");  // �Զ�����д��ģʽ���ļ�
    if (fp) {
        fwrite(&goodsCount, sizeof(int), 1, fp);  // д����Ʒ����
        fwrite(goodsList, sizeof(Goods), goodsCount, fp);  // д����Ʒ�б�
        fclose(fp);  // �ر��ļ�
    }
}

// ���ļ��м�����Ʒ��Ϣ
void loadGoods() {
    FILE* fp = fopen("goods.dat", "rb");  // �Զ����ƶ�ȡģʽ���ļ�
    if (fp) {
        fread(&goodsCount, sizeof(int), 1, fp);  // ��ȡ��Ʒ����
        fread(goodsList, sizeof(Goods), goodsCount, fp);  // ��ȡ��Ʒ�б�
        fclose(fp);  // �ر��ļ�
        nextGoodsId = 1;
        // �ҳ�������ƷID����������һ����Ʒ��ID
        for (int i = 0; i < goodsCount; i++)
            if (goodsList[i].id >= nextGoodsId)
                nextGoodsId = goodsList[i].id + 1;
    }
}

// ��������Ϣ���浽�ļ�
void saveVehicles() {
    FILE* fp = fopen("vehicles.dat", "wb");  // �Զ�����д��ģʽ���ļ�
    if (fp) {
        fwrite(&vehiclesCount, sizeof(int), 1, fp);  // д�복������
        fwrite(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);  // д�복���б�
        fclose(fp);  // �ر��ļ�
    }
}

// ���ļ��м��س�����Ϣ
void loadVehicles() {
    FILE* fp = fopen("vehicles.dat", "rb");  // �Զ����ƶ�ȡģʽ���ļ�
    if (fp) {
        fread(&vehiclesCount, sizeof(int), 1, fp);  // ��ȡ��������
        fread(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);  // ��ȡ�����б�
        fclose(fp);  // �ر��ļ�
        nextVehicleId = 1;
        // �ҳ����ĳ���ID����������һ��������ID
        for (int i = 0; i < vehiclesCount; i++)
            if (vehiclesList[i].id >= nextVehicleId)
                nextVehicleId = vehiclesList[i].id + 1;
    }
}

// ���ֿ���Ϣ���浽�ļ�
void saveWarehouses() {
    FILE* fp = fopen("warehouses.dat", "wb");  // �Զ�����д��ģʽ���ļ�
    if (fp) {
        fwrite(&warehousesCount, sizeof(int), 1, fp);  // д��ֿ�����
        fwrite(warehousesList, sizeof(Warehouse), warehousesCount, fp);  // д��ֿ��б�
        fclose(fp);  // �ر��ļ�
    }
}

// ���ļ��м��زֿ���Ϣ
void loadWarehouses() {
    FILE* fp = fopen("warehouses.dat", "rb");  // �Զ����ƶ�ȡģʽ���ļ�
    if (fp) {
        fread(&warehousesCount, sizeof(int), 1, fp);  // ��ȡ�ֿ�����
        fread(warehousesList, sizeof(Warehouse), warehousesCount, fp);  // ��ȡ�ֿ��б�
        fclose(fp);  // �ر��ļ�
        nextWarehouseId = 1;
        // �ҳ����Ĳֿ�ID����������һ���ֿ��ID
        for (int i = 0; i < warehousesCount; i++)
            if (warehousesList[i].id >= nextWarehouseId)
                nextWarehouseId = warehousesList[i].id + 1;
    }
}

// ��������Ϣ���浽�ļ�
void saveOrders() {
    FILE* fp = fopen("orders.dat", "wb");  // �Զ�����д��ģʽ���ļ�
    if (fp) {
        fwrite(&ordersCount, sizeof(int), 1, fp);  // д�붩������
        fwrite(ordersList, sizeof(Order), ordersCount, fp);  // д�붩���б�
        fclose(fp);  // �ر��ļ�
    }
}

// ���ļ��м��ض�����Ϣ
void loadOrders() {
    FILE* fp = fopen("orders.dat", "rb");  // �Զ����ƶ�ȡģʽ���ļ�
    if (fp) {
        fread(&ordersCount, sizeof(int), 1, fp);  // ��ȡ��������
        fread(ordersList, sizeof(Order), ordersCount, fp);  // ��ȡ�����б�
        fclose(fp);  // �ر��ļ�
        nextOrderId = 1;
        // �ҳ����Ķ���ID����������һ��������ID
        for (int i = 0; i < ordersCount; i++)
            if (ordersList[i].id >= nextOrderId)
                nextOrderId = ordersList[i].id + 1;
    }
}
