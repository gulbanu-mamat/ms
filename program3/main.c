#include <stdio.h>  // 包含标准输入输出库的头文件
#include <stdlib.h> // 包含标准库的头文件，提供内存分配、进程控制等函数
#include <string.h> // 包含字符串处理函数的头文件
#include <time.h>   // 包含时间处理函数的头文件
#include <ctype.h>  // 包含字符处理函数的头文件
#include <ncurses.h> // 包含ncurses库的头文件，用于创建基于文本的用户界面

// Define maximum length constants
// 定义各种数据的最大长度常量
#define MAX_NAME_LEN 50      // 姓名的最大长度
#define MAX_TYPE_LEN 30      // 类型的最大长度
#define MAX_ADDR_LEN 100     // 地址的最大长度
#define MAX_PHONE_LEN 20     // 电话号码的最大长度
#define MAX_TIME_LEN 30      // 时间的最大长度
#define MAX_PLATE_LEN 15     // 车牌号码的最大长度
#define MAX_LOC_LEN 50       // 位置的最大长度
#define MAX_ITEMS 50         // 订单中物品的最大数量
#define MAX_GOODS 100        // 商品的最大数量
#define MAX_VEHICLES 50      // 车辆的最大数量
#define MAX_WAREHOUSES 20    // 仓库的最大数量
#define MAX_ORDERS 100       // 订单的最大数量
#include "library.h"  // 包含自定义的库头文件

// 主函数，程序的入口点
int main() {
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
    return 0;          // 程序正常退出，返回0
}
