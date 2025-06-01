#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <ncurses.h>

// Define maximum length constants
#define MAX_NAME_LEN 50
#define MAX_TYPE_LEN 30
#define MAX_ADDR_LEN 100
#define MAX_PHONE_LEN 20
#define MAX_TIME_LEN 30
#define MAX_PLATE_LEN 15
#define MAX_LOC_LEN 50
#define MAX_ITEMS 50
#define MAX_GOODS 100
#define MAX_VEHICLES 50
#define MAX_WAREHOUSES 20
#define MAX_ORDERS 100

// Data structures
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char type[MAX_TYPE_LEN];
    double weight;
    double volume;
    double price;
    int quantity;
} Goods;

typedef struct {
    int id;
    char plateNumber[MAX_PLATE_LEN];
    char type[MAX_TYPE_LEN];
    double maxLoad;
    double maxVolume;
    int available; // 1 means available, 0 means unavailable
    char currentLocation[MAX_LOC_LEN];
} Vehicle;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char location[MAX_LOC_LEN];
    double capacity;
    double usedCapacity;
} Warehouse;

typedef struct {
    int goodsId;
    int quantity;
} OrderItem;

typedef struct {
    int id;
    char customerName[MAX_NAME_LEN];
    char customerPhone[MAX_PHONE_LEN];
    char sourceAddress[MAX_ADDR_LEN];
    char destinationAddress[MAX_ADDR_LEN];
    char status[20]; // "Pending", "Shipping", "Completed"
    char createTime[MAX_TIME_LEN];
    char deliveryTime[MAX_TIME_LEN];
    OrderItem items[MAX_ITEMS];
    int itemCount;
    int assignedVehicle; // -1 means unassigned
} Order;

// Global data storage
Goods goodsList[MAX_GOODS];
int goodsCount = 0;
int nextGoodsId = 1;

Vehicle vehiclesList[MAX_VEHICLES];
int vehiclesCount = 0;
int nextVehicleId = 1;

Warehouse warehousesList[MAX_WAREHOUSES];
int warehousesCount = 0;
int nextWarehouseId = 1;

Order ordersList[MAX_ORDERS];
int ordersCount = 0;
int nextOrderId = 1;

// 文件保存与加载函数
void saveGoods() {
    FILE* fp = fopen("goods.dat", "wb");
    if (fp) {
        fwrite(&goodsCount, sizeof(int), 1, fp);
        fwrite(goodsList, sizeof(Goods), goodsCount, fp);
        fclose(fp);
    }
}
void loadGoods() {
    FILE* fp = fopen("goods.dat", "rb");
    if (fp) {
        fread(&goodsCount, sizeof(int), 1, fp);
        fread(goodsList, sizeof(Goods), goodsCount, fp);
        fclose(fp);
        nextGoodsId = 1;
        for (int i = 0; i < goodsCount; i++)
            if (goodsList[i].id >= nextGoodsId)
                nextGoodsId = goodsList[i].id + 1;
    }
}
void saveVehicles() {
    FILE* fp = fopen("vehicles.dat", "wb");
    if (fp) {
        fwrite(&vehiclesCount, sizeof(int), 1, fp);
        fwrite(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);
        fclose(fp);
    }
}
void loadVehicles() {
    FILE* fp = fopen("vehicles.dat", "rb");
    if (fp) {
        fread(&vehiclesCount, sizeof(int), 1, fp);
        fread(vehiclesList, sizeof(Vehicle), vehiclesCount, fp);
        fclose(fp);
        nextVehicleId = 1;
        for (int i = 0; i < vehiclesCount; i++)
            if (vehiclesList[i].id >= nextVehicleId)
                nextVehicleId = vehiclesList[i].id + 1;
    }
}
void saveWarehouses() {
    FILE* fp = fopen("warehouses.dat", "wb");
    if (fp) {
        fwrite(&warehousesCount, sizeof(int), 1, fp);
        fwrite(warehousesList, sizeof(Warehouse), warehousesCount, fp);
        fclose(fp);
    }
}
void loadWarehouses() {
    FILE* fp = fopen("warehouses.dat", "rb");
    if (fp) {
        fread(&warehousesCount, sizeof(int), 1, fp);
        fread(warehousesList, sizeof(Warehouse), warehousesCount, fp);
        fclose(fp);
        nextWarehouseId = 1;
        for (int i = 0; i < warehousesCount; i++)
            if (warehousesList[i].id >= nextWarehouseId)
                nextWarehouseId = warehousesList[i].id + 1;
    }
}
void saveOrders() {
    FILE* fp = fopen("orders.dat", "wb");
    if (fp) {
        fwrite(&ordersCount, sizeof(int), 1, fp);
        fwrite(ordersList, sizeof(Order), ordersCount, fp);
        fclose(fp);
    }
}
void loadOrders() {
    FILE* fp = fopen("orders.dat", "rb");
    if (fp) {
        fread(&ordersCount, sizeof(int), 1, fp);
        fread(ordersList, sizeof(Order), ordersCount, fp);
        fclose(fp);
        nextOrderId = 1;
        for (int i = 0; i < ordersCount; i++)
            if (ordersList[i].id >= nextOrderId)
                nextOrderId = ordersList[i].id + 1;
    }
}

