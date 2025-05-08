// --- orders_backend.c ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 10
#define MAX_ORDERS 100
#define ORDER_FILE "orders.csv"

typedef struct Item {
    char name[100];
    int quantity;
    float price;
    char unit[20];
} Item;

typedef struct Order {
    char orderID[20];    // Changed to char array to preserve the full order ID
    char customerName[100];
    char status[100];
    float totalAmount;
    Item items[MAX_ITEMS];
    int itemCount;
    char date[50];
} Order;

typedef struct OrderQueue {
    Order orders[MAX_ORDERS];
    int front;
    int rear;
} OrderQueue;

OrderQueue queue = {.front = 0, .rear = -1};  // Modified initialization to match admin code

void trimNewline(char* str) {
    str[strcspn(str, "\n")] = 0;
    str[strcspn(str, "\r")] = 0;
}

void enqueue(Order order) {
    if (queue.rear == MAX_ORDERS - 1) {
        printf("⚠ Order queue full.\n");
        return;
    }
    queue.orders[++queue.rear] = order;
}

void parseOrderLine(char* line) {
    Order o = {0};
    char itemString[1024];
    
    // Make a copy of the line for processing
    char lineCopy[1024];
    strcpy(lineCopy, line);
    
    // Remove newline if present
    lineCopy[strcspn(lineCopy, "\r\n")] = 0;
    
    char* token;
    
    // Order ID
    token = strtok(lineCopy, ",");
    if (token) strcpy(o.orderID, token);
    
    // Customer Name
    token = strtok(NULL, ",");
    if (token) strcpy(o.customerName, token);
    
    // Status 
    token = strtok(NULL, ",");
    if (token) strcpy(o.status, token);
    
    // Items 
    token = strtok(NULL, ",");
    if (token && token[0] == '"') {
       
        strcpy(itemString, token + 1); // Skip the opening quote
        
        
        while (token && itemString[strlen(itemString)-1] != '"') {
            token = strtok(NULL, ",");
            if (token) {
                strcat(itemString, ",");
                strcat(itemString, token);
            }
        }
        
        
        if (itemString[strlen(itemString)-1] == '"')
            itemString[strlen(itemString)-1] = '\0';
    } else {
         
        if (token) strcpy(itemString, token);
    }
    
    // Total Amount
    token = strtok(NULL, ",\r\n");
    if (token) o.totalAmount = atof(token);
    else o.totalAmount = 0;
    
    // Parse the items
    o.itemCount = 0;
    token = strtok(itemString, "|");
    while (token && o.itemCount < MAX_ITEMS) {
        char name[100];
        int quantity;
        float price;
        
        if (sscanf(token, "%[^:]:%d:%f", name, &quantity, &price) == 3) {
            strcpy(o.items[o.itemCount].name, name);
            o.items[o.itemCount].quantity = quantity;
            o.items[o.itemCount].price = price;
            strcpy(o.items[o.itemCount].unit, "unit");
            o.itemCount++;
        }
        
        token = strtok(NULL, "|");
    }
    
    strcpy(o.date, "Unknown");
    enqueue(o);
}

void loadOrdersFromFile() {
    FILE* file = fopen(ORDER_FILE, "r");
    if (!file) {
        printf("❌ Cannot open orders.csv\n");
        return;
    }

    // Reset the queue
    queue.front = 0;
    queue.rear = -1;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        parseOrderLine(line);
    }

    fclose(file);
}

void saveOrdersToFile() {
    FILE* file = fopen(ORDER_FILE, "w");
    if (!file) {
        printf("❌ Cannot write to orders.csv\n");
        return;
    }

    for (int i = queue.front; i <= queue.rear; i++) {
        Order o = queue.orders[i];
        fprintf(file, "%s,%s,%s,\"", o.orderID, o.customerName, o.status);
        for (int j = 0; j < o.itemCount; j++) {
            fprintf(file, "%s:%d:%.2f", o.items[j].name, o.items[j].quantity, o.items[j].price);
            if (j < o.itemCount - 1) fprintf(file, "|");
        }
        fprintf(file, "\",%.2f\n", o.totalAmount);
    }

    fclose(file);
}

void viewMyOrders(const char* username) {
    int found = 0;
    printf("\n📦 Orders for %s:\n", username);
    for (int i = queue.front; i <= queue.rear; i++) {
        if (strcmp(queue.orders[i].customerName, username) == 0) {
            Order o = queue.orders[i];
            printf("\nOrder ID: %s\nStatus: %s\nItems:\n", o.orderID, o.status);
            for (int j = 0; j < o.itemCount; j++) {
                printf("- %s x%d = Rs. %.2f\n", o.items[j].name, o.items[j].quantity, 
                       o.items[j].quantity * o.items[j].price);
            }
            printf("Total: Rs. %.2f\n", o.totalAmount);
            found = 1;
        }
    }
    if (!found) printf("❌ No orders found.\n");
}

void updateOrderStatus(const char* orderID, const char* newStatus) {
    for (int i = queue.front; i <= queue.rear; i++) {
        if (strcmp(queue.orders[i].orderID, orderID) == 0) {
            strcpy(queue.orders[i].status, newStatus);
            printf("✅ Order %s status updated to '%s'\n", orderID, newStatus);
            saveOrdersToFile();
            return;
        }
    }
    printf("❌ Order not found.\n");
}

void cancelOrder(const char* orderID) {
    updateOrderStatus(orderID, "Cancelled");
}

void menu() {
    char username[100];
    int choice;
    char orderID[20], status[50];

    // Always reload from file to get the latest status updates from admin
    loadOrdersFromFile();

    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    trimNewline(username);

    do {
        // Reload orders each time to get the latest updates
        loadOrdersFromFile();
        
        printf("\n--- MY ORDERS MENU ---\n");
        printf("1. View My Orders\n");
        printf("2. Update Order Status\n");
        printf("3. Cancel Order\n");
        printf("4. Exit\nChoice: ");
        scanf("%d", &choice); getchar();

        if (choice == 1) {
            viewMyOrders(username);
        } else if (choice == 2) {
            printf("Enter Order ID (e.g., ORD1745949668): ");
            fgets(orderID, sizeof(orderID), stdin);
            trimNewline(orderID);
            printf("Enter new status (Pending/Accepted/Packed/Shipped/Delivered): ");
            fgets(status, 50, stdin);
            trimNewline(status);
            updateOrderStatus(orderID, status);
        } else if (choice == 3) {
            printf("Enter Order ID to cancel: ");
            fgets(orderID, sizeof(orderID), stdin);
            trimNewline(orderID);
            cancelOrder(orderID);
        }

    } while (choice != 4);
}

int main() {
    menu();
    return 0;
}

