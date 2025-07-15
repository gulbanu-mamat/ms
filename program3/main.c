#include <stdio.h>  // ������׼����������ͷ�ļ�
#include <stdlib.h> // ������׼���ͷ�ļ����ṩ�ڴ���䡢���̿��ƵȺ���
#include <string.h> // �����ַ�������������ͷ�ļ�
#include <time.h>   // ����ʱ�䴦��������ͷ�ļ�
#include <ctype.h>  // �����ַ�����������ͷ�ļ�
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
#include "library.h"  // 包含自定义的库头文件
#include "sqlite_db.h" // 包含SQLite数据库头文件

// 全局数据库连接
SQLiteDB g_database;

// 主函数，程序的入口点
int main() {
    // 初始化SQLite数据库
    if (sqlite_db_init(&g_database, "warehouse_system.db") == 0) {
        printf("SQLite数据库初始化成功\n");
        sqlite_create_tables(&g_database);
    } else {
        printf("SQLite数据库初始化失败，将仅使用文件存储\n");
    }

    loadGoods();       // 从文件中加载商品信息
    loadVehicles();    // 从文件中加载车辆信息
    loadWarehouses();  // 从文件中加载仓库信息
    loadOrders();      // 从文件中加载订单信息
    showStartupScreen(); // 显示启动屏幕
    showMainMenu();      // 显示主菜单

    // 程序退出前再次保存数据
    saveGoods();       // 将商品信息保存到文件
    saveVehicles();    // 将车辆信息保存到文件
    saveWarehouses();  // 将仓库信息保存到文件
    saveOrders();      // 将订单信息保存到文件

    // 关闭SQLite数据库连接
    sqlite_db_close(&g_database);

    return 0;          // 返回程序退出状态码0
}
