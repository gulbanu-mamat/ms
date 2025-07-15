#include <stdio.h>
#include <stdlib.h>
#include "sqlite_db.h"

// 演示数据库操作的主函数
int main() {
    SQLiteDB database;
    
    printf("=== SQLite 数据库演示程序 ===\n\n");
    
    // 1. 初始化数据库连接
    printf("1. 初始化数据库连接...\n");
    if (sqlite_db_init(&database, "warehouse_management.db") != 0) {
        fprintf(stderr, "数据库初始化失败！\n");
        return 1;
    }
    
    // 2. 创建表
    printf("\n2. 创建数据库表...\n");
    if (sqlite_create_tables(&database) != 0) {
        fprintf(stderr, "创建表失败！\n");
        sqlite_db_close(&database);
        return 1;
    }
    
    // 3. 插入仓库数据
    printf("\n3. 插入仓库数据...\n");
    sqlite_insert_warehouse(&database, "北京仓库", "北京市朝阳区", 10000);
    sqlite_insert_warehouse(&database, "上海仓库", "上海市浦东新区", 15000);
    sqlite_insert_warehouse(&database, "广州仓库", "广州市天河区", 12000);
    
    // 4. 插入货物数据
    printf("\n4. 插入货物数据...\n");
    sqlite_insert_goods(&database, "苹果手机", 100, 5999.00, "电子产品");
    sqlite_insert_goods(&database, "笔记本电脑", 50, 8999.00, "电子产品");
    sqlite_insert_goods(&database, "运动鞋", 200, 299.00, "服装");
    sqlite_insert_goods(&database, "图书", 500, 59.90, "文化用品");
    
    // 5. 插入车辆数据
    printf("\n5. 插入车辆数据...\n");
    sqlite_insert_vehicle(&database, "东风卡车", "京A12345", 5000);
    sqlite_insert_vehicle(&database, "解放卡车", "沪B67890", 8000);
    sqlite_insert_vehicle(&database, "江淮卡车", "粤C11111", 6000);
    
    // 6. 插入订单数据
    printf("\n6. 插入订单数据...\n");
    sqlite_insert_order(&database, "ORD001", "张三", "苹果手机", 2, 11998.00);
    sqlite_insert_order(&database, "ORD002", "李四", "笔记本电脑", 1, 8999.00);
    sqlite_insert_order(&database, "ORD003", "王五", "运动鞋", 3, 897.00);
    
    // 7. 查询所有数据
    printf("\n7. 查询数据库中的所有数据...\n");
    sqlite_query_warehouses(&database);
    sqlite_query_goods(&database);
    sqlite_query_vehicles(&database);
    sqlite_query_orders(&database);
    
    // 8. 更新数据
    printf("\n8. 更新数据...\n");
    sqlite_update_goods_quantity(&database, "苹果手机", 150);
    sqlite_update_warehouse_capacity(&database, "北京仓库", 12000);
    
    // 9. 再次查询以验证更新
    printf("\n9. 验证更新结果...\n");
    sqlite_query_goods(&database);
    sqlite_query_warehouses(&database);
    
    // 10. 统计表中记录数量
    printf("\n10. 统计表中记录数量...\n");
    printf("仓库数量: %d\n", sqlite_get_table_count(&database, "warehouses"));
    printf("货物数量: %d\n", sqlite_get_table_count(&database, "goods"));
    printf("车辆数量: %d\n", sqlite_get_table_count(&database, "vehicles"));
    printf("订单数量: %d\n", sqlite_get_table_count(&database, "orders"));
    
    // 11. 自定义查询
    printf("\n11. 执行自定义查询...\n");
    printf("查询价格大于1000的货物:\n");
    sqlite_query_with_callback(&database, 
        "SELECT name, price, category FROM goods WHERE price > 1000;", 
        NULL, NULL);
    
    printf("\n查询总价值大于10000的订单:\n");
    sqlite_query_with_callback(&database, 
        "SELECT order_id, customer, total_price FROM orders WHERE total_price > 10000;", 
        NULL, NULL);
    
    // 12. 删除数据演示
    printf("\n12. 删除数据演示...\n");
    sqlite_delete_goods(&database, "图书");
    
    printf("删除后的货物列表:\n");
    sqlite_query_goods(&database);
    
    // 13. 关闭数据库连接
    printf("\n13. 关闭数据库连接...\n");
    sqlite_db_close(&database);
    
    printf("\n=== 演示程序执行完成 ===\n");
    return 0;
}

// 演示高级功能的函数
void advanced_demo() {
    SQLiteDB database;
    
    printf("\n=== 高级功能演示 ===\n");
    
    if (sqlite_db_init(&database, "warehouse_management.db") != 0) {
        return;
    }
    
    // 复杂查询示例
    printf("\n复杂查询示例:\n");
    
    // 查询每个类别的货物总数和平均价格
    printf("各类别货物统计:\n");
    sqlite_query_with_callback(&database,
        "SELECT category, COUNT(*) as count, AVG(price) as avg_price, SUM(quantity) as total_quantity "
        "FROM goods GROUP BY category;",
        NULL, NULL);
    
    // 查询总价值最高的前3个订单
    printf("\n总价值最高的前3个订单:\n");
    sqlite_query_with_callback(&database,
        "SELECT order_id, customer, goods_name, total_price "
        "FROM orders ORDER BY total_price DESC LIMIT 3;",
        NULL, NULL);
    
    // 查询仓库容量利用率（假设当前存储量为总容量的60%）
    printf("\n仓库容量信息:\n");
    sqlite_query_with_callback(&database,
        "SELECT name, location, capacity, "
        "ROUND(capacity * 0.6) as current_usage, "
        "ROUND((capacity * 0.6 / capacity) * 100, 2) || '%' as usage_rate "
        "FROM warehouses;",
        NULL, NULL);
    
    sqlite_db_close(&database);
} 