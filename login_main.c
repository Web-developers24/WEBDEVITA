#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "users.txt"
#define KEY 5

typedef struct User {
    char username[50];
    char password[50];
    char email[100];
    struct User* next;
} User;

User* userList = NULL;

void encrypt(char* pwd) {
    for (int i = 0; pwd[i]; i++) pwd[i] ^= KEY;
}

void decrypt(char* pwd) {
    for (int i = 0; pwd[i]; i++) pwd[i] ^= KEY;
}

int isValidPassword(const char* pwd) {
    int hasL = 0, hasD = 0, hasS = 0;
    for (int i = 0; pwd[i]; i++) {
        if (isalpha(pwd[i])) hasL = 1;
        else if (isdigit(pwd[i])) hasD = 1;
        else hasS = 1;
    }
    return hasL && hasD && hasS;
}

void loadUsers() {
    FILE* f = fopen(FILE_NAME, "r");
    if (!f) return;

    char uname[50], pwd[50], email[100];
    while (fscanf(f, "%s %s %s", uname, pwd, email) != EOF) {
        User* u = malloc(sizeof(User));
        strcpy(u->username, uname);
        strcpy(u->password, pwd);
        decrypt(u->password); // convert back to plain text
        strcpy(u->email, email);
        u->next = userList;
        userList = u;
    }
    fclose(f);
}

void saveUsers() {
    FILE* f = fopen(FILE_NAME, "w");
    if (!f) return;

    User* cur = userList;
    while (cur) {
        char tempPwd[50];
        strcpy(tempPwd, cur->password);
        encrypt(tempPwd);  // temporarily encrypt for file storage
        fprintf(f, "%s %s %s\n", cur->username, tempPwd, cur->email);
        cur = cur->next;
    }
    fclose(f);
}

int registerUser(char* uname, char* pwd, char* email) {
    User* cur = userList;
    while (cur) {
        if (strcmp(cur->username, uname) == 0) return 0; // user exists
        cur = cur->next;
    }

    if (!isValidPassword(pwd)) return -1; // invalid password

    User* u = malloc(sizeof(User));
    strcpy(u->username, uname);
    strcpy(u->password, pwd); // store as plain text
    strcpy(u->email, email);
    u->next = userList;
    userList = u;

    saveUsers();
    return 1; // success
}

int loginUser(char* uname, char* pwd) {
    User* cur = userList;
    while (cur) {
        if (strcmp(cur->username, uname) == 0 && strcmp(cur->password, pwd) == 0) {
            return 1; // success
        }
        cur = cur->next;
    }
    return 0; // fail
}

int adminLogin(char* uname, char* pwd) {
    return (strcmp(uname, "admin") == 0 && strcmp(pwd, "admin123") == 0);
}

void menu() {
    int choice;
    char uname[50], pwd[50], email[100];

    do {
        printf("\n--- Suyambu Lingam Pazhamudhir Cholai ---\n");
        printf("1. Signup\n2. Login\n3. Admin Login\n4. Exit\nEnter choice: ");
        scanf("%d", &choice);
        getchar(); // consume leftover newline

        if (choice == 1) {
            printf("Enter Username: "); fgets(uname, 50, stdin); uname[strcspn(uname, "\n")] = '\0';
            printf("Enter Email: "); fgets(email, 100, stdin); email[strcspn(email, "\n")] = '\0';
            printf("Enter Password: "); fgets(pwd, 50, stdin); pwd[strcspn(pwd, "\n")] = '\0';

            int res = registerUser(uname, pwd, email);
            if (res == 1) printf("✅ Signup Successful!\n");
            else if (res == -1) printf("❌ Password must include letters, numbers, and special characters!\n");
            else printf("❌ Username already exists!\n");

        } else if (choice == 2) {
            printf("Enter Username: "); fgets(uname, 50, stdin); uname[strcspn(uname, "\n")] = '\0';
            printf("Enter Password: "); fgets(pwd, 50, stdin); pwd[strcspn(pwd, "\n")] = '\0';

            if (loginUser(uname, pwd)) printf("✅ Login Successful!\n");
            else printf("❌ Invalid username or password!\n");

        } else if (choice == 3) {
            printf("Enter Admin Username: "); fgets(uname, 50, stdin); uname[strcspn(uname, "\n")] = '\0';
            printf("Enter Admin Password: "); fgets(pwd, 50, stdin); pwd[strcspn(pwd, "\n")] = '\0';

            if (adminLogin(uname, pwd)) printf("✅ Admin Login Successful!\n");
            else printf("❌ Invalid Admin Credentials!\n");
        }

    } while (choice != 4);
}

int main() {
    loadUsers();
    menu();
    return 0;
}
