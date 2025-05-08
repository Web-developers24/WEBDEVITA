#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRUIT_FILE "fruits.txt"
#define CART_FILE "cart.txt"

// ---------- Fruit BST ----------
typedef struct Fruit {
    char name[50];
    float price;
    char unit[20];
    int stock;
    struct Fruit* left;
    struct Fruit* right;
} Fruit;

Fruit* fruitRoot = NULL;

Fruit* createFruit(char* name, float price, char* unit, int stock) {
    Fruit* f = malloc(sizeof(Fruit));
    strcpy(f->name, name);
    f->price = price;
    strcpy(f->unit, unit);
    f->stock = stock;
    f->left = f->right = NULL;
    return f;
}

Fruit* insertFruit(Fruit* root, char* name, float price, char* unit, int stock) {
    if (!root) return createFruit(name, price, unit, stock);
    if (strcmp(name, root->name) < 0)
        root->left = insertFruit(root->left, name, price, unit, stock);
    else if (strcmp(name, root->name) > 0)
        root->right = insertFruit(root->right, name, price, unit, stock);
    return root;
}

Fruit* searchFruit(Fruit* root, char* name) {
    if (!root) return NULL;
    int cmp = strcmp(name, root->name);
    if (cmp == 0) return root;
    else if (cmp < 0) return searchFruit(root->left, name);
    else return searchFruit(root->right, name);
}

void loadFruits() {
    FILE* f = fopen(FRUIT_FILE, "r");
    if (!f) return;
    char name[50], unit[20];
    float price;
    int qty;
    while (fscanf(f, "%s %f %d %s", name, &price, &qty, unit) == 4)
        fruitRoot = insertFruit(fruitRoot, name, price, unit, qty);
    fclose(f);
}

void displayFruits(Fruit* root) {
    if (!root) return;
    displayFruits(root->left);
    printf("%s - Rs. %.2f per %s (%d available)\n", root->name, root->price, root->unit, root->stock);
    displayFruits(root->right);
}

void saveFruitsToFile(Fruit* root, FILE* f) {
    if (!root) return;
    saveFruitsToFile(root->left, f);
    fprintf(f, "%s %.2f %d %s\n", root->name, root->price, root->stock, root->unit);
    saveFruitsToFile(root->right, f);
}

void saveFruits() {
    FILE* f = fopen(FRUIT_FILE, "w");
    if (!f) return;
    saveFruitsToFile(fruitRoot, f);
    fclose(f);
}

// ---------- Cart Queue ----------
typedef struct Cart {
    char name[50];
    float qty;
    float total;
    struct Cart* next;
} Cart;

Cart* front = NULL;
Cart* rear = NULL;

void addToCart(char* username, char* fruitName, float qty) {
    Fruit* f = searchFruit(fruitRoot, fruitName);
    if (!f) {
        printf("❌ Fruit not found!\n");
        return;
    }
    if (f->stock < qty) {
        printf("⚠ Not enough stock for %s!\n", fruitName);
        return;
    }

    f->stock -= qty;

    Cart* c = malloc(sizeof(Cart));
    strcpy(c->name, fruitName);
    c->qty = qty;
    c->total = qty * f->price;
    c->next = NULL;

    if (!front) front = rear = c;
    else {
        rear->next = c;
        rear = c;
    }

    // ✅ Save to cart.txt with username
    FILE* fp = fopen(CART_FILE, "a");
    if (fp) {
        fprintf(fp, "%s %s %.2f %.2f\n", username, fruitName, qty, c->total);
        fclose(fp);
    } else {
        printf("⚠ Could not open cart.txt to save.\n");
    }

    printf("✅ Added %.2f of %s to %s's cart (saved to file).\n", qty, fruitName, username);
}

void viewCart() {
    if (!front) {
        printf("🛒 Your cart is empty.\n");
        return;
    }

    Cart* cur = front;
    float total = 0;
    printf("\n--- CART ITEMS (CURRENT SESSION) ---\n");
    while (cur) {
        printf("%s - %.2f units - Rs. %.2f\n", cur->name, cur->qty, cur->total);
        total += cur->total;
        cur = cur->next;
    }
    printf("TOTAL: Rs. %.2f\n", total);
}

void viewCartFromFile(char* username) {
    FILE* fp = fopen(CART_FILE, "r");
    if (!fp) {
        printf("⚠ Could not open cart.txt!\n");
        return;
    }

    char uname[50], fruit[50];
    float qty, total;
    int found = 0;

    printf("\n--- CART ITEMS FOR USER: %s ---\n", username);
    while (fscanf(fp, "%s %s %f %f", uname, fruit, &qty, &total) == 4) {
        if (strcmp(uname, username) == 0) {
            printf("%s - %.2f units - Rs. %.2f\n", fruit, qty, total);
            found = 1;
        }
    }

    if (!found) printf("🛒 No items found for this user.\n");
    fclose(fp);
}

void clearCart() {
    Cart* cur = front;
    while (cur) {
        Cart* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    front = rear = NULL;
}

// ---------- Menu ----------
void menu() {
    int ch;
    char fruitName[50], username[50];
    float qty;

    printf("Enter your username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0;

    do {
        printf("\n--- SUYAMBU LINGAM PAZHAMUDHIR CHOLAI ---\n");
        printf("1. View Fruits\n2. Add to Cart\n3. View Current Cart\n4. View Saved Cart from File\n5. Checkout\n6. Exit\nChoice: ");
        scanf("%d", &ch);
        getchar();

        if (ch == 1) {
            displayFruits(fruitRoot);
        } else if (ch == 2) {
            printf("Enter fruit name: ");
            fgets(fruitName, 50, stdin);
            fruitName[strcspn(fruitName, "\n")] = 0;
            printf("Enter quantity: ");
            scanf("%f", &qty); getchar();
            addToCart(username, fruitName, qty);
        } else if (ch == 3) {
            viewCart();
        } else if (ch == 4) {
            viewCartFromFile(username);
        } else if (ch == 5) {
            viewCart();
            printf("✅ Checkout successful. Thank you!\n");
            clearCart();
        }

    } while (ch != 6);

    saveFruits();
}

int main() {
    loadFruits();
    menu();
    return 0;
}
