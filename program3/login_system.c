#include "login_system.h"
#include <sys/stat.h>
#include <sys/types.h>

// 加载用户数据
int load_users(User users[], int *user_count) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        printf("警告：用户文件不存在，将创建新文件。\n");
        *user_count = 0;
        return 0;
    }
    
    *user_count = 0;
    char line[200];
    
    while (fgets(line, sizeof(line), file) && *user_count < MAX_USERS) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;
        
        char *token = strtok(line, ";");
        if (token) {
            strcpy(users[*user_count].username, token);
            
            token = strtok(NULL, ";");
            if (token) {
                strcpy(users[*user_count].password, token);
                
                token = strtok(NULL, ";");
                if (token) {
                    strcpy(users[*user_count].status, token);
                } else {
                    strcpy(users[*user_count].status, "normal");
                }
                (*user_count)++;
            }
        }
    }
    
    fclose(file);
    return 1;
}

// 保存用户数据
int save_users(User users[], int user_count) {
    FILE *file = fopen(USERS_FILE, "w");
    if (!file) {
        printf("错误：无法保存用户数据！\n");
        return 0;
    }
    
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s;%s;%s\n", users[i].username, users[i].password, users[i].status);
    }
    
    fclose(file);
    return 1;
}

// 用户认证
int authenticate_user(const char *username, const char *password, User users[], int user_count, int *user_index) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            *user_index = i;
            return 1;
        }
    }
    return 0;
}

// 验证密码格式
int is_valid_password(const char *password) {
    int len = strlen(password);
    if (len < 8 || len > 15) {
        return 0;
    }
    
    int has_letter = 0, has_digit = 0;
    for (int i = 0; i < len; i++) {
        if (isalpha(password[i])) {
            has_letter = 1;
        }
        if (isdigit(password[i])) {
            has_digit = 1;
        }
    }
    
    return has_letter && has_digit;
}

// 生成随机密码
void generate_random_password(char *password) {
    const char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *digits = "0123456789";
    
    srand(time(NULL));
    
    // 确保至少有一个字母和一个数字
    password[0] = letters[rand() % strlen(letters)];
    password[1] = digits[rand() % strlen(digits)];
    
    // 生成剩余的字符
    for (int i = 2; i < 10; i++) {
        if (rand() % 2) {
            password[i] = letters[rand() % strlen(letters)];
        } else {
            password[i] = digits[rand() % strlen(digits)];
        }
    }
    
    password[10] = '\0';
}

// 清除输入缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 修改密码
void change_password(User users[], int user_count, int user_index) {
    char old_password[MAX_PASSWORD_LEN];
    char new_password[MAX_PASSWORD_LEN];
    char confirm_password[MAX_PASSWORD_LEN];
    
    printf("\n=== 修改密码 ===\n");
    printf("请输入当前密码: ");
    scanf("%s", old_password);
    
    if (strcmp(old_password, users[user_index].password) != 0) {
        printf("当前密码错误！\n");
        return;
    }
    
    do {
        printf("请输入新密码 (8-15位，包含字母和数字): ");
        scanf("%s", new_password);
        
        if (!is_valid_password(new_password)) {
            printf("密码格式不正确！密码必须为8-15位，包含字母和数字。\n");
            continue;
        }
        
        if (strcmp(new_password, users[user_index].password) == 0) {
            printf("新密码不能与原密码相同！\n");
            continue;
        }
        
        printf("请确认新密码: ");
        scanf("%s", confirm_password);
        
        if (strcmp(new_password, confirm_password) != 0) {
            printf("两次输入的密码不一致！\n");
            continue;
        }
        
        break;
    } while (1);
    
    strcpy(users[user_index].password, new_password);
    strcpy(users[user_index].status, "normal");
    
    if (save_users(users, user_count)) {
        printf("密码修改成功！\n");
    } else {
        printf("密码修改失败！\n");
    }
}

// 创建用户目录
void create_user_directory(const char *username) {
    char dir_path[100];
    sprintf(dir_path, "users/%s", username);
    
#ifdef _WIN32
    system("mkdir users 2>nul");
    char mkdir_cmd[150];
    sprintf(mkdir_cmd, "mkdir \"%s\" 2>nul", dir_path);
    system(mkdir_cmd);
#else
    system("mkdir -p users");
    char mkdir_cmd[150];
    sprintf(mkdir_cmd, "mkdir -p \"%s\"", dir_path);
    system(mkdir_cmd);
#endif
    
    // 创建用户专属的数据文件
    char file_path[150];
    
    sprintf(file_path, "%s/orders.dat", dir_path);
    FILE *file = fopen(file_path, "a");
    if (file) fclose(file);
    
    sprintf(file_path, "%s/vehicles.dat", dir_path);
    file = fopen(file_path, "a");
    if (file) fclose(file);
    
    sprintf(file_path, "%s/warehouses.dat", dir_path);
    file = fopen(file_path, "a");
    if (file) fclose(file);
    
    sprintf(file_path, "%s/goods.dat", dir_path);
    file = fopen(file_path, "a");
    if (file) fclose(file);
}

