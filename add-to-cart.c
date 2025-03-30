#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Product {
    char name[50];
    float price;
    int stock;
    char unit[20];
    struct Product* next; 
} Product;

typedef struct CartItem {
    char productName[50];
    int quantity;
    float totalPrice;
    struct CartItem* next; 
} CartItem;

Product* productList = NULL;
CartItem* cartList = NULL;

Product* createProduct(char* name, float price, int stock, char* unit) {
    Product* newProduct = (Product*)malloc(sizeof(Product));
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->stock = stock;
    strcpy(newProduct->unit, unit);
    newProduct->next = NULL;
    return newProduct;
}

void addProduct() {
    char name[50], unit[20];
    float price;
    int stock;

    printf("\nEnter product details:\n");
    printf("Enter product name: ");
    scanf(" %[^\n]s", name);
    printf("Enter price: ");
    scanf("%f", &price);
    printf("Enter stock quantity: ");
    scanf("%d", &stock);
    printf("Enter unit (e.g., Per Kg/Per Piece): ");
    scanf(" %[^\n]s", unit);

    Product* newProduct = createProduct(name, price, stock, unit);
    newProduct->next = productList;
    productList = newProduct;

    printf("Product added successfully!\n");
}

void displayProducts() {
    if (!productList) {
        printf("No products available. Please add products first.\n");
        return;
    }

    printf("\nAvailable Products:\n");
    printf("---------------------------------------------------\n");
    printf("Name          Price   Stock   Unit\n");
    printf("---------------------------------------------------\n");

    Product* current = productList;
    while (current) {
        printf("%-12s  %.2f   %d     %s\n", current->name, current->price, current->stock, current->unit);
        current = current->next;
    }
    printf("---------------------------------------------------\n");
}

Product* findProductByName(char* name) {
    Product* current = productList;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

CartItem* createCartItem(char* productName, int quantity, float totalPrice) {
    CartItem* newItem = (CartItem*)malloc(sizeof(CartItem));
    strcpy(newItem->productName, productName);
    newItem->quantity = quantity;
    newItem->totalPrice = totalPrice;
    newItem->next = NULL;
    return newItem;
}


void addToCart() {
    char productName[50];
    int quantity;

    printf("\nEnter product name to add to cart: ");
    scanf(" %[^\n]s", productName);

    Product* product = findProductByName(productName);
    if (!product) {
        printf("Product not found. Please check the name and try again.\n");
        return;
    }

    printf("Enter quantity: ");
    scanf("%d", &quantity);

    if (quantity <= 0 || quantity > product->stock) {
        printf("Invalid quantity. Available stock: %d\n", product->stock);
        return;
    }

    CartItem* newItem = createCartItem(product->name, quantity, quantity * product->price);
    newItem->next = cartList;
    cartList = newItem;

    product->stock -= quantity;

    printf("%d %s added to cart successfully!\n", quantity, product->name);
}

void displayCart() {
    if (!cartList) {
        printf("Your cart is empty.\n");
        return;
    }

    printf("\nPlaced Orders:\n");
    printf("---------------------------------------------------\n");
    printf("Product       Quantity   Total Price\n");
    printf("---------------------------------------------------\n");

    CartItem* current = cartList;
    while (current) {
        printf("%-12s  %d         %.2f\n", current->productName, current->quantity, current->totalPrice);
        current = current->next;
    }
    printf("---------------------------------------------------\n");
}

void deleteOrder() {
    char productName[50];
    printf("\nEnter product name to delete from cart: ");
    scanf(" %[^\n]s", productName);

    CartItem *current = cartList, *prev = NULL;
    while (current) {
        if (strcmp(current->productName, productName) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                cartList = current->next;
            }
            free(current);
            printf("Order for %s deleted from cart.\n", productName);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Order not found in cart.\n");
}

int main() {
    int choice;

    do {
        printf("\nProduct and Order Management System\n");
        printf("1. Add Product\n");
        printf("2. Display Available Products\n");
        printf("3. Add to Cart\n");
        printf("4. View Cart\n");
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
                deleteOrder();
                break;
            case 6:
                printf("Exiting... Have a nice day!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}
