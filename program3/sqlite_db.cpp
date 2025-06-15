#include "sqlite_db.h"
#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

// 默认回调函数，用于打印查询结果
static int default_callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\t";
    }
    cout << endl;
    return 0;
}

// 初始化数据库连接
int sqlite_db_init(SQLiteDB *database, const char *db_path) {
    if (!database || !db_path) {
        return -1;
    }
    
    int rc = sqlite3_open(db_path, &database->db);
    if (rc) {
        cerr << "无法打开数据库: " << sqlite3_errmsg(database->db) << endl;
        return -1;
    } else {
        cout << "成功打开数据库: " << db_path << endl;
    }
    
    database->error_msg = NULL;
    return 0;
}

// 关闭数据库连接
int sqlite_db_close(SQLiteDB *database) {
    if (!database || !database->db) {
        return -1;
    }
    
    int rc = sqlite3_close(database->db);
    if (rc != SQLITE_OK) {
        cerr << "关闭数据库失败: " << sqlite3_errmsg(database->db) << endl;
        return -1;
    }
    
    if (database->error_msg) {
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
    }
    
    cout << "数据库连接已关闭" << endl;
    return 0;
}

// 创建所有表
int sqlite_create_tables(SQLiteDB *database) {
    if (!database || !database->db) {
        return -1;
    }
    
    // 创建仓库表
    const char *create_warehouses_sql = 
        "CREATE TABLE IF NOT EXISTS warehouses ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE,"
        "location TEXT NOT NULL,"
        "capacity INTEGER NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    // 创建货物表
    const char *create_goods_sql = 
        "CREATE TABLE IF NOT EXISTS goods ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE,"
        "quantity INTEGER NOT NULL,"
        "price REAL NOT NULL,"
        "category TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    // 创建车辆表
    const char *create_vehicles_sql = 
        "CREATE TABLE IF NOT EXISTS vehicles ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "model TEXT NOT NULL,"
        "license_plate TEXT NOT NULL UNIQUE,"
        "capacity INTEGER NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    // 创建订单表
    const char *create_orders_sql = 
        "CREATE TABLE IF NOT EXISTS orders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "order_id TEXT NOT NULL UNIQUE,"
        "customer TEXT NOT NULL,"
        "goods_name TEXT NOT NULL,"
        "quantity INTEGER NOT NULL,"
        "total_price REAL NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    // 执行创建表的SQL语句
    if (sqlite_execute_sql(database, create_warehouses_sql) != 0) {
        return -1;
    }
    
    if (sqlite_execute_sql(database, create_goods_sql) != 0) {
        return -1;
    }
    
    if (sqlite_execute_sql(database, create_vehicles_sql) != 0) {
        return -1;
    }
    
    if (sqlite_execute_sql(database, create_orders_sql) != 0) {
        return -1;
    }
    
    cout << "所有表创建成功" << endl;
    return 0;
}

// 通用SQL执行函数
int sqlite_execute_sql(SQLiteDB *database, const char *sql) {
    if (!database || !database->db || !sql) {
        return -1;
    }
    
    int rc = sqlite3_exec(database->db, sql, NULL, NULL, &database->error_msg);
    if (rc != SQLITE_OK) {
        cerr << "SQL执行错误: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
        return -1;
    }
    
    return 0;
}

// 插入仓库数据
int sqlite_insert_warehouse(SQLiteDB *database, const char *name, const char *location, int capacity) {
    if (!database || !database->db || !name || !location) {
        return -1;
    }
    
    char sql[512];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO warehouses (name, location, capacity) VALUES ('%s', '%s', %d);",
        name, location, capacity);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "仓库 '" << name << "' 插入成功" << endl;
    }
    
    return result;
}

// 插入货物数据
int sqlite_insert_goods(SQLiteDB *database, const char *name, int quantity, double price, const char *category) {
    if (!database || !database->db || !name || !category) {
        return -1;
    }
    
    char sql[512];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO goods (name, quantity, price, category) VALUES ('%s', %d, %.2f, '%s');",
        name, quantity, price, category);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "货物 '" << name << "' 插入成功" << endl;
    }
    
    return result;
}

// 插入车辆数据
int sqlite_insert_vehicle(SQLiteDB *database, const char *model, const char *license_plate, int capacity) {
    if (!database || !database->db || !model || !license_plate) {
        return -1;
    }
    
    char sql[512];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO vehicles (model, license_plate, capacity) VALUES ('%s', '%s', %d);",
        model, license_plate, capacity);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "车辆 '" << license_plate << "' 插入成功" << endl;
    }
    
    return result;
}

// 插入订单数据
int sqlite_insert_order(SQLiteDB *database, const char *order_id, const char *customer, 
                       const char *goods_name, int quantity, double total_price) {
    if (!database || !database->db || !order_id || !customer || !goods_name) {
        return -1;
    }
    
    char sql[512];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO orders (order_id, customer, goods_name, quantity, total_price) VALUES ('%s', '%s', '%s', %d, %.2f);",
        order_id, customer, goods_name, quantity, total_price);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "订单 '" << order_id << "' 插入成功" << endl;
    }
    
    return result;
}

