#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define MAX_ORDERS 100
#define PRODUCT_FILE "fruits.txt"
#define ORDER_FILE "orders.csv"

// Product structure
typedef struct {
    char name[100];
    float price;
    char unit[20];
    int stock;
    char description[300];
    char imageURL[300];
} Product;

// Order structure
typedef struct {
    char orderID[20];
    char customerName[100];
    char status[20];
    char items[1000]; // Serialized items
    float totalAmount;
    char customerAddress[200];
} Order;

// Order queue globals
Order orderQueue[MAX_ORDERS];
int front = 0, rear = -1;

// Enqueue an order
void enqueueOrder(Order o) {
    if (rear == MAX_ORDERS - 1) {
        printf("Queue is full!\n");
        return;
    }
    orderQueue[++rear] = o;
}

// Dequeue an order
Order dequeueOrder() {
    Order emptyOrder;
    if (front > rear) {
        printf("Queue is empty!\n");
        strcpy(emptyOrder.customerName, "");
        return emptyOrder;
    }
    return orderQueue[front++];
}

Product products[MAX_PRODUCTS];
int productCount = 0;

void loadProducts() {
    FILE* file = fopen(PRODUCT_FILE, "r");
    if (!file) {
        printf("Could not open product file.\n");
        return;
    }
    productCount = 0;
    while (fscanf(file, "%s %f %d %s", products[productCount].name, &products[productCount].price, &products[productCount].stock, products[productCount].unit) == 4) {
        strcpy(products[productCount].description, "Delicious and fresh fruit.");
        strcpy(products[productCount].imageURL, "N/A");
        productCount++;
    }
    fclose(file);
}

void saveProducts() {
    FILE* file = fopen(PRODUCT_FILE, "w");
    if (!file) {
        printf("Could not write to product file.\n");
        return;
    }
    for (int i = 0; i < productCount; i++) {
        fprintf(file, "%s %.2f %d %s\n", products[i].name, products[i].price, products[i].stock, products[i].unit);
    }
    fclose(file);
}

void loadOrders() {
    FILE* file = fopen(ORDER_FILE, "r");
    if (!file) {
        printf("Could not open orders file.\n");
        return;
    }

    char line[1024];
    front = 0;
    rear = -1;
    
    while (fgets(line, sizeof(line), file)) {
        Order o = {0};
        
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
        
        // Items - This is a bit tricky as items might contain commas inside quotes
        token = strtok(NULL, ",");
        if (token && token[0] == '"') {
            // If starts with a quote, we need to capture until the closing quote
            char items[1000] = {0};
            strcpy(items, token + 1); // Skip the opening quote
            
            // If this token doesn't end with a quote, keep adding tokens until we find one that does
            while (token && items[strlen(items)-1] != '"') {
                token = strtok(NULL, ",");
                if (token) {
                    strcat(items, ",");
                    strcat(items, token);
                }
            }
            
            // Remove the closing quote if present
            if (items[strlen(items)-1] == '"')
                items[strlen(items)-1] = '\0';
                
            strcpy(o.items, items);
            
            // Get the total amount
            token = strtok(NULL, ",\r\n");
        } else {
            // No quotes, just take the token as is
            if (token) strcpy(o.items, token);
            
            // Get the total amount
            token = strtok(NULL, ",\r\n");
        }
        
        // Total Amount
        if (token) o.totalAmount = atof(token);
        else o.totalAmount = 0;
        
        // Set a default address since it's not in the CSV
        strcpy(o.customerAddress, "Address-Not-Found");
        
        enqueueOrder(o);
    }
    fclose(file);
}

void saveOrders() {
    FILE* file = fopen(ORDER_FILE, "w");
    if (!file) return;

    for (int i = front; i <= rear; i++) {
        Order o = orderQueue[i];
        // Check if items contain commas and need quotes
        if (strchr(o.items, ',')) {
            fprintf(file, "%s,%s,%s,\"%s\",%.2f\n", 
                o.orderID, o.customerName, o.status, o.items, o.totalAmount);
        } else {
            fprintf(file, "%s,%s,%s,%s,%.2f\n", 
                o.orderID, o.customerName, o.status, o.items, o.totalAmount);
        }
    }
    fclose(file);
}

void viewAllProducts() {
    printf("\n--- Product List ---\n");
    for (int i = 0; i < productCount; i++) {
        printf("%d. %s - Rs. %.2f (%s), Stock: %d\n", i + 1, products[i].name, products[i].price, products[i].unit, products[i].stock);
    }
}

void addProduct() {
    if (productCount >= MAX_PRODUCTS) {
        printf("Product list full.\n");
        return;
    }
    Product p;
    printf("Enter name: "); scanf("%s", p.name);
    printf("Enter price: "); scanf("%f", &p.price);
    printf("Enter stock: "); scanf("%d", &p.stock);
    printf("Enter unit: "); scanf("%s", p.unit);
    printf("Enter description: "); getchar(); fgets(p.description, 300, stdin);
    p.description[strcspn(p.description, "\n")] = 0;
    printf("Enter image URL: "); fgets(p.imageURL, 300, stdin);
    p.imageURL[strcspn(p.imageURL, "\n")] = 0;
    products[productCount++] = p;
    saveProducts();
    printf("✅ Product added.\n");
}

void viewOrders() {
    printf("\n--- Orders ---\n");
    for (int i = front; i <= rear; i++) {
        Order o = orderQueue[i];
        printf("Order %d: %s | Customer: %s | Status: %s | Items: %s | Total: Rs. %.2f\n",
            i + 1, o.orderID, o.customerName, o.status, o.items, o.totalAmount);
    }
}

void updateOrderStatus() {
    int index;
    char newStatus[20];
    printf("Enter order number to update: "); scanf("%d", &index);
    if (index < 1 || index > rear - front + 1) {
        printf("Invalid order number.\n");
        return;
    }
    printf("Enter new status (Accepted/Packed/Shipped/Delivered): ");
    scanf("%s", newStatus);
    strcpy(orderQueue[front + index - 1].status, newStatus);
    saveOrders();
    printf("✅ Order status updated.\n");
}

void menu() {
    int choice;
    loadProducts();
    loadOrders();
    do {
        printf("\n--- Admin Menu ---\n");
        printf("1. View Products\n2. Add Product\n3. View Orders\n4. Update Order Status\n5. Exit\nChoice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: viewAllProducts(); break;
            case 2: addProduct(); break;
            case 3: viewOrders(); break;
            case 4: updateOrderStatus(); break;
            case 5: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

int main() {
    menu();}