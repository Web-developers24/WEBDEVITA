#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRUITS_FILE "fruits.txt"
#define USERS_FILE "users.txt"
#define CART_FILE "cart.txt"

typedef struct Fruit {
    char name[50];
    float price;
    char unit[20];
    struct Fruit* next;
} Fruit;

typedef struct User {
    char username[50];
    char password[50];
    struct User* next;
} User;

typedef struct CartItem {
    char username[50];
    char fruitName[50];
    float quantity;
    float price;
    struct CartItem* next;
} CartItem;

Fruit* fruitList = NULL;
User* userList = NULL;
CartItem* cartList = NULL;

void loadFruits();
void saveFruits();
void loadUsers();
void saveUsers();
void loadCart();
void saveCart();
void addFruit(char* name, float price, char* unit);
void addUser(char* username, char* password);
void addToCart(char* username, char* fruitName, float quantity);
void viewCart(char* username);
void displayFruits();
void displayUsers();

int main() {
    loadFruits();
    loadUsers();
    loadCart();

    int choice;
    char username[50], password[50], fruitName[50], unit[20];
    float price, quantity;

    while (1) {
        printf("\n=== Backend Management System ===\n");
        printf("1. Add Fruit\n");
        printf("2. Add User\n");
        printf("3. Add to Cart\n");
        printf("4. View Fruits\n");
        printf("5. View Users\n");
        printf("6. View Cart\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter fruit name: ");
                scanf(" %[^\n]s", fruitName);
                printf("Enter price: ");
                scanf("%f", &price);
                printf("Enter unit (e.g., kg, piece): ");
                scanf(" %[^\n]s", unit);
                addFruit(fruitName, price, unit);
                saveFruits();
                break;
            case 2:
                printf("Enter username: ");
                scanf(" %[^\n]s", username);
                printf("Enter password: ");
                scanf(" %[^\n]s", password);
                addUser(username, password);
                saveUsers();
                break;
            case 3:
                printf("Enter username: ");
                scanf(" %[^\n]s", username);
                printf("Enter fruit name: ");
                scanf(" %[^\n]s", fruitName);
                printf("Enter quantity: ");
                scanf("%f", &quantity);
                addToCart(username, fruitName, quantity);
                saveCart();
                break;
            case 4:
                displayFruits();
                break;
            case 5:
                displayUsers();
                break;
            case 6:
                printf("Enter username to view cart: ");
                scanf(" %[^\n]s", username);
                viewCart(username);
                break;
            case 7:
                printf("Exiting...\n");
                saveFruits();
                saveUsers();
                saveCart();
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}

void loadFruits() {
    FILE* file = fopen(FRUITS_FILE, "r");
    if (!file) return;

    char name[50], unit[20];
    float price;
    while (fscanf(file, "%49s %f %19s", name, &price, unit) != EOF) {
        addFruit(name, price, unit);
    }
    fclose(file);
}

void saveFruits() {
    FILE* file = fopen(FRUITS_FILE, "w");
    if (!file) return;

    Fruit* current = fruitList;
    while (current) {
        fprintf(file, "%s %.2f %s\n", current->name, current->price, current->unit);
        current = current->next;
    }
    fclose(file);
}

void loadUsers() {
    FILE* file = fopen(USERS_FILE, "r");
    if (!file) return;

    char username[50], password[50];
    while (fscanf(file, "%49s %49s", username, password) != EOF) {
        addUser(username, password);
    }
    fclose(file);
}

void saveUsers() {
    FILE* file = fopen(USERS_FILE, "w");
    if (!file) return;

    User* current = userList;
    while (current) {
        fprintf(file, "%s %s\n", current->username, current->password);
        current = current->next;
    }
    fclose(file);
}

void loadCart() {
    FILE* file = fopen(CART_FILE, "r");
    if (!file) return;

    char username[50], fruitName[50];
    float quantity, price;
    while (fscanf(file, "%49s %49s %f %f", username, fruitName, &quantity, &price) != EOF) {
        addToCart(username, fruitName, quantity);
    }
    fclose(file);
}

void saveCart() {
    FILE* file = fopen(CART_FILE, "w");
    if (!file) return;

    CartItem* current = cartList;
    while (current) {
        fprintf(file, "%s %s %.2f %.2f\n", current->username, current->fruitName, current->quantity, current->price);
        current = current->next;
    }
    fclose(file);
}

void addFruit(char* name, float price, char* unit) {
    Fruit* newFruit = (Fruit*)malloc(sizeof(Fruit));
    strcpy(newFruit->name, name);
    newFruit->price = price;
    strcpy(newFruit->unit, unit);
    newFruit->next = fruitList;
    fruitList = newFruit;
    printf("Fruit '%s' added successfully.\n", name);
}

void addUser(char* username, char* password) {
    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->next = userList;
    userList = newUser;
    printf("User '%s' added successfully.\n", username);
}

void addToCart(char* username, char* fruitName, float quantity) {
    Fruit* fruit = fruitList;
    while (fruit) {
        if (strcmp(fruit->name, fruitName) == 0) {
            CartItem* newItem = (CartItem*)malloc(sizeof(CartItem));
            strcpy(newItem->username, username);
            strcpy(newItem->fruitName, fruitName);
            newItem->quantity = quantity;
            newItem->price = quantity * fruit->price;
            newItem->next = cartList;
            cartList = newItem;
            printf("Added %s to %s's cart.\n", fruitName, username);
            return;
        }
        fruit = fruit->next;
    }
    printf("Fruit not found.\n");
}

void viewCart(char* username) {
    CartItem* current = cartList;
    printf("\n=== Cart for %s ===\n", username);
    while (current) {
        if (strcmp(current->username, username) == 0) {
            printf("%s: %.2f units, Rs. %.2f\n", current->fruitName, current->quantity, current->price);
        }
        current = current->next;
    }
}

void displayFruits() {
    Fruit* current = fruitList;
    printf("\n=== Available Fruits ===\n");
    while (current) {
        printf("%s: Rs. %.2f/%s\n", current->name, current->price, current->unit);
        current = current->next;
    }
}

void displayUsers() {
    User* current = userList;
    printf("\n=== Registered Users ===\n");
    while (current) {
        printf("Username: %s\n", current->username);
        current = current->next;
    }
}