#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ȫ�ֱ�������
// ��Ʒ�б��洢������Ʒ��Ϣ
Goods goodsList[MAX_GOODS];
int goodsCount = 0;   //��ǰ
int nextGoodsId = 1;

// �����б��洢���г�����Ϣ
Vehicle vehiclesList[MAX_VEHICLES];
int vehiclesCount = 0;
int nextVehicleId = 1;

// �ֿ��б��洢���вֿ���Ϣ
Warehouse warehousesList[MAX_WAREHOUSES];
int warehousesCount = 0;
int nextWarehouseId = 1;

// �����б��洢���ж�����Ϣ
Order ordersList[MAX_ORDERS];
int ordersCount = 0;
int nextOrderId = 1;


// ��ȡ��ǰ���ں�ʱ�䣬������洢�ڻ�������
void getCurrentDateTime(char* buffer) {
    time_t now = time(0);  // ��ȡ��ǰʱ��
    struct tm* timeinfo = localtime(&now);  // ��ʱ��ת��Ϊ����ʱ��
    // ��ʽ��ʱ�䲢�洢�ڻ�������
    strftime(buffer, MAX_TIME_LEN, "%Y-%m-%d %H:%M:%S", timeinfo);
}
