#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define FILENAME "users.txt"
#define KEY 5
#define MAX_ATTEMPTS 3

typedef struct User 
{
    char username[50];
    char password[50];
    struct User* next; 
} User;

User* userList = NULL; 
int loginAttempts = 0;

void loadUsers();
void saveUsers();
void registerUser();
int loginUser();
void encryptPassword(char* password);
void decryptPassword(char* password);
void adminPanel();
void viewUsers();
void deleteUser(char* username);
void getMaskedPassword(char* password);

int main() {
    loadUsers(); 

    int choice;
    while (1) {
        printf("\n__________________________________________________\n");
        printf(" Welcome to Suyambu Lingam Pazhamudhir Cholai \n");
        printf("____________________________________________________\n");
        printf("1 Register\n");
        printf("2 Login\n");
        printf("3 Admin Panel\n");
        printf("4 Exit\n");
        printf("____________________________________________________\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginAttempts >= MAX_ATTEMPTS) {
                    printf("\n Too many failed attempts! Please try again later.\n");
                } else if (loginUser()) {
                    printf("\n Login successful! \nRedirecting to your cart...\n");
                    loginAttempts = 0;
                } else {
                    loginAttempts++;
                    printf("\n Invalid username or password. Attempts left: %d\n", MAX_ATTEMPTS - loginAttempts);
                }
                break;
            case 3:
                adminPanel();
                break;
            case 4:
                saveUsers(); 
                printf("\n Exiting... Thank you for visiting!\n");
                exit(0);
            default:
                printf("\n Invalid choice. Please try again.\n");
        }
    }
    return 0;
}

void loadUsers() {
    FILE* file = fopen(FILENAME, "r");
    if (!file) return;

    char username[50], password[50];
    while (fscanf(file, "%s %s", username, password) != EOF) {
        User* newUser = (User*)malloc(sizeof(User));
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        decryptPassword(newUser->password);
        newUser->next = userList;
        userList = newUser;
    }
    fclose(file);
}

void saveUsers() {
    FILE* file = fopen(FILENAME, "w");
    if (!file) return;

    User* current = userList;
    while (current) {
        encryptPassword(current->password);
        fprintf(file, "%s %s\n", current->username, current->password);
        decryptPassword(current->password); 
        current = current->next;
    }
    fclose(file);
}

void registerUser() {
    char username[50], password[50];
    printf("\n--- Register New User ---\n");
    printf("Enter username: ");
    scanf("%s", username);

    printf("Enter password: ");
    getMaskedPassword(password);

    User* current = userList;
    while (current) {
        if (strcmp(current->username, username) == 0) {
            printf("\n Username already exists! Try a different one.\n");
            return;
        }
        current = current->next;
    }

    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    encryptPassword(newUser->password);
    newUser->next = userList;
    userList = newUser;

    printf("\n Registration successful! You can now log in. \n");
}

int loginUser() {
    char username[50], password[50];
    printf("\n--- User Login ---\n");
    printf("Enter username: ");
    scanf("%s", username);

    printf("Enter password: ");
    getMaskedPassword(password);

    encryptPassword(password);

    User* current = userList;
    while (current) {
        if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
            return 1; 
        }
        current = current->next;
    }
    return 0; 
}

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

void adminPanel() {
    char adminUser[50] = "admin";
    char adminPass[50] = "admin123";
    char username[50], password[50];

    printf("\n--- Admin Login ---\n");
    printf("Enter admin username: ");
    scanf("%s", username);

    printf("Enter admin password: ");
    getMaskedPassword(password);

    if (strcmp(username, adminUser) == 0 && strcmp(password, adminPass) == 0) {
        int choice;
        printf("\n Admin Access Granted! \n");
        do {
            printf("\n--- Admin Panel ---\n");
            printf("1 View Users\n");
            printf("2 Delete User\n");
            printf("3 Logout\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    viewUsers();
                    break;
                case 2:
                    printf("Enter username to delete: ");
                    scanf("%s", username);
                    deleteUser(username);
                    break;
                case 3:
                    printf("\n Logging out from Admin Panel.\n");
                    return;
                default:
                    printf("\n Invalid choice. Please try again.\n");
            }
        } while (choice != 3);
    } else {
        printf("\n Invalid admin credentials!\n");
    }
}

void viewUsers() {
    if (!userList) {
        printf("\n No registered users found!\n");
        return;
    }

    printf("\n--- Registered Users ---\n");
    User* current = userList;
    while (current) {
        printf("Username: %s | Password: %s\n", current->username, current->password);
        current = current->next;
    }
}

void deleteUser(char* username) {
    User *current = userList, *prev = NULL;

    while (current) {
        if (strcmp(current->username, username) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                userList = current->next;
            }
            free(current);
            printf("\n User '%s' deleted successfully.\n", username);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("\n User not found!\n");
}

void getMaskedPassword(char* password) {
    char ch;
    int i = 0;
    while ((ch = getch()) != 13) { 
        if (ch == '\b' && i > 0) { 
            printf("\b \b");
            i--;
        } else if (ch != '\b') {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}
