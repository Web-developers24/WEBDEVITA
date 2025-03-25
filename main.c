#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> 

#define MAX_USERS 100
#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define FILENAME "users.txt"
#define KEY 5 
#define MAX_ATTEMPTS 3

typedef struct {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} User;

void registerUser();
int loginUser();
void saveUser(User user);
int checkCredentials(char *username, char *password);
void encryptPassword(char *password);
void decryptPassword(char *password);
void adminPanel();
void viewUsers();
void deleteUser(char *username);
void getMaskedPassword(char *password);

int loginAttempts = 0;

int main() {
    int choice;
    while (1) {
        printf("\n====================================================\n");
        printf(" Welcome to Suyambu Lingam Pazhamudhir Cholai \n");
        printf("====================================================\n");
        printf("1 Register\n");
        printf("2 Login\n");
        printf("3 Admin Panel\n");
        printf("4 Exit\n");
        printf("====================================================\n");
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
                printf("\n Exiting... Thank you for visiting!\n");
                exit(0);
            default:
                printf("\n Invalid choice. Please try again.\n");
        }
    }
    return 0;
}

void registerUser() {
    User user;
    printf("\n--- Register New User ---\n");
    printf("Enter username: ");
    scanf("%s", user.username);
    
    printf(" Enter password: ");
    getMaskedPassword(user.password);

    if (checkCredentials(user.username, user.password)) {
        printf("\n Username already exists! Try a different one.\n");
        return;
    }

    encryptPassword(user.password);
    saveUser(user);
    printf("\n Registration successful! You can now log in. \n");
}

int loginUser() {
    char username[USERNAME_LEN], password[PASSWORD_LEN];
    printf("\n---  User Login ---\n");
    printf(" Enter username: ");
    scanf("%s", username);

    printf(" Enter password: ");
    getMaskedPassword(password);

    encryptPassword(password);

    if (checkCredentials(username, password)) {
        return 1; 
    }
    return 0; 
}

void saveUser(User user) {
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("\n Error opening file!\n");
        return;
    }
    fprintf(file, "%s %s\n", user.username, user.password);
    fclose(file);
}

int checkCredentials(char *username, char *password) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        return 0;
    }

    User user;
    while (fscanf(file, "%s %s", user.username, user.password) != EOF) {
        if (strcmp(user.username, username) == 0) {
            if (strcmp(user.password, password) == 0) {
                fclose(file);
                return 1; 
            }
        }
    }
    fclose(file);
    return 0;
}

void encryptPassword(char *password) {
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] ^= KEY; 
    }
}

void decryptPassword(char *password) {
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] ^= KEY;
    }
}

void adminPanel() {
    char adminUser[USERNAME_LEN] = "admin";
    char adminPass[PASSWORD_LEN] = "admin123";
    char username[USERNAME_LEN], password[PASSWORD_LEN];

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
            printf(" Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    viewUsers();
                    break;
                case 2:
                    printf(" Enter username to delete: ");
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
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("\n No registered users found!\n");
        return;
    }

    User user;
    printf("\n--- Registered Users ---\n");
    while (fscanf(file, "%s %s", user.username, user.password) != EOF) {
        decryptPassword(user.password);
        printf("Username: %s | Password: %s\n", user.username, user.password);
    }
    fclose(file);
}

void deleteUser(char *username) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("\n No registered users found!\n");
        return;
    }

    User users[MAX_USERS];
    int count = 0, found = 0;

    // Read all users into memory
    while (fscanf(file, "%s %s", users[count].username, users[count].password) != EOF) {
        if (strcmp(users[count].username, username) != 0) {
            count++;
        } else {
            found = 1;
        }
    }
    fclose(file);

    if (!found) {
        printf("\n User not found!\n");
        return;
    }

    file = fopen(FILENAME, "w");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s\n", users[i].username, users[i].password);
    }
    fclose(file);
    printf("\n User '%s' deleted successfully.\n", username);
}

void getMaskedPassword(char *password) {
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
