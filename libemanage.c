/* ================= START ================= */

/* ================= IMPORT ================= */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_BOOKS 100
#define MAX_STR 64

/* ANSI colors */
#define CLR_RESET "\033[0m"
#define CLR_MENU  "\033[1;36m"
#define CLR_OK    "\033[1;32m"
#define CLR_BAD   "\033[1;31m"
#define CLR_INFO  "\033[1;33m"

/* ================= DATA STRUCTURES ================= */

typedef struct {
    char name[MAX_STR];
    char phone[MAX_STR];
} Borrower;

typedef struct {
    int id;
    char title[MAX_STR];
    char author[MAX_STR];
    int year;
    int available;
    Borrower borrower;
} Book;

/* ================= GLOBAL ================= */

Book library[MAX_BOOKS];
int totalBooks = 0;

/* ================= UTILITY ================= */

void clearScreen() { system("cls"); }

void pauseScreen() {
    printf("\n");
    system("pause");
}

void toLowerStr(char *s) {
    for (; *s; s++) *s = tolower(*s);
}

void normalizeIDs() {
    for (int i = 0; i < totalBooks; i++)
        library[i].id = i + 1;
}

int safeIntInput() {
    int x;
    while (scanf("%d", &x) != 1) {
        while (getchar() != '\n');
        printf(CLR_BAD "Invalid input. Enter number: " CLR_RESET);
    }
    return x;
}

void safeStringInput(char *buf) {
    while (1) {
        fgets(buf, MAX_STR, stdin);
        buf[strcspn(buf, "\n")] = 0;
        if (strlen(buf) > 0) return;
        printf(CLR_BAD "Input cannot be empty. Retry: " CLR_RESET);
    }
}

/* ================= ASCII ART ================= */

void banner() {
    printf("\n");
    printf("        _     _ _     __  __                                   \n");
    printf("       | |   (_) |   |  \\/  |                                  \n");
    printf("       | |    _| |__ | \\  / | ___ _ __   __ _  __ _  ___       \n");
    printf("       | |   | | '_ \\| |\\/| |/ _ \\ '_ \\ / _` |/ _` |/ _ \\      \n");
    printf("       | |___| | |_) | |  | |  __/ | | | (_| | (_| |  __/      \n");
    printf("       |_____|_|_.__/|_|  |_|\\___|_| |_|\\__,_|\\__, |\\___|      \n");
    printf("                                                __/ |          \n");
    printf("                                               |___/           \n\n");
}

/* ================= PRELOAD ================= */

void preload() {
    Book b[] = {
        {1,"Web Application Hacker Handbook","Stuttard",2011,1,{"",""}},
        {2,"Hacking Art of Exploitation","Erickson",2008,1,{"",""}},
        {3,"Blue Team Handbook","Murdoch",2018,1,{"",""}},
        {4,"Practical Malware Analysis","Sikorski",2012,1,{"",""}},
        {5,"Cybersecurity Essentials","Cisco",2020,1,{"",""}},
        {6,"Engineering Mathematics","Kreyszig",2014,1,{"",""}},
        {7,"Digital Logic Design","Mano",2016,1,{"",""}},
        {8,"Operating System Concepts","Silberschatz",2019,1,{"",""}},
        {9,"Computer Organization","Hamacher",2018,1,{"",""}},
        {10,"Data Structures in C","Lipschutz",2017,1,{"",""}}
    };

    totalBooks = 10;
    for (int i = 0; i < totalBooks; i++)
        library[i] = b[i];
}

/* ================= DISPLAY ================= */

void printBook(Book *b) {
    printf("%03d  %-30s %-20s %4d  ",
           b->id, b->title, b->author, b->year);

    if (b->available)
        printf(CLR_OK "Available" CLR_RESET "\n");
    else
        printf(CLR_BAD "Borrowed" CLR_RESET "\n");
}

void showBooks(Book arr[], int n) {
    printf("\nID   Title                          Author               Year  Status\n");
    printf("------------------------------------------------------------------------\n");
    for (int i = 0; i < n; i++)
        printBook(&arr[i]);
}

/* ================= SEARCH ================= */

int findByID(int id) {
    for (int i = 0; i < totalBooks; i++)
        if (library[i].id == id) return i;
    return -1;
}

int findByName(char *name) {
    char key[MAX_STR], temp[MAX_STR];
    strcpy(key, name);
    toLowerStr(key);

    for (int i = 0; i < totalBooks; i++) {
        strcpy(temp, library[i].title);
        toLowerStr(temp);
        if (strstr(temp, key)) return i;
    }
    return -1;
}

/* ================= TEMP SORT (BUBBLE) ================= */

void tempSort(int mode) {
    Book temp[MAX_BOOKS];
    memcpy(temp, library, sizeof(Book) * totalBooks);

    for (int i = 0; i < totalBooks - 1; i++) {
        for (int j = 0; j < totalBooks - i - 1; j++) {
            int swap = 0;
            if (mode == 1 && strcmp(temp[j].title, temp[j+1].title) > 0)
                swap = 1;
            if (mode == 2 && temp[j].year > temp[j+1].year)
                swap = 1;

            if (swap) {
                Book t = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = t;
            }
        }
    }
    showBooks(temp, totalBooks);
}

