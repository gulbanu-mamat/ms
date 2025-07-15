# SQLite3 链接错误解决方案

## 问题描述
在Code::Blocks中编译包含SQLite3的项目时，出现类似以下的链接错误：
```
undefined reference to `sqlite3_open'
undefined reference to `sqlite3_close'
undefined reference to `sqlite3_exec'
...
```

## 解决方案

### 方案1：手动修改Code::Blocks项目设置

1. **打开项目属性**
   - 在Code::Blocks中右键点击项目名称
   - 选择 "Build Options..."

2. **配置编译器设置**
   - 选择 "Search directories" 标签页
   - 点击 "Compiler" 子标签
   - 点击 "Add" 按钮，添加路径：`C:\msys64\mingw64\include`

3. **配置链接器设置**
   - 选择 "Search directories" 标签页
   - 点击 "Linker" 子标签
   - 点击 "Add" 按钮，添加路径：`C:\msys64\mingw64\lib`

4. **添加链接库**
   - 选择 "Linker settings" 标签页
   - 在 "Link libraries" 区域点击 "Add" 按钮
   - 添加：`sqlite3`

### 方案2：使用命令行编译

如果Code::Blocks配置有问题，可以直接使用命令行编译：

```bash
# 编译对象文件
gcc -c sqlite_db.c -o sqlite_db.o -IC:/msys64/mingw64/include
gcc -c sqlite_demo.c -o sqlite_demo.o -IC:/msys64/mingw64/include

# 链接生成可执行文件
gcc sqlite_db.o sqlite_demo.o -o sqlite_demo -LC:/msys64/mingw64/lib -lsqlite3

# 运行程序
./sqlite_demo
```

### 方案3：使用Makefile（推荐）

我已经为你创建了一个Makefile，可以直接使用：

```bash
# 打开MSYS2终端，进入项目目录
cd /d/程序设计/programs/program3

# 编译项目
make

# 运行程序
make run

# 清理编译文件
make clean
```

### 方案4：检查MSYS2路径

如果MSYS2安装在不同位置，需要修改路径：

1. **检查MSYS2安装路径**
   ```cmd
   where sqlite3
   ```

2. **根据实际路径修改配置**
   - 如果MSYS2在 `C:\msys64`，使用：
     - 头文件：`C:\msys64\mingw64\include`
     - 库文件：`C:\msys64\mingw64\lib`
   
   - 如果MSYS2在其他位置，相应调整路径

## 验证安装

运行以下命令验证SQLite3是否正确安装：

```bash
# 检查SQLite3版本
sqlite3 --version

# 检查库文件
ls C:\msys64\mingw64\lib\libsqlite3*

# 检查头文件
ls C:\msys64\mingw64\include\sqlite3.h
```

## 常见问题

### 1. 路径中包含中文字符
如果项目路径包含中文字符，可能导致编译问题。建议：
- 将项目移到英文路径下
- 或使用相对路径

### 2. 32位/64位不匹配
确保使用的是64位版本的MSYS2和SQLite3：
```bash
pacman -S mingw-w64-x86_64-sqlite3
```

### 3. Code::Blocks编译器设置
确保Code::Blocks使用的是MSYS2的GCC编译器：
- Settings → Compiler
- 选择 "GNU GCC Compiler"
- 设置正确的编译器路径

## 项目文件结构

确保项目包含以下文件：
```
program3/
├── sqlite_db.h          # SQLite数据库头文件
├── sqlite_db.c          # SQLite数据库实现
├── sqlite_demo.c        # 演示程序
├── program3.cbp         # Code::Blocks项目文件
├── Makefile            # Make编译配置
└── README_SQLite.md    # 项目说明文档
```

## 完整的编译命令示例

```bash
# 方法1：使用Makefile（推荐）
make clean && make

# 方法2：手动编译
gcc -Wall -std=c99 -c sqlite_db.c -o sqlite_db.o -IC:/msys64/mingw64/include
gcc -Wall -std=c99 -c sqlite_demo.c -o sqlite_demo.o -IC:/msys64/mingw64/include
gcc sqlite_db.o sqlite_demo.o -o sqlite_demo -LC:/msys64/mingw64/lib -lsqlite3

# 方法3：一条命令编译
gcc -Wall -std=c99 sqlite_db.c sqlite_demo.c -o sqlite_demo -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -lsqlite3
```

## 成功编译后的运行结果

如果编译成功，运行程序会看到类似输出：
```
=== SQLite 数据库演示程序 ===

1. 初始化数据库连接...
成功打开数据库: warehouse_management.db

2. 创建数据库表...
所有表创建成功

3. 插入仓库数据...
仓库 '北京仓库' 插入成功
...
```

## 如果问题仍然存在

1. **重新安装SQLite3**
   ```bash
   pacman -S mingw-w64-x86_64-sqlite3
   ```

2. **检查环境变量**
   确保MSYS2的bin目录在PATH中

3. **使用绝对路径**
   在编译命令中使用完整的绝对路径

4. **联系支持**
   如果问题仍然存在，请提供：
   - 完整的错误信息
   - MSYS2安装路径
   - 操作系统版本 