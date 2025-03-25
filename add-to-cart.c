#include <stdio.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define MAX_ORDERS 100

struct Product {
    char name[50];
    float price;
    int stock;
    char unit[20];
};

struct Cart {
    char productName[50];
    int quantity;
    float totalPrice;
};

struct Product products[MAX_PRODUCTS];
struct Cart cart[MAX_ORDERS];
int productCount = 0, orderCount = 0;

void addProduct() {
    if (productCount >= MAX_PRODUCTS) {
        printf("? Product list is full. Cannot add more products.\n");
        return;
    }

    printf("\n?? Enter product details:\n");
    printf("Enter product name: ");
    scanf(" %[^\n]s", products[productCount].name);

    printf("Enter price (?): ");
    scanf("%f", &products[productCount].price);

    printf("Enter stock quantity: ");
    scanf("%d", &products[productCount].stock);

    printf("Enter unit (e.g., Per Kg/Per Piece/Per Dozen): ");
    scanf(" %[^\n]s", products[productCount].unit);

    printf("? Product added successfully!\n");
    productCount++;
}

void displayProducts() {
    if (productCount == 0) {
        printf("? No products available. Please add products first.\n");
        return;
    }

    printf("\n? Available Products:\n");
    printf("---------------------------------------------------\n");
    printf("Index   Name          Price (?)   Stock   Unit\n");
    printf("---------------------------------------------------\n");

    for (int i = 0; i < productCount; i++) {
        printf("%d       %-12s  ?%.2f       %d     %s\n", i + 1, products[i].name, products[i].price, products[i].stock, products[i].unit);
    }
    printf("---------------------------------------------------\n");
}

int getProductIndexByName(char name[]) {
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

void addToCart() {
    char productName[50];
    int quantity, index;

    printf("\n?? Enter product name to add to cart: ");
    scanf(" %[^\n]s", productName);

    index = getProductIndexByName(productName);

    if (index == -1) {
        printf("? Product not found. Please check the name and try again.\n");
        return;
    }

    printf("? Product found! \nName: %s | Price: ?%.2f | Unit: %s\n", 
           products[index].name, products[index].price, products[index].unit);

    printf("Enter quantity: ");
    scanf("%d", &quantity);

    if (quantity <= 0 || quantity > products[index].stock) {
        printf("?? Invalid quantity. Available stock: %d\n", products[index].stock);
        return;
    }

    strcpy(cart[orderCount].productName, products[index].name);
    cart[orderCount].quantity = quantity;
    cart[orderCount].totalPrice = quantity * products[index].price;
    orderCount++;

    products[index].stock -= quantity;

    printf("? %d %s added to cart successfully!\n", quantity, products[index].name);
}

void displayCart() {
    if (orderCount == 0) {
        printf("?? Your cart is empty.\n");
        return;
    }

    printf("\n?? Placed Orders:\n");
    printf("---------------------------------------------------\n");
    printf("Index   Product       Quantity   Total Price (?)\n");
    printf("---------------------------------------------------\n");

    for (int i = 0; i < orderCount; i++) {
        printf("%d       %-12s  %d         ?%.2f\n", i + 1, cart[i].productName, cart[i].quantity, cart[i].totalPrice);
    }
    printf("---------------------------------------------------\n");
}

void deleteOrder(int index) {
    if (index < 1 || index > orderCount) {
        printf("? Invalid order index.\n");
        return;
    }

    index--; 
    printf("? Order for %s deleted from cart.\n", cart[index].productName);

    for (int i = index; i < orderCount - 1; i++) {
        cart[i] = cart[i + 1];
    }
    orderCount--;
}

int main() {
    int choice, index;

    do {
        printf("\n?? Product and Order Management System\n");
        printf("1. Add Product\n");
        printf("2. Display Available Products\n");
        printf("3. Add to Cart (Auto-fill Price & Unit)\n");
        printf("4. View Cart (Placed Orders)\n");
        printf("5. Delete Order from Cart\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addProduct();
                break;

            case 2:
                displayProducts();
                break;

            case 3:
                addToCart();
                break;

            case 4:
                displayCart();
                break;

            case 5:
                displayCart();
                if (orderCount == 0) {
                    break;
                }
                printf("Enter order index to delete: ");
                scanf("%d", &index);
                deleteOrder(index);
                break;

            case 6:
                printf("?? Exiting... Have a nice day!\n");
                break;

            default:
                printf("?? Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}