/* ================= CORE FEATURES ================= */

void viewBooks() {
    int ch;
    clearScreen(); banner();
    showBooks(library, totalBooks);

    printf("\n1. Sort by Name (A-Z)");
    printf("\n2. Sort by Year");
    printf("\n0. Back");

    printf("\n\nChoice: ");
    ch = safeIntInput();

    if (ch == 1) tempSort(1);
    else if (ch == 2) tempSort(2);

    pauseScreen();
}

int searchAndShow() {
    int opt, id, idx = -1;
    char name[MAX_STR];

    printf("\n1. Search by Name");
    printf("\n2. Search by ID");
    printf("\n\nChoice: ");
    opt = safeIntInput();
    getchar();

    if (opt == 1) {
        printf("\nEnter name: ");
        safeStringInput(name);
        idx = findByName(name);
    } else if (opt == 2) {
        printf("\nEnter ID: ");
        id = safeIntInput();
        idx = findByID(id);
    }

    if (idx == -1) {
        printf(CLR_BAD "\nBook not found.\n" CLR_RESET);
        return -1;
    }

    printf(CLR_INFO "\nBook Found:\n" CLR_RESET);
    printBook(&library[idx]);
    return idx;
}

void addBook() {
    clearScreen(); banner();
    Book b;
    b.available = 1;
    strcpy(b.borrower.name,"");
    strcpy(b.borrower.phone,"");

    getchar();
    printf("\nEnter Book Name: ");
    safeStringInput(b.title);

    printf("Enter Author: ");
    safeStringInput(b.author);

    printf("Enter Year: ");
    b.year = safeIntInput();

    library[totalBooks++] = b;
    normalizeIDs();

    printf(CLR_OK "\nBook added.\n" CLR_RESET);
    pauseScreen();
}

void removeBook() {
    clearScreen(); banner();
    int idx = searchAndShow();
    if (idx == -1) { pauseScreen(); return; }

    for (int i = idx; i < totalBooks - 1; i++)
        library[i] = library[i + 1];

    totalBooks--;
    normalizeIDs();

    printf(CLR_OK "\nBook removed.\n" CLR_RESET);
    pauseScreen();
}

void borrowBook() {
    clearScreen(); banner();
    int idx = searchAndShow();
    if (idx == -1 || !library[idx].available) {
        printf(CLR_BAD "\nCannot borrow.\n" CLR_RESET);
        pauseScreen(); return;
    }

    getchar();
    printf("\nBorrower Name: ");
    safeStringInput(library[idx].borrower.name);

    printf("Phone: ");
    safeStringInput(library[idx].borrower.phone);

    library[idx].available = 0;
    printf(CLR_OK "\nBorrowed successfully.\n" CLR_RESET);
    pauseScreen();
}

void borrowedTable() {
    clearScreen(); banner();
    printf("\nID   Title                          Borrower           Phone\n");
    printf("-------------------------------------------------------------\n");

    for (int i = 0; i < totalBooks; i++)
        if (!library[i].available)
            printf("%03d  %-30s %-18s %s\n",
                library[i].id,
                library[i].title,
                library[i].borrower.name,
                library[i].borrower.phone);

    pauseScreen();
}

void returnBook() {
    clearScreen(); banner();
    int idx = searchAndShow();
    if (idx == -1 || library[idx].available) {
        printf(CLR_BAD "\nInvalid return.\n" CLR_RESET);
        pauseScreen(); return;
    }

    library[idx].available = 1;
    strcpy(library[idx].borrower.name,"");
    strcpy(library[idx].borrower.phone,"");

    printf(CLR_OK "\nBook returned.\n" CLR_RESET);
    pauseScreen();
}

/* ================= MAIN ================= */

int main() {
    int ch;
    preload();

    while (1) {
        clearScreen(); banner();
        printf(CLR_MENU "1. View All Books\n" CLR_RESET);
        printf(CLR_MENU "2. Add Book\n" CLR_RESET);
        printf(CLR_MENU "3. Remove Book\n" CLR_RESET);
        printf(CLR_MENU "4. Borrow Book\n" CLR_RESET);
        printf(CLR_MENU "5. Borrowed Table\n" CLR_RESET);
        printf(CLR_MENU "6. Return Book\n" CLR_RESET);
        printf(CLR_MENU "7. Exit\n" CLR_RESET);

        printf("\nChoice: ");
        ch = safeIntInput();

        switch (ch) {
            case 1: viewBooks(); break;
            case 2: addBook(); break;
            case 3: removeBook(); break;
            case 4: borrowBook(); break;
            case 5: borrowedTable(); break;
            case 6: returnBook(); break;
            case 7: exit(0);
            default:
                printf(CLR_BAD "\nInvalid choice.\n" CLR_RESET);
                pauseScreen();
        }
    }
}

/* ================= END ================= */
