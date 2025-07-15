#include "order.h"
#include "goods.h"
#include "vehicle.h"
#include "fileio.h"
#include "ui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// createOrder��viewOrders��processOrders��completeOrders��ʵ��
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
    saveGoods();  // 商品库存也保存

    // 同时保存到SQLite数据库（简化处理，保存第一个商品的订单信息）
    if (o.itemCount > 0) {
        // 计算总价（简化处理）
        double totalPrice = 0.0;
        for (int i = 0; i < o.itemCount; i++) {
            int goodsIndex = findGoodsById(o.items[i].goodsId);
            if (goodsIndex != -1) {
                totalPrice += goodsList[goodsIndex].price * o.items[i].quantity;
            }
        }

        // 保存第一个商品的订单到数据库
        char orderId[20];
        sprintf(orderId, "ORD%d", o.id);
        int goodsIndex = findGoodsById(o.items[0].goodsId);
        if (goodsIndex != -1) {
            if (sqlite_insert_order(&g_database, orderId, o.customerName,
                                  goodsList[goodsIndex].name, o.items[0].quantity, totalPrice) != 0) {
                printf("警告：订单保存到SQLite数据库失败\n");
            }
        }
    }

    printCentered(win, 16, "Order created successfully! Press any key to return...", 60);
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
    // �ͷų���
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

