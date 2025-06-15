#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 数据库连接结构
typedef struct {
    sqlite3 *db;
    char *error_msg;
} SQLiteDB;

// 数据库初始化和关闭函数
int sqlite_db_init(SQLiteDB *database, const char *db_path);
int sqlite_db_close(SQLiteDB *database);

// 创建表函数
int sqlite_create_tables(SQLiteDB *database);

// 通用执行SQL函数
int sqlite_execute_sql(SQLiteDB *database, const char *sql);

// 数据插入函数
int sqlite_insert_warehouse(SQLiteDB *database, const char *name, const char *location, int capacity);
int sqlite_insert_goods(SQLiteDB *database, const char *name, int quantity, double price, const char *category);
int sqlite_insert_vehicle(SQLiteDB *database, const char *model, const char *license_plate, int capacity);
int sqlite_insert_order(SQLiteDB *database, const char *order_id, const char *customer, const char *goods_name, int quantity, double total_price);

// 数据查询函数
int sqlite_query_warehouses(SQLiteDB *database);
int sqlite_query_goods(SQLiteDB *database);
int sqlite_query_vehicles(SQLiteDB *database);
int sqlite_query_orders(SQLiteDB *database);

// 数据更新函数
int sqlite_update_goods_quantity(SQLiteDB *database, const char *name, int new_quantity);
int sqlite_update_warehouse_capacity(SQLiteDB *database, const char *name, int new_capacity);

// 数据删除函数
int sqlite_delete_goods(SQLiteDB *database, const char *name);
int sqlite_delete_warehouse(SQLiteDB *database, const char *name);

// 辅助函数
void sqlite_print_error(SQLiteDB *database, const char *operation);
int sqlite_get_table_count(SQLiteDB *database, const char *table_name);

// 回调函数类型
typedef int (*sqlite_callback_t)(void *data, int argc, char **argv, char **azColName);

// 通用查询函数
int sqlite_query_with_callback(SQLiteDB *database, const char *sql, sqlite_callback_t callback, void *data);

#endif // SQLITE_DB_H 