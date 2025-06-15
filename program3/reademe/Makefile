# Makefile for SQLite Database Project
# 适用于 MSYS2 环境

# 编译器设置
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# SQLite 库设置 (MSYS2)
SQLITE_CFLAGS = $(shell pkg-config --cflags sqlite3)
SQLITE_LIBS = $(shell pkg-config --libs sqlite3)

# 如果 pkg-config 不可用，使用默认设置
ifeq ($(SQLITE_CFLAGS),)
    SQLITE_CFLAGS = -I/mingw64/include
endif

ifeq ($(SQLITE_LIBS),)
    SQLITE_LIBS = -L/mingw64/lib -lsqlite3
endif

# 目标文件
TARGET = sqlite_demo
SQLITE_DEMO_TARGET = sqlite_demo

# 源文件
SOURCES = sqlite_db.c sqlite_demo.c
OBJECTS = $(SOURCES:.c=.o)

# 默认目标
all: $(TARGET)

# 编译主程序
$(TARGET): $(OBJECTS)
	@echo "正在链接程序..."
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIBS)
	@echo "编译完成: $@"



# 编译目标文件
%.o: %.c sqlite_db.h
	@echo "正在编译: $<"
	$(CC) $(CFLAGS) $(SQLITE_CFLAGS) -c $< -o $@

# 清理编译文件
clean:
	@echo "清理编译文件..."
	rm -f $(OBJECTS) $(TARGET) *.exe *.db
	@echo "清理完成"

# 运行程序
run: $(TARGET)
	@echo "运行程序..."
	./$(TARGET)

# 安装 SQLite3 (for MSYS2)
install-sqlite:
	@echo "安装 SQLite3..."
	pacman -S mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-pkg-config

# 检查 SQLite 安装
check-sqlite:
	@echo "检查 SQLite3 安装状态..."
	@sqlite3 --version || echo "SQLite3 未安装，请运行 'make install-sqlite'"
	@pkg-config --modversion sqlite3 || echo "pkg-config 未找到 sqlite3"

# 显示帮助信息
help:
	@echo "可用的目标:"
	@echo "  all            - 编译程序"
	@echo "  clean          - 清理编译文件"
	@echo "  run            - 编译并运行程序"
	@echo "  install-sqlite - 安装 SQLite3 (需要管理员权限)"
	@echo "  check-sqlite   - 检查 SQLite3 安装状态"
	@echo "  help           - 显示此帮助信息"

# 调试版本
debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

# 发布版本
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

.PHONY: all clean run install-sqlite check-sqlite help debug release 