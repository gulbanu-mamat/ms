# Authentication System Integration Guide

## Overview

This guide explains how to integrate the new authentication system with your existing C++ warehouse management project.

## Files Created/Modified

### New Files:
- `auth_system.h` - Authentication system header file
- `auth_system.cpp` - Authentication system implementation
- `main_with_auth.cpp` - Integrated main program with authentication
- `Makefile_integrated_auth` - Makefile for the integrated system
- `users.txt` - User data storage file (created automatically)

### Key Features:
- ✅ **C++ Compatible** - Written in C++ to match your project
- ✅ **ncurses Integration** - Uses ncurses library like your existing UI
- ✅ **English Interface** - All prompts and messages in English
- ✅ **User Data Isolation** - Each user has separate data directories
- ✅ **Password Security** - Strong password requirements and validation
- ✅ **Admin Panel** - Complete user management functionality

## Integration Steps

### Step 1: Compile the Integrated System

```bash
# Use the integrated Makefile
g++ -Wall -Wextra -std=c++11 -o warehouse_system_with_auth main_with_auth.cpp auth_system.cpp library.cpp ui.cpp fileio.cpp goods.cpp vehicle.cpp warehouse.cpp order.cpp sqlite_db.cpp -lncurses -lsqlite3
```

Or use the Makefile:
```bash
make -f Makefile_integrated_auth
```

### Step 2: Run the System

```bash
./warehouse_system_with_auth
```

### Step 3: Login with Default Admin Account

- **Username**: `admin`
- **Password**: `Admin123456`

## System Flow

```
1. Welcome Screen
2. Login Screen
3. Authentication Check
4. If Admin → Admin Panel
   - Register Users
   - Reset Passwords
   - View Users
   - Enter Main System
5. If User → User Menu
   - Enter Warehouse System
   - Change Password
6. Main Warehouse System (Your Existing Code)
7. Return to Auth System or Logout
```

## User Data Structure

```
project_root/
├── users/
│   ├── admin/
│   │   ├── orders.dat
│   │   ├── vehicles.dat
│   │   ├── warehouses.dat
│   │   └── goods.dat
│   └── username/
│       ├── orders.dat
│       ├── vehicles.dat
│       ├── warehouses.dat
│       └── goods.dat
├── users.txt (user credentials)
└── warehouse_system_with_auth.exe
```

## Password Requirements

- 8-15 characters long
- Must contain both letters and numbers
- Cannot be the same as the current password
- Initial passwords force change on first login

## Admin Functions

1. **Register New User**
   - Creates new user with random initial password
   - User must change password on first login
   - Creates user-specific data directory

2. **Reset User Password**
   - Generates new random password
   - Marks as "initial" status requiring password change
   - Cannot reset admin password

3. **View All Users**
   - Shows all registered users and their status

4. **Change Admin Password**
   - Admin can change their own password

## User Functions

1. **Enter Warehouse System**
   - Seamlessly integrates with your existing system
   - User-specific data isolation

2. **Change Password**
   - Users can change their password anytime
   - Must meet security requirements

## Customization Options

### 1. Modify User Data Path Handling

In `main_with_auth.cpp`, modify the `setUserDataPaths()` function to integrate with your file I/O functions:

```cpp
void setUserDataPaths(const char* username) {
    // Set global variables or modify your file paths
    char userPath[200];
    sprintf(userPath, "users/%s", username);
    
    // Example: Set environment variable
    setenv("USER_DATA_PATH", userPath, 1);
    
    // Or modify global path variables in your existing code
}
```

### 2. Integrate with Existing File I/O

You can modify your existing `loadGoods()`, `saveGoods()`, etc. functions to use user-specific paths:

```cpp
// In your existing functions, replace hardcoded paths like:
// FILE *file = fopen("goods.dat", "r");

// With user-specific paths:
char filepath[200];
sprintf(filepath, "users/%s/goods.dat", currentUser.c_str());
FILE *file = fopen(filepath, "r");
```

### 3. Database Integration

The system can use separate SQLite databases for each user:

```cpp
char dbPath[200];
sprintf(dbPath, "users/%s/warehouse.db", username);
sqlite_db_init(&g_database, dbPath);
```

## Testing the System

1. **Test Admin Login**:
   - Login as admin
   - Create a test user
   - Reset user password

2. **Test User Login**:
   - Login with test user
   - Change initial password
   - Enter warehouse system

3. **Test Data Isolation**:
   - Create data as user1
   - Login as user2
   - Verify user2 cannot see user1's data

## Troubleshooting

### Common Issues:

1. **Compilation Errors**:
   - Ensure ncurses library is installed
   - Check that all source files are present

2. **Login Issues**:
   - Verify `users.txt` file is created
   - Check file permissions

3. **ncurses Display Issues**:
   - Ensure terminal supports ncurses
   - Try resizing terminal window

### Debug Mode:

Add debug prints in `auth_system.cpp` to trace authentication flow:

```cpp
printf("DEBUG: User count: %d\n", userCount);
printf("DEBUG: Login attempt: %s\n", username);
```

## Security Considerations

1. **Password Storage**: Currently stored in plain text. For production, consider:
   - Password hashing (bcrypt, scrypt)
   - Salt generation
   - Secure file permissions

2. **User Data**: Implement proper file permissions for user directories

3. **Session Management**: Consider adding session timeouts

## Future Enhancements

1. **Role-based Access Control**: Different permission levels
2. **Audit Logging**: Track user activities
3. **Password Expiration**: Force periodic password changes
4. **Account Lockout**: Prevent brute force attacks
5. **Two-Factor Authentication**: Enhanced security

## Support

If you encounter issues:
1. Check the compilation output for errors
2. Verify all dependencies are installed
3. Test with the default admin account first
4. Review the integration guide steps 