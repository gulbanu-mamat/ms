# SQLite 数据库项目

这是一个使用 C 语言和 SQLite 数据库的完整项目，包含了仓库管理系统的数据库操作功能。

## 项目结构

```
├── sqlite_db.h        # SQLite 数据库头文件
├── sqlite_db.c        # SQLite 数据库实现文件
├── sqlite_demo.c      # 演示程序
├── Makefile          # 编译配置文件
└── README_SQLite.md  # 项目说明文档
```

## 环境要求

- **操作系统**: Windows 10/11
- **编译环境**: MSYS2
- **数据库**: SQLite3
- **编译器**: GCC

## 安装 MSYS2 和 SQLite3

### 1. 安装 MSYS2

如果还没有安装 MSYS2，请从 [https://www.msys2.org/](https://www.msys2.org/) 下载并安装。

### 2. 安装 SQLite3

在 MSYS2 终端中运行以下命令：

```bash
# 更新 pacman 数据库
pacman -Syu

# 安装 SQLite3 开发包
pacman -S mingw-w64-x86_64-sqlite3

# 安装 pkg-config（用于自动检测库）
pacman -S mingw-w64-x86_64-pkg-config

# 安装 GCC 编译器（如果还没有）
pacman -S mingw-w64-x86_64-gcc
```

### 3. 验证安装

```bash
# 检查 SQLite3 版本
sqlite3 --version

# 检查 pkg-config 是否能找到 SQLite3
pkg-config --modversion sqlite3
```

## 编译和运行

### 使用 Makefile

```bash
# 检查 SQLite3 安装状态
make check-sqlite

# 编译项目
make

# 编译并运行演示程序
make run

# 清理编译文件
make clean

# 查看所有可用命令
make help
```

### 手动编译

```bash
# 编译 SQLite 数据库模块
gcc -c sqlite_db.c -o sqlite_db.o $(pkg-config --cflags sqlite3)

# 编译演示程序
gcc -c sqlite_demo.c -o sqlite_demo.o $(pkg-config --cflags sqlite3)

# 链接生成可执行文件
gcc sqlite_db.o sqlite_demo.o -o sqlite_demo $(pkg-config --libs sqlite3)

# 运行程序
./sqlite_demo
```

## 功能特性

### 数据库操作

- ✅ **数据库连接管理**: 打开/关闭数据库连接
- ✅ **表结构创建**: 自动创建仓库、货物、车辆、订单表
- ✅ **数据插入**: 支持插入各类数据记录
- ✅ **数据查询**: 支持单表查询和复杂查询
- ✅ **数据更新**: 支持更新记录字段
- ✅ **数据删除**: 支持删除记录
- ✅ **统计功能**: 获取表中记录数量
- ✅ **自定义查询**: 支持自定义 SQL 查询语句

### 数据表结构

#### 1. 仓库表 (warehouses)
- `id`: 主键 (自增)
- `name`: 仓库名称 (唯一)
- `location`: 仓库位置
- `capacity`: 仓库容量
- `created_at`: 创建时间

#### 2. 货物表 (goods)
- `id`: 主键 (自增)
- `name`: 货物名称 (唯一)
- `quantity`: 货物数量
- `price`: 货物价格
- `category`: 货物类别
- `created_at`: 创建时间

#### 3. 车辆表 (vehicles)
- `id`: 主键 (自增)
- `model`: 车辆型号
- `license_plate`: 车牌号 (唯一)
- `capacity`: 载重量
- `created_at`: 创建时间

#### 4. 订单表 (orders)
- `id`: 主键 (自增)
- `order_id`: 订单编号 (唯一)
- `customer`: 客户姓名
- `goods_name`: 货物名称
- `quantity`: 订单数量
- `total_price`: 总价
- `created_at`: 创建时间

## API 使用说明

### 初始化和关闭

```c
SQLiteDB database;

// 初始化数据库连接
if (sqlite_db_init(&database, "your_database.db") != 0) {
    // 处理错误
}

// 关闭数据库连接
sqlite_db_close(&database);
```

### 创建表

```c
// 创建所有表
if (sqlite_create_tables(&database) != 0) {
    // 处理错误
}
```

### 插入数据

```c
// 插入仓库数据
sqlite_insert_warehouse(&database, "仓库名", "位置", 容量);

// 插入货物数据
sqlite_insert_goods(&database, "货物名", 数量, 价格, "类别");

// 插入车辆数据
sqlite_insert_vehicle(&database, "型号", "车牌号", 载重量);

// 插入订单数据
sqlite_insert_order(&database, "订单号", "客户", "货物", 数量, 总价);
```

### 查询数据

```c
// 查询所有仓库
sqlite_query_warehouses(&database);

// 查询所有货物
sqlite_query_goods(&database);

// 自定义查询
sqlite_query_with_callback(&database, "SELECT * FROM goods WHERE price > 1000;", NULL, NULL);
```

### 更新数据

```c
// 更新货物数量
sqlite_update_goods_quantity(&database, "货物名", 新数量);

// 更新仓库容量
sqlite_update_warehouse_capacity(&database, "仓库名", 新容量);
```

### 删除数据

```c
// 删除货物
sqlite_delete_goods(&database, "货物名");

// 删除仓库
sqlite_delete_warehouse(&database, "仓库名");
```

## 运行示例

运行 `sqlite_demo` 程序将会：

1. 创建一个名为 `warehouse_management.db` 的数据库文件
2. 创建所有必要的表结构
3. 插入示例数据
4. 演示各种查询操作
5. 演示更新和删除操作
6. 显示统计信息

## 故障排除

### 常见问题

1. **编译错误: "sqlite3.h: No such file or directory"**
   - 解决方法: 确保已安装 `mingw-w64-x86_64-sqlite3` 包

2. **链接错误: "undefined reference to `sqlite3_open`"**
   - 解决方法: 确保使用了正确的链接选项 `-lsqlite3`

3. **运行时错误: "无法打开数据库"**
   - 解决方法: 检查文件权限和路径是否正确

### 调试技巧

```bash
# 编译调试版本
make debug

# 使用 gdb 调试
gdb ./sqlite_demo
```

## 扩展功能

你可以根据需要扩展以下功能：

- 添加更多的数据表
- 实现数据备份和恢复
- 添加事务支持
- 实现数据导入/导出
- 添加索引优化查询性能
- 实现数据验证和约束

## 许可证

此项目仅供学习和参考使用。 