// 查询仓库数据
int sqlite_query_warehouses(SQLiteDB *database) {
    if (!database || !database->db) {
        return -1;
    }
    
    const char *sql = "SELECT * FROM warehouses;";
    cout << "\n=== 仓库信息 ===\n";
    
    int rc = sqlite3_exec(database->db, sql, default_callback, NULL, &database->error_msg);
    if (rc != SQLITE_OK) {
        cerr << "查询仓库数据失败: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
        return -1;
    }
    
    return 0;
}

// 查询货物数据
int sqlite_query_goods(SQLiteDB *database) {
    if (!database || !database->db) {
        return -1;
    }
    
    const char *sql = "SELECT * FROM goods;";
    cout << "\n=== 货物信息 ===\n";
    
    int rc = sqlite3_exec(database->db, sql, default_callback, NULL, &database->error_msg);
    if (rc != SQLITE_OK) {
        cerr << "查询货物数据失败: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
        return -1;
    }
    
    return 0;
}

// 查询车辆数据
int sqlite_query_vehicles(SQLiteDB *database) {
    if (!database || !database->db) {
        return -1;
    }
    
    const char *sql = "SELECT * FROM vehicles;";
    cout << "\n=== 车辆信息 ===\n";
    
    int rc = sqlite3_exec(database->db, sql, default_callback, NULL, &database->error_msg);
    if (rc != SQLITE_OK) {
        cerr << "查询车辆数据失败: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
        return -1;
    }
    
    return 0;
}

// 查询订单数据
int sqlite_query_orders(SQLiteDB *database) {
    if (!database || !database->db) {
        return -1;
    }
    
    const char *sql = "SELECT * FROM orders;";
    cout << "\n=== 订单信息 ===\n";
    
    int rc = sqlite3_exec(database->db, sql, default_callback, NULL, &database->error_msg);
    if (rc != SQLITE_OK) {
        cerr << "查询订单数据失败: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
        return -1;
    }
    
    return 0;
}

// 更新货物数量
int sqlite_update_goods_quantity(SQLiteDB *database, const char *name, int new_quantity) {
    if (!database || !database->db || !name) {
        return -1;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), 
        "UPDATE goods SET quantity = %d WHERE name = '%s';",
        new_quantity, name);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "货物 '" << name << "' 数量更新为 " << new_quantity << endl;
    }
    
    return result;
}

// 更新仓库容量
int sqlite_update_warehouse_capacity(SQLiteDB *database, const char *name, int new_capacity) {
    if (!database || !database->db || !name) {
        return -1;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), 
        "UPDATE warehouses SET capacity = %d WHERE name = '%s';",
        new_capacity, name);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "仓库 '" << name << "' 容量更新为 " << new_capacity << endl;
    }
    
    return result;
}

// 删除货物
int sqlite_delete_goods(SQLiteDB *database, const char *name) {
    if (!database || !database->db || !name) {
        return -1;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM goods WHERE name = '%s';", name);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "货物 '" << name << "' 删除成功" << endl;
    }
    
    return result;
}

// 删除仓库
int sqlite_delete_warehouse(SQLiteDB *database, const char *name) {
    if (!database || !database->db || !name) {
        return -1;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM warehouses WHERE name = '%s';", name);
    
    int result = sqlite_execute_sql(database, sql);
    if (result == 0) {
        cout << "仓库 '" << name << "' 删除成功" << endl;
    }
    
    return result;
}

// 打印错误信息
void sqlite_print_error(SQLiteDB *database, const char *operation) {
    if (!database || !operation) {
        return;
    }
    
    if (database->error_msg) {
        cerr << operation << " 失败: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
    }
}

// 获取表中记录数量
int sqlite_get_table_count(SQLiteDB *database, const char *table_name) {
    if (!database || !database->db || !table_name) {
        return -1;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT COUNT(*) FROM %s;", table_name);
    
    sqlite3_stmt *stmt;
    int count = 0;
    
    if (sqlite3_prepare_v2(database->db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    
    sqlite3_finalize(stmt);
    return count;
}

// 通用查询函数，支持自定义回调
int sqlite_query_with_callback(SQLiteDB *database, const char *sql, sqlite_callback_t callback, void *data) {
    if (!database || !database->db || !sql) {
        return -1;
    }
    
    sqlite_callback_t cb = callback ? callback : default_callback;
    
    int rc = sqlite3_exec(database->db, sql, cb, data, &database->error_msg);
    if (rc != SQLITE_OK) {
        cerr << "查询执行失败: " << database->error_msg << endl;
        sqlite3_free(database->error_msg);
        database->error_msg = NULL;
        return -1;
    }
    
    return 0;
}