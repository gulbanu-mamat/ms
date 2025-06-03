#include <stdio.h>  // ������׼����������ͷ�ļ�
#include <stdlib.h> // ������׼���ͷ�ļ����ṩ�ڴ���䡢���̿��ƵȺ���
#include <string.h> // �����ַ�����������ͷ�ļ�
#include <time.h>   // ����ʱ�䴦������ͷ�ļ�
#include <ctype.h>  // �����ַ���������ͷ�ļ�
#include <ncurses.h> // ����ncurses���ͷ�ļ������ڴ��������ı����û�����

// Define maximum length constants
// ����������ݵ���󳤶ȳ���
#define MAX_NAME_LEN 50      // ��������󳤶�
#define MAX_TYPE_LEN 30      // ���͵���󳤶�
#define MAX_ADDR_LEN 100     // ��ַ����󳤶�
#define MAX_PHONE_LEN 20     // �绰�������󳤶�
#define MAX_TIME_LEN 30      // ʱ�����󳤶�
#define MAX_PLATE_LEN 15     // ���ƺ������󳤶�
#define MAX_LOC_LEN 50       // λ�õ���󳤶�
#define MAX_ITEMS 50         // ��������Ʒ���������
#define MAX_GOODS 100        // ��Ʒ���������
#define MAX_VEHICLES 50      // �������������
#define MAX_WAREHOUSES 20    // �ֿ���������
#define MAX_ORDERS 100       // �������������
#include "library.h"  // �����Զ���Ŀ�ͷ�ļ�

// ���������������ڵ�
int main() {
    loadGoods();       // ���ļ��м�����Ʒ��Ϣ
    loadVehicles();    // ���ļ��м��س�����Ϣ
    loadWarehouses();  // ���ļ��м��زֿ���Ϣ
    loadOrders();      // ���ļ��м��ض�����Ϣ
    showStartupScreen(); // ��ʾ������Ļ
    showMainMenu();      // ��ʾ���˵�
    // �����˳�ǰ�ٴα�������
    saveGoods();       // ����Ʒ��Ϣ���浽�ļ�
    saveVehicles();    // ��������Ϣ���浽�ļ�
    saveWarehouses();  // ���ֿ���Ϣ���浽�ļ�
    saveOrders();      // ��������Ϣ���浽�ļ�
    return 0;          // ���������˳�������0
}
