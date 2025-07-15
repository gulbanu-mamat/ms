#include "login_system.h"

int main() {
    User users[MAX_USERS];
    int user_count = 0;
    
    printf("=== 物流管理系统登录界面 ===\n");
    
    // 加载用户数据
    if (!load_users(users, &user_count)) {
        printf("加载用户数据时出现问题。\n");
    }
    
    // 如果没有用户，创建默认管理员账户
    if (user_count == 0) {
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "Admin123456");
        strcpy(users[0].status, "normal");
        user_count = 1;
        save_users(users, user_count);
        printf("已创建默认管理员账户 - 用户名: admin, 密码: Admin123456\n");
    }
    
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int user_index;
    
    while (1) {
        printf("\n=== 用户登录 ===\n");
        printf("用户名: ");
        scanf("%s", username);
        printf("密码: ");
        scanf("%s", password);
        
        if (authenticate_user(username, password, users, user_count, &user_index)) {
            printf("登录成功！欢迎，%s！\n", username);
            
            // 检查是否需要强制修改密码
            if (strcmp(users[user_index].status, "initial") == 0) {
                printf("\n检测到您使用初始密码登录，为了安全需要修改密码。\n");
                change_password(users, user_count, user_index);
                continue; // 修改密码后重新登录
            }
            
            // 创建用户目录（如果不存在）
            create_user_directory(username);
            
            // 根据用户类型进入不同界面
            if (strcmp(username, "admin") == 0) {
                admin_menu(users, &user_count);
            } else {
                user_menu(users, user_count, user_index);
            }
        } else {
            printf("用户名或密码错误！\n");
        }
    }
    
    return 0;
} 