// Utility functions
void getCurrentDateTime(char* buffer) {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, MAX_TIME_LEN, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// UI functions
void drawBorder(WINDOW* win) {
    box(win, 0, 0);
}

void printTitle(WINDOW* win, const char* title, int width) {
    int titleLen = strlen(title);
    mvwprintw(win, 1, (width - titleLen) / 2, "%s", title);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
}

void printCentered(WINDOW* win, int y, const char* msg, int width) {
    mvwprintw(win, y, (width - strlen(msg)) / 2, "%s", msg);
}

// Menu function with up/down navigation
int navigateMenu(WINDOW* win, char* choices[], int n_choices, int startY, int startX) {
    int highlight = 0;
    int choice = 0;
    int c;

    // Initialize item attributes
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // Display menu
    while(1) {
        for(int i = 0; i < n_choices; i++) {
            if(i == highlight)
                wattron(win, COLOR_PAIR(1));
            mvwprintw(win, startY + i, startX, "%s", choices[i]);
            if(i == highlight)
                wattroff(win, COLOR_PAIR(1));
        }

        wrefresh(win);
        c = wgetch(win);

        switch(c) {
            case KEY_UP:
                if(highlight > 0)
                    highlight--;
                break;
            case KEY_DOWN:
                if(highlight < n_choices - 1)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                return choice;
            default:
                break;
        }
    }
}

// Goods management functions
void addGoods() {
    if (goodsCount >= MAX_GOODS) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Goods storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Goods g;
    g.id = nextGoodsId++;

    WINDOW* win = newwin(22, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Add New Product", 60);

    // 显示已存在商品的id和名称
    mvwprintw(win, 3, 2, "Existing Products:");
    int showCount = goodsCount < 10 ? goodsCount : 10; // 最多显示10个
    for (int i = 0; i < showCount; i++) {
        mvwprintw(win, 4 + i, 4, "ID:%d  Name:%s", goodsList[i].id, goodsList[i].name);
    }

    int inputRow = 4 + showCount + 1;

    mvwprintw(win, inputRow, 2, "Product Name: ");
    echo();
    mvwgetnstr(win, inputRow, 16, g.name, MAX_NAME_LEN - 1);

    mvwprintw(win, inputRow + 1, 2, "Product Type: ");
    mvwgetnstr(win, inputRow + 1, 16, g.type, MAX_TYPE_LEN - 1);

    char buffer[20];
    mvwprintw(win, inputRow + 2, 2, "Weight(kg): ");
    mvwgetnstr(win, inputRow + 2, 14, buffer, 19);
    g.weight = atof(buffer);

    mvwprintw(win, inputRow + 3, 2, "Volume(m³): ");
    mvwgetnstr(win, inputRow + 3, 14, buffer, 19);
    g.volume = atof(buffer);

    mvwprintw(win, inputRow + 4, 2, "Price($): ");
    mvwgetnstr(win, inputRow + 4, 14, buffer, 19);
    g.price = atof(buffer);

    mvwprintw(win, inputRow + 5, 2, "Quantity: ");
    mvwgetnstr(win, inputRow + 5, 14, buffer, 19);
    g.quantity = atoi(buffer);
    noecho();

    goodsList[goodsCount++] = g;

    printCentered(win, inputRow + 7, "Product added successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewGoods() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Product List", 70);

    if (goodsCount == 0) {
        printCentered(win, 10, "No product records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 6, "Name");
    mvwprintw(win, startY, 22, "Type");
    mvwprintw(win, startY, 35, "Weight(kg)");
    mvwprintw(win, startY, 46, "Volume(m³)");
    mvwprintw(win, startY, 57, "Price($)");
    mvwprintw(win, startY, 68, "Qty");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < goodsCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", goodsList[i].id);
        mvwprintw(win, startY + displayCount, 6, "%.15s", goodsList[i].name);
        mvwprintw(win, startY + displayCount, 22, "%.12s", goodsList[i].type);
        mvwprintw(win, startY + displayCount, 35, "%.2f", goodsList[i].weight);
        mvwprintw(win, startY + displayCount, 46, "%.2f", goodsList[i].volume);
        mvwprintw(win, startY + displayCount, 57, "%.2f", goodsList[i].price);
        mvwprintw(win, startY + displayCount, 68, "%d", goodsList[i].quantity);
        displayCount++;
    }

    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

// Vehicle management functions
void addVehicle() {
    if (vehiclesCount >= MAX_VEHICLES) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Vehicle storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Vehicle v;
    v.id = nextVehicleId++;
    v.available = 1; // Default to available

    WINDOW* win = newwin(20, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Add New Vehicle", 60);

    mvwprintw(win, 4, 2, "License Plate: ");
    echo();
    mvwgetnstr(win, 4, 16, v.plateNumber, MAX_PLATE_LEN - 1);

    mvwprintw(win, 5, 2, "Vehicle Type: ");
    mvwgetnstr(win, 5, 16, v.type, MAX_TYPE_LEN - 1);

    char buffer[20];
    mvwprintw(win, 6, 2, "Max Load(kg): ");
    mvwgetnstr(win, 6, 17, buffer, 19);
    v.maxLoad = atof(buffer);

    mvwprintw(win, 7, 2, "Max Volume(m³): ");
    mvwgetnstr(win, 7, 17, buffer, 19);
    v.maxVolume = atof(buffer);

    mvwprintw(win, 8, 2, "Current Location: ");
    mvwgetnstr(win, 8, 20, v.currentLocation, MAX_LOC_LEN - 1);
    noecho();

    vehiclesList[vehiclesCount++] = v;
    saveVehicles();

    printCentered(win, 10, "Vehicle added successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewVehicles() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Vehicle List", 70);

    if (vehiclesCount == 0) {
        printCentered(win, 10, "No vehicle records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 5, "Plate");
    mvwprintw(win, startY, 15, "Type");
    mvwprintw(win, startY, 30, "Load(kg)");
    mvwprintw(win, startY, 41, "Volume(m³)");
    mvwprintw(win, startY, 52, "Status");
    mvwprintw(win, startY, 60, "Location");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < vehiclesCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", vehiclesList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%s", vehiclesList[i].plateNumber);
        mvwprintw(win, startY + displayCount, 15, "%.14s", vehiclesList[i].type);
        mvwprintw(win, startY + displayCount, 30, "%.2f", vehiclesList[i].maxLoad);
        mvwprintw(win, startY + displayCount, 41, "%.2f", vehiclesList[i].maxVolume);
        mvwprintw(win, startY + displayCount, 52, "%s", vehiclesList[i].available ? "Available" : "In use");
        mvwprintw(win, startY + displayCount, 60, "%.10s", vehiclesList[i].currentLocation);
        displayCount++;
    }

    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

// Warehouse management functions
void addWarehouse() {
    if (warehousesCount >= MAX_WAREHOUSES) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Warehouse storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Warehouse w;
    w.id = nextWarehouseId++;
    w.usedCapacity = 0.0;

    WINDOW* win = newwin(20, 60, 2, 10);
    drawBorder(win);
    printTitle(win, "Add New Warehouse", 60);

    mvwprintw(win, 4, 2, "Warehouse Name: ");
    echo();
    mvwgetnstr(win, 4, 18, w.name, MAX_NAME_LEN - 1);

    mvwprintw(win, 5, 2, "Location: ");
    mvwgetnstr(win, 5, 12, w.location, MAX_LOC_LEN - 1);

    char buffer[20];
    mvwprintw(win, 6, 2, "Capacity(m³): ");
    mvwgetnstr(win, 6, 16, buffer, 19);
    w.capacity = atof(buffer);
    noecho();

    warehousesList[warehousesCount++] = w;
    saveWarehouses();

    printCentered(win, 8, "Warehouse added successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewWarehouses() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Warehouse List", 70);

    if (warehousesCount == 0) {
        printCentered(win, 10, "No warehouse records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 5, "Name");
    mvwprintw(win, startY, 25, "Location");
    mvwprintw(win, startY, 45, "Capacity(m³)");
    mvwprintw(win, startY, 58, "Used(m³)");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < warehousesCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", warehousesList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%.19s", warehousesList[i].name);
        mvwprintw(win, startY + displayCount, 25, "%.19s", warehousesList[i].location);
        mvwprintw(win, startY + displayCount, 45, "%.2f", warehousesList[i].capacity);
        mvwprintw(win, startY + displayCount, 58, "%.2f", warehousesList[i].usedCapacity);
        displayCount++;
    }

    printCentered(win, 19, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

// Find goods by ID
int findGoodsById(int id) {
    for (int i = 0; i < goodsCount; i++) {
        if (goodsList[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Find vehicle by ID
int findVehicleById(int id) {
    for (int i = 0; i < vehiclesCount; i++) {
        if (vehiclesList[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Order management functions
void createOrder() {
    if (goodsCount == 0) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "No products available! Cannot create order.");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    if (ordersCount >= MAX_ORDERS) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "Order storage full, cannot add more!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    Order o;
    memset(&o, 0, sizeof(Order)); // Zero initialization
    o.id = nextOrderId++;
    strcpy(o.status, "Pending");
    getCurrentDateTime(o.createTime);
    o.assignedVehicle = -1; // Unassigned vehicle
    o.itemCount = 0;

    WINDOW* win = newwin(22, 60, 1, 10);
    drawBorder(win);
    printTitle(win, "Create New Order", 60);

    mvwprintw(win, 4, 2, "Customer Name: ");
    echo();
    mvwgetnstr(win, 4, 16, o.customerName, MAX_NAME_LEN - 1);

    mvwprintw(win, 5, 2, "Phone Number: ");
    mvwgetnstr(win, 5, 16, o.customerPhone, MAX_PHONE_LEN - 1);

    mvwprintw(win, 6, 2, "Source Address: ");
    mvwgetnstr(win, 6, 18, o.sourceAddress, MAX_ADDR_LEN - 1);

    mvwprintw(win, 7, 2, "Destination Address: ");
    mvwgetnstr(win, 7, 22, o.destinationAddress, MAX_ADDR_LEN - 1);

    // Add products to order
    mvwprintw(win, 9, 2, "Add products to order:");
    mvwprintw(win, 10, 2, "Available Products:");
    for (int i = 0; i < goodsCount && i < 10; i++) {
        mvwprintw(win, 11 + i, 4, "ID:%d  Name:%s  Qty:%d", goodsList[i].id, goodsList[i].name, goodsList[i].quantity);
    }
    int inputRow = 11 + (goodsCount < 10 ? goodsCount : 10) + 1;

    char buffer[20];
    int goodsIndex;

    while (1) {
        mvwprintw(win, inputRow, 2, "Enter Product ID (0 to finish): ");
        wclrtoeol(win);
        mvwgetnstr(win, inputRow, 33, buffer, 19);
        int goodsId = atoi(buffer);

        if (goodsId == 0) break;

        goodsIndex = findGoodsById(goodsId);
        if (goodsIndex == -1) {
            mvwprintw(win, inputRow + 1, 2, "Invalid product ID!                ");
            continue;
        }

        mvwprintw(win, inputRow + 1, 2, "Enter quantity: ");
        wclrtoeol(win);
        mvwgetnstr(win, inputRow + 1, 18, buffer, 19);
        int quantity = atoi(buffer);

        if (quantity <= 0 || quantity > goodsList[goodsIndex].quantity) {
            mvwprintw(win, inputRow + 2, 2, "Invalid quantity!                  ");
            continue;
        }

        goodsList[goodsIndex].quantity -= quantity;
        o.items[o.itemCount].goodsId = goodsId;
        o.items[o.itemCount].quantity = quantity;
        o.itemCount++;

        mvwprintw(win, inputRow + 2, 2, "Product added to order!              ");

        if (o.itemCount >= MAX_ITEMS) {
            mvwprintw(win, inputRow + 3, 2, "Maximum product limit reached!    ");
            break;
        }
    }


    noecho();

    if (o.itemCount == 0) {
        printCentered(win, 16, "Order contains no products, creation canceled.", 60);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    ordersList[ordersCount++] = o;
    saveOrders(); // 自动保存
    saveGoods();  // 商品库存也变了

    printCentered(win, 16, "Order created successfully! Press any key to return...", 60);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewOrderDetail(int orderIndex) {
    Order* order = &ordersList[orderIndex];

    WINDOW* win = newwin(22, 70, 1, 5);
    drawBorder(win);
    printTitle(win, "Order Details", 70);

    mvwprintw(win, 4, 2, "Order ID: ");
    wprintw(win, "%d", order->id);

    mvwprintw(win, 5, 2, "Customer Name: ");
    wprintw(win, "%s", order->customerName);

    mvwprintw(win, 6, 2, "Phone Number: ");
    wprintw(win, "%s", order->customerPhone);

    mvwprintw(win, 7, 2, "Source Address: ");
    wprintw(win, "%s", order->sourceAddress);

    mvwprintw(win, 8, 2, "Destination Address: ");
    wprintw(win, "%s", order->destinationAddress);

    mvwprintw(win, 9, 2, "Status: ");
    wprintw(win, "%s", order->status);

    mvwprintw(win, 10, 2, "Created: ");
    wprintw(win, "%s", order->createTime);

    mvwprintw(win, 11, 2, "Assigned Vehicle: ");
    if (order->assignedVehicle != -1) {
        int vehicleIndex = findVehicleById(order->assignedVehicle);
        if (vehicleIndex != -1) {
            wprintw(win, "%s (%s)", vehiclesList[vehicleIndex].plateNumber, vehiclesList[vehicleIndex].type);
        } else {
            wprintw(win, "Unknown vehicle ID: %d", order->assignedVehicle);
        }
    } else {
        wprintw(win, "Unassigned");
    }

    // Display products in the order
    mvwprintw(win, 13, 2, "Order Products:");
    mvwprintw(win, 14, 2, "ID");
    mvwprintw(win, 14, 7, "Name");
    mvwprintw(win, 14, 25, "Qty");
    mvwprintw(win, 14, 35, "Price");
    mvwprintw(win, 14, 45, "Total");

    double orderTotal = 0.0;
    int row = 15;
    for (int i = 0; i < order->itemCount && row < 20; i++) {
        int goodsIndex = findGoodsById(order->items[i].goodsId);
        if (goodsIndex != -1) {
            mvwprintw(win, row, 2, "%d", goodsList[goodsIndex].id);
            mvwprintw(win, row, 7, "%.17s", goodsList[goodsIndex].name);
            mvwprintw(win, row, 25, "%d", order->items[i].quantity);
            mvwprintw(win, row, 35, "%.2f", goodsList[goodsIndex].price);
            double itemTotal = goodsList[goodsIndex].price * order->items[i].quantity;
            mvwprintw(win, row, 45, "%.2f", itemTotal);
            orderTotal += itemTotal;
            row++;
        }
    }

    mvwprintw(win, row + 1, 2, "Order Total: ");
    wprintw(win, "%.2f", orderTotal);

    printCentered(win, 21, "Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void viewOrders() {
    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Order List", 70);

    if (ordersCount == 0) {
        printCentered(win, 10, "No order records! Press any key to return...", 70);
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    int startY = 4;
    mvwprintw(win, startY, 1, "ID");
    mvwprintw(win, startY, 5, "Customer");
    mvwprintw(win, startY, 20, "Status");
    mvwprintw(win, startY, 30, "Created");
    mvwprintw(win, startY, 50, "Items");
    startY += 1;

    int displayCount = 0;
    for (int i = 0; i < ordersCount && startY + displayCount < 18; i++) {
        mvwprintw(win, startY + displayCount, 1, "%d", ordersList[i].id);
        mvwprintw(win, startY + displayCount, 5, "%.14s", ordersList[i].customerName);
        mvwprintw(win, startY + displayCount, 20, "%s", ordersList[i].status);
        mvwprintw(win, startY + displayCount, 30, "%.19s", ordersList[i].createTime);
        mvwprintw(win, startY + displayCount, 50, "%d", ordersList[i].itemCount);
        displayCount++;
    }

    char buffer[20];
    mvwprintw(win, startY + displayCount + 1, 2, "Enter Order ID to view details (0 to return): ");
    echo();
    mvwgetnstr(win, startY + displayCount + 1, 46, buffer, 19);
    noecho();

    int orderId = atoi(buffer);
    if (orderId != 0) {
        for (int i = 0; i < ordersCount; i++) {
            if (ordersList[i].id == orderId) {
                viewOrderDetail(i);
                break;
            }
        }
    }

    delwin(win);
}

void processOrders() {
    // Find pending orders
    int pendingOrders[MAX_ORDERS];
    int pendingCount = 0;

    for (int i = 0; i < ordersCount; i++) {
        if (strcmp(ordersList[i].status, "Pending") == 0) {
            pendingOrders[pendingCount++] = i;
        }
    }

    if (pendingCount == 0) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "No pending orders!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    // Find available vehicles
    int availableVehicles[MAX_VEHICLES];
    int availableCount = 0;

    for (int i = 0; i < vehiclesCount; i++) {
        if (vehiclesList[i].available) {
            availableVehicles[availableCount++] = i;
        }
    }

    if (availableCount == 0) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "No available vehicles!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Process Pending Orders", 70);

    int startY = 4;
    mvwprintw(win, startY, 1, "Pending Orders:");
    mvwprintw(win, startY + 1, 1, "ID");
    mvwprintw(win, startY + 1, 6, "Customer");
    mvwprintw(win, startY + 1, 20, "Created");
    mvwprintw(win, startY + 1, 40, "Items");

    int displayCount = 0;
    for (int i = 0; i < pendingCount && i < 5; i++) { // Display up to 5 pending orders
        Order* order = &ordersList[pendingOrders[i]];
        mvwprintw(win, startY + 2 + i, 1, "%d", order->id);
        mvwprintw(win, startY + 2 + i, 6, "%.13s", order->customerName);
        mvwprintw(win, startY + 2 + i, 20, "%.19s", order->createTime);
        mvwprintw(win, startY + 2 + i, 40, "%d", order->itemCount);
        displayCount++;
    }

    startY += 2 + displayCount + 1;
    mvwprintw(win, startY, 1, "Available Vehicles:");
    mvwprintw(win, startY + 1, 1, "ID");
    mvwprintw(win, startY + 1, 6, "Plate");
    mvwprintw(win, startY + 1, 16, "Type");

    displayCount = 0;
    for (int i = 0; i < availableCount && i < 5; i++) { // Display up to 5 available vehicles
        Vehicle* vehicle = &vehiclesList[availableVehicles[i]];
        mvwprintw(win, startY + 2 + i, 1, "%d", vehicle->id);
        mvwprintw(win, startY + 2 + i, 6, "%s", vehicle->plateNumber);
        mvwprintw(win, startY + 2 + i, 16, "%.14s", vehicle->type);
        displayCount++;
    }

    char buffer[20];
    mvwprintw(win, 18, 1, "Enter Order ID: ");
    echo();
    mvwgetnstr(win, 18, 16, buffer, 19);
    int orderId = atoi(buffer);

    int orderIndex = -1;
    for (int i = 0; i < pendingCount; i++) {
        if (ordersList[pendingOrders[i]].id == orderId) {
            orderIndex = pendingOrders[i];
            break;
        }
    }

    if (orderIndex == -1) {
        mvwprintw(win, 19, 1, "Invalid Order ID! Press any key to return...");
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    mvwprintw(win, 19, 1, "Enter Vehicle ID: ");
    mvwgetnstr(win, 19, 18, buffer, 19);
    noecho();
    int vehicleId = atoi(buffer);

    int vehicleIndex = -1;
    for (int i = 0; i < availableCount; i++) {
        if (vehiclesList[availableVehicles[i]].id == vehicleId) {
            vehicleIndex = availableVehicles[i];
            break;
        }
    }

    if (vehicleIndex == -1) {
        mvwprintw(win, 19, 40, "Invalid Vehicle ID! Press any key to return...");
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // Update order and vehicle status
    strcpy(ordersList[orderIndex].status, "Shipping");
    ordersList[orderIndex].assignedVehicle = vehiclesList[vehicleIndex].id;
    vehiclesList[vehicleIndex].available = 0; // Set as unavailable
    saveOrders();
    saveVehicles();

    printCentered(win, 19, "Order has been updated to Shipping status! Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void completeOrders() {
    // Find shipping orders
    int shippingOrders[MAX_ORDERS];
    int shippingCount = 0;

    for (int i = 0; i < ordersCount; i++) {
        if (strcmp(ordersList[i].status, "Shipping") == 0) {
            shippingOrders[shippingCount++] = i;
        }
    }

    if (shippingCount == 0) {
        WINDOW* popup = newwin(5, 40, 10, 20);
        box(popup, 0, 0);
        mvwprintw(popup, 2, 5, "No orders in shipping!");
        wrefresh(popup);
        wgetch(popup);
        delwin(popup);
        return;
    }

    WINDOW* win = newwin(20, 70, 2, 5);
    drawBorder(win);
    printTitle(win, "Complete Shipping Orders", 70);

    int startY = 4;
    mvwprintw(win, startY, 1, "Shipping Orders:");
    mvwprintw(win, startY + 1, 1, "ID");
    mvwprintw(win, startY + 1, 6, "Customer");
    mvwprintw(win, startY + 1, 20, "Vehicle ID");
    mvwprintw(win, startY + 1, 30, "Created");

    for (int i = 0; i < shippingCount; i++) {
        Order* order = &ordersList[shippingOrders[i]];
        mvwprintw(win, startY + 2 + i, 1, "%d", order->id);
        mvwprintw(win, startY + 2 + i, 6, "%.13s", order->customerName);
        mvwprintw(win, startY + 2 + i, 20, "%d", order->assignedVehicle);
        mvwprintw(win, startY + 2 + i, 30, "%.19s", order->createTime);
    }

    char buffer[20];
    mvwprintw(win, 16, 1, "Enter Order ID to complete: ");
    echo();
    mvwgetnstr(win, 16, 28, buffer, 19);
    noecho();
    int orderId = atoi(buffer);

    int orderIndex = -1;
    for (int i = 0; i < shippingCount; i++) {
        if (ordersList[shippingOrders[i]].id == orderId) {
            orderIndex = shippingOrders[i];
            break;
        }
    }

    if (orderIndex == -1) {
        mvwprintw(win, 18, 1, "Invalid Order ID! Press any key to return...");
        wrefresh(win);
        wgetch(win);
        delwin(win);
        return;
    }

    // Update order status
    strcpy(ordersList[orderIndex].status, "Completed");
    getCurrentDateTime(ordersList[orderIndex].deliveryTime);
    // 释放车辆
    int vehicleId = ordersList[orderIndex].assignedVehicle;
    for (int i = 0; i < vehiclesCount; i++) {
        if (vehiclesList[i].id == vehicleId) {
            vehiclesList[i].available = 1;
            break;
        }
    }
    saveOrders();
    saveVehicles();

    printCentered(win, 18, "Order has been completed! Press any key to return...", 70);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

// Main program
void showMainMenu() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();

    // Define color pairs
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // Selected item
    init_pair(2, COLOR_WHITE, COLOR_BLUE);  // Title

    int running = 1;
    while (running) {
        clear();

        attron(COLOR_PAIR(2));
        mvprintw(1, 30, "LOGISTICS MANAGEMENT SYSTEM");
        attroff(COLOR_PAIR(2));

        char* main_menu[] = {
            "1. Product Management",
            "2. Vehicle Management",
            "3. Warehouse Management",
            "4. Order Management",
            "5. Exit System"
        };

        mvprintw(10, 10, "Use UP/DOWN arrows to navigate and ENTER to select");

        // Display and handle main menu
        int choice = navigateMenu(stdscr, main_menu, 5, 3, 10);

        switch (choice) {
            case 0: { // Product Management
                clear();
                char* sub_menu[] = {
                    "1. Add Product",
                    "2. View Products",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addGoods();
                        break;
                    case 1:
                        viewGoods();
                        break;
                    case 2:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 1: { // Vehicle Management
                clear();
                char* sub_menu[] = {
                    "1. Add Vehicle",
                    "2. View Vehicles",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addVehicle();
                        break;
                    case 1:
                        viewVehicles();
                        break;
                    case 2:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 2: { // Warehouse Management
                clear();
                char* sub_menu[] = {
                    "1. Add Warehouse",
                    "2. View Warehouses",
                    "3. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 3, 3, 10);

                switch (subChoice) {
                    case 0:
                        addWarehouse();
                        break;
                    case 1:
                        viewWarehouses();
                        break;
                    case 2:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 3: { // Order Management
                clear();
                char* sub_menu[] = {
                    "1. Create Order",
                    "2. View Orders",
                    "3. Process Orders",
                    "4. Complete Orders",
                    "5. Back to Main Menu"
                };
                int subChoice = navigateMenu(stdscr, sub_menu, 5, 3, 10);

                switch (subChoice) {
                    case 0:
                        createOrder();
                        break;
                    case 1:
                        viewOrders();
                        break;
                    case 2:
                        processOrders();
                        break;
                    case 3:
                        completeOrders();
                        break;
                    case 4:
                        // Return to main menu
                        break;
                }
                break;
            }
            case 4: // Exit System
                running = 0;
                break;
        }
    }

    endwin();
}

void showStartupScreen() {
    // 初始化ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0); // 隐藏光标
    start_color(); // 启用颜色

    // 定义颜色对
    init_pair(1, COLOR_WHITE, COLOR_BLUE);    // 标题
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // 进度条
    init_pair(3, COLOR_WHITE, COLOR_BLACK);   // 文本
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // 加载框

    // 获取窗口尺寸
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // 显示标题
    attron(COLOR_PAIR(1));
    const char* title = "LOGISTICS MANAGEMENT SYSTEM";
    mvprintw(rows / 2 - 5, (cols - strlen(title)) / 2, "%s", title);
    attroff(COLOR_PAIR(1));

    // 绘制加载框
    int box_width = 60;
    int box_height = 10;
    int box_y = rows / 2 - box_height / 2;
    int box_x = (cols - box_width) / 2;

    attron(COLOR_PAIR(4));
    // 绘制边框
    mvprintw(box_y, box_x, "+");
    mvprintw(box_y, box_x + box_width, "+");
    mvprintw(box_y + box_height, box_x, "+");
    mvprintw(box_y + box_height, box_x + box_width, "+");

    // 绘制横线
    for (int i = 1; i < box_width; i++) {
        mvprintw(box_y, box_x + i, "-");
        mvprintw(box_y + box_height, box_x + i, "-");
    }

    // 绘制竖线
    for (int i = 1; i < box_height; i++) {
        mvprintw(box_y + i, box_x, "|");
        mvprintw(box_y + i, box_x + box_width, "|");
    }

    // 绘制标题
    const char* loading_text = "LOADING...";
    mvprintw(box_y + 2, (cols - strlen(loading_text)) / 2, "%s", loading_text);

    // 绘制进度条区域
    mvprintw(box_y + 4, box_x + 2, "[");
    mvprintw(box_y + 4, box_x + box_width - 2, "]");
    attroff(COLOR_PAIR(4));

    // 进度条参数
    int bar_width = box_width - 6;
    int bar_y = box_y + 4;
    int bar_x = box_x + 3;

    // 显示初始进度条（空）
    attron(COLOR_PAIR(3));
    for (int i = 0; i < bar_width; i++) {
        mvprintw(bar_y, bar_x + i, " ");
    }
    attroff(COLOR_PAIR(3));
    refresh();

    // 模拟加载过程
    for (int i = 0; i <= bar_width; i++) {
        // 更新进度条
        attron(COLOR_PAIR(2));
        for (int j = 0; j < i; j++) {
            mvprintw(bar_y, bar_x + j, "=");
        }
        attroff(COLOR_PAIR(2));

        // 显示百分比
        int percent = (i * 100) / bar_width;
        attron(COLOR_PAIR(3));
        mvprintw(box_y + 6, (cols - 4) / 2, "%3d%%", percent);
        attroff(COLOR_PAIR(3));

        refresh();
        napms(50); // 延迟50毫秒
    }

    // 显示完成消息
    const char* ready_text = "SYSTEM READY!";
    attron(COLOR_PAIR(2));
    mvprintw(box_y + 8, (cols - strlen(ready_text)) / 2, "%s", ready_text);
    attroff(COLOR_PAIR(2));
    refresh();

    // 等待用户按键
    napms(1000); // 等待1秒
    getch();

    // 清屏准备进入主程序
    clear();
    refresh();
}

int main() {
    loadGoods();
    loadVehicles();
    loadWarehouses();
    loadOrders();
    showStartupScreen();
    showMainMenu();
    // 退出前再次保存
    saveGoods();
    saveVehicles();
    saveWarehouses();
    saveOrders();
    return 0;
}
