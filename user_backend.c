#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define USERS_FILE "users.txt"
#define KEY 5

typedef struct User {
    char username[50];
    char password[50];
    char address[500];
    struct User* next;
} User;

User* userList = NULL;

void encryptPassword(char* password) {
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] ^= KEY;
    }
}

void decryptPassword(char* password) {
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] ^= KEY;
    }
}

void loadUsers() {
    FILE* file = fopen(USERS_FILE, "r");
    if (!file) return;

    char username[50], password[50], address[500];
    while (fscanf(file, "%s %s %[^\n]", username, password, address) != EOF) {
        User* newUser = (User*)malloc(sizeof(User));
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        decryptPassword(newUser->password);
        strcpy(newUser->address, address);
        newUser->next = userList;
        userList = newUser;
    }
    fclose(file);
}

void saveUsers() {
    FILE* file = fopen(USERS_FILE, "w");
    if (!file) return;

    User* current = userList;
    while (current) {
        encryptPassword(current->password);
        fprintf(file, "%s %s %s\n", current->username, current->password, current->address);
        decryptPassword(current->password);
        current = current->next;
    }
    fclose(file);
}

User* findUser(const char* username) {
    User* current = userList;
    while (current) {
        if (strcmp(current->username, username) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void updateAddress(const char* username, const char* newAddress) {
    User* user = findUser(username);
    if (user) {
        strcpy(user->address, newAddress);
        saveUsers();
        printf("? Address updated for %s!\n", username);
    } else {
        printf("? User not found!\n");
    }
}

void showAddress(const char* username) {
    User* user = findUser(username);
    if (user) {
        printf("?? Address for %s: %s\n", username, user->address);
    } else {
        printf("? User not found!\n");
    }
}

void menu() {
    int ch;
    char username[50], address[500];

    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    do {
        printf("\n--- USER DASHBOARD: %s ---\n", username);
        printf("1. View Saved Address\n2. Update Address\n3. Exit\nChoice: ");
        scanf("%d", &ch); getchar();

        if (ch == 1) {
            showAddress(username);
        } else if (ch == 2) {
            printf("Enter new address:\n");
            fgets(address, sizeof(address), stdin);
            address[strcspn(address, "\n")] = 0;
            updateAddress(username, address);
        }

    } while (ch != 3);
}

int main() {
    loadUsers();
    menu();
    return 0;
}