// 注册用户
void register_user(User users[], int *user_count) {
    if (*user_count >= MAX_USERS) {
        printf("用户数量已达上限！\n");
        return;
    }
    
    char username[MAX_USERNAME_LEN];
    printf("\n=== 注册新用户 ===\n");
    printf("请输入用户名: ");
    scanf("%s", username);
    
    // 检查用户名是否已存在
    for (int i = 0; i < *user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("用户名已存在！\n");
            return;
        }
    }
    
    // 生成随机密码
    char password[MAX_PASSWORD_LEN];
    generate_random_password(password);
    
    strcpy(users[*user_count].username, username);
    strcpy(users[*user_count].password, password);
    strcpy(users[*user_count].status, "initial");
    
    (*user_count)++;
    
    // 创建用户目录
    create_user_directory(username);
    
    if (save_users(users, *user_count)) {
        printf("用户注册成功！\n");
        printf("用户名: %s\n", username);
        printf("初始密码: %s\n", password);
        printf("注意：用户首次登录时需要修改密码。\n");
    } else {
        printf("用户注册失败！\n");
        (*user_count)--;
    }
}

// 重置用户密码
void reset_user_password(User users[], int user_count) {
    char username[MAX_USERNAME_LEN];
    printf("\n=== 重置用户密码 ===\n");
    printf("请输入要重置密码的用户名: ");
    scanf("%s", username);
    
    int user_index = -1;
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            user_index = i;
            break;
        }
    }
    
    if (user_index == -1) {
        printf("用户不存在！\n");
        return;
    }
    
    if (strcmp(users[user_index].username, "admin") == 0) {
        printf("不能重置管理员密码！\n");
        return;
    }
    
    // 生成新的随机密码
    char new_password[MAX_PASSWORD_LEN];
    generate_random_password(new_password);
    
    strcpy(users[user_index].password, new_password);
    strcpy(users[user_index].status, "initial");
    
    if (save_users(users, user_count)) {
        printf("密码重置成功！\n");
        printf("用户名: %s\n", username);
        printf("新密码: %s\n", new_password);
        printf("注意：用户使用此密码登录时需要修改密码。\n");
    } else {
        printf("密码重置失败！\n");
    }
}

// 管理员菜单
void admin_menu(User users[], int *user_count) {
    int choice;
    
    while (1) {
        printf("\n=== 管理员界面 ===\n");
        printf("1. 注册新用户\n");
        printf("2. 重置用户密码\n");
        printf("3. 查看所有用户\n");
        printf("4. 修改管理员密码\n");
        printf("5. 退出登录\n");
        printf("请选择操作: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("输入无效，请输入数字！\n");
            continue;
        }
        
        switch (choice) {
            case 1:
                register_user(users, user_count);
                break;
            case 2:
                reset_user_password(users, *user_count);
                break;
            case 3:
                printf("\n=== 用户列表 ===\n");
                for (int i = 0; i < *user_count; i++) {
                    printf("用户名: %s, 状态: %s\n", users[i].username, users[i].status);
                }
                break;
            case 4:
                change_password(users, *user_count, 0); // 假设管理员是第一个用户
                break;
            case 5:
                printf("退出登录成功！\n");
                return;
            default:
                printf("无效选择，请重新选择！\n");
        }
    }
}

// 普通用户菜单
void user_menu(User users[], int user_count, int user_index) {
    int choice;
    
    while (1) {
        printf("\n=== 用户界面 - %s ===\n", users[user_index].username);
        printf("1. 进入物流管理系统\n");
        printf("2. 修改密码\n");
        printf("3. 退出登录\n");
        printf("请选择操作: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("输入无效，请输入数字！\n");
            continue;
        }
        
        switch (choice) {
            case 1:
                printf("\n=== 物流管理系统 ===\n");
                printf("欢迎 %s 进入物流管理系统！\n", users[user_index].username);
                printf("您的数据文件位于: users/%s/\n", users[user_index].username);
                printf("这里可以集成您的物流管理功能...\n");
                break;
            case 2:
                change_password(users, user_count, user_index);
                break;
            case 3:
                printf("退出登录成功！\n");
                return;
            default:
                printf("无效选择，请重新选择！\n");
        }
    }
} 