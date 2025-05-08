// --- cart_backend.c ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CART_FILE "cart.txt"
#define ORDER_FILE "orders.csv"

typedef struct CartItem {
    char username[50];
    char fruitName[50];
    float quantity;
    float price;
    struct CartItem* next;
} CartItem;

CartItem* cartList = NULL;

void loadCart() {
    FILE* file = fopen(CART_FILE, "r");
    if (!file) return;

    char username[50], fruitName[50];
    float quantity, price;

    while (fscanf(file, "%s %s %f %f", username, fruitName, &quantity, &price) == 4) {
        CartItem* newItem = (CartItem*)malloc(sizeof(CartItem));
        strcpy(newItem->username, username);
        strcpy(newItem->fruitName, fruitName);
        newItem->quantity = quantity;
        newItem->price = price;
        newItem->next = cartList;
        cartList = newItem;
    }

    fclose(file);
}

void saveCart() {
    FILE* file = fopen(CART_FILE, "w");
    if (!file) return;

    CartItem* temp = cartList;
    while (temp) {
        fprintf(file, "%s %s %.2f %.2f\n", temp->username, temp->fruitName, temp->quantity, temp->price);
        temp = temp->next;
    }

    fclose(file);
}

void generateOrderID(char* buffer) {
    sprintf(buffer, "ORD%ld", time(NULL));
}

void placeOrder(char* username) {
    CartItem *temp = cartList, *prev = NULL;
    int placed = 0;
    float total = 0;
    char itemsString[1024] = "";
    char buffer[200];

    FILE* file = fopen(ORDER_FILE, "a");
    if (!file) {
        printf("Error: Cannot write to order file.\n");
        return;
    }

    char orderID[30];
    generateOrderID(orderID);

    temp = cartList;
    while (temp) {
        if (strcmp(temp->username, username) == 0) {
            if (itemsString[0]) strcat(itemsString, "|");
            sprintf(buffer, "%s:%.0f:%.2f", temp->fruitName, temp->quantity, temp->price);
            strcat(itemsString, buffer);
            total += temp->quantity * temp->price;
            placed = 1;
        }
        temp = temp->next;
    }

    if (placed) {
        fprintf(file, "%s,%s,%s,\"%s\",%.2f\n", orderID, username, "Pending", itemsString, total);
        printf("\u2705 Order placed for %s. Order ID: %s\n", username, orderID);
    } else {
        printf("\u26A0 No items to order for %s.\n", username);
    }

    fclose(file);

    temp = cartList;
    prev = NULL;
    while (temp) {
        if (strcmp(temp->username, username) == 0) {
            if (prev) prev->next = temp->next;
            else cartList = temp->next;
            CartItem* toFree = temp;
            temp = temp->next;
            free(toFree);
        } else {
            prev = temp;
            temp = temp->next;
        }
    }

    saveCart();
}

void viewCart(char* username) {
    CartItem* temp = cartList;
    int found = 0;
    float total = 0;
    printf("\n--- Cart for %s ---\n", username);
    while (temp) {
        if (strcmp(temp->username, username) == 0) {
            printf("Fruit: %s | Quantity: %.2f | Price: Rs.%.2f\n", temp->fruitName, temp->quantity, temp->price);
            total += temp->quantity * temp->price;
            found = 1;
        }
        temp = temp->next;
    }

    if (!found) {
        printf("Cart is empty.\n");
    } else {
        printf("Total Price: Rs.%.2f\n", total);
    }
}

void removeItem(char* username, char* fruitName) {
    CartItem *temp = cartList, *prev = NULL;

    while (temp) {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->fruitName, fruitName) == 0) {
            if (prev) prev->next = temp->next;
            else cartList = temp->next;

            free(temp);
            saveCart();
            printf("\U0001F5D1 Item '%s' removed for user '%s'.\n", fruitName, username);
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    printf("\u274C Item not found.\n");
}

void updateQuantity(char* username, char* fruitName, float change) {
    CartItem* temp = cartList;
    while (temp) {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->fruitName, fruitName) == 0) {
            temp->quantity += change;
            if (temp->quantity <= 0) {
                removeItem(username, fruitName);
                printf("\U0001F5D1 Item removed due to zero or negative quantity.\n");
            } else {
                saveCart();
                printf("\U0001F501 Quantity updated successfully.\n");
            }
            return;
        }
        temp = temp->next;
    }
    printf("\u274C Item not found.\n");
}

void menu() {
    int choice;
    char username[50], fruitName[50];
    float quantityChange;

    do {
        printf("\n--- CART MENU ---\n");
        printf("1. View Cart\n");
        printf("2. Place Order\n");
        printf("3. Remove Item\n");
        printf("4. Update Quantity\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter username: ");
                fgets(username, 50, stdin);
                username[strcspn(username, "\n")] = 0;
                viewCart(username);
                break;

            case 2:
                printf("Enter username: ");
                fgets(username, 50, stdin);
                username[strcspn(username, "\n")] = 0;
                placeOrder(username);
                break;

            case 3:
                printf("Enter username: ");
                fgets(username, 50, stdin);
                username[strcspn(username, "\n")] = 0;
                printf("Enter fruit name to remove: ");
                fgets(fruitName, 50, stdin);
                fruitName[strcspn(fruitName, "\n")] = 0;
                removeItem(username, fruitName);
                break;

            case 4:
                printf("Enter username: ");
                fgets(username, 50, stdin);
                username[strcspn(username, "\n")] = 0;
                printf("Enter fruit name to update: ");
                fgets(fruitName, 50, stdin);
                fruitName[strcspn(fruitName, "\n")] = 0;
                printf("Enter quantity change (use negative to reduce): ");
                scanf("%f", &quantityChange);
                getchar();
                updateQuantity(username, fruitName, quantityChange);
                break;

            case 5:
                printf("\U0001F44B Exiting...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 5);
}

int main() {
    loadCart();
    menu();
    saveCart();
}
