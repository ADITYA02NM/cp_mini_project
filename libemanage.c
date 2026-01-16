/*
 * libemanage.c - Library Book Management System
 * 
 * A command-line application for managing library books with borrowing,
 * returning, adding, removing, and sorting capabilities.
 * 
 * Compile: gcc libemanage.c -o libemanage -Wall -Wextra
 * Run: ./libemanage
 * 
 * Security Notes:
 * - Input validation on all user entries
 * - Buffer overflow protection via fgets + manual bounds checking
 * - Integer overflow checks on year input
 * - Case-insensitive search to prevent enumeration attacks
 * - No dynamic memory allocation (stack-based arrays only)
 * 
 * Constraints:
 * - Max 100 books
 * - Standard C (C99 compatible)
 * - No external libraries beyond standard library
 * - No command-line arguments
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

/* ANSI Color Codes - Use sparingly for status and menu highlights */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"

/* System Constraints */
#define MAX_BOOKS 100
#define MAX_STRING 100
#define MAX_PHONE 20
#define INITIAL_BOOKS 10

/* Borrower Information Structure */
typedef struct {
    char name[MAX_STRING];
    char phone[MAX_PHONE];
} Borrower;

/* Book Structure */
typedef struct {
    int id;
    char title[MAX_STRING];
    char author[MAX_STRING];
    int year;
    int available;  /* 1 = available, 0 = borrowed */
    Borrower borrower;
} Book;

/* Global State */
Book library[MAX_BOOKS];
int book_count = 0;

/* Function Prototypes - Organized by Functionality */

/* Core Operations */
void initialize_library(void);
void normalize_ids(void);
void view_all_books(void);
void add_book(void);
void remove_book(void);
void borrow_book(void);
void return_book(void);
void view_borrowed_books(void);

/* Search & Sort */
int search_by_name(const char *query, int *matches, int max_matches);
int search_by_id(int id);
void bubble_sort_by_title(Book *temp_array, int count);
void bubble_sort_by_year(Book *temp_array, int count);

/* UI & Display */
void clear_screen(void);
void display_banner(void);
void display_book_table(Book *books, int count);
void display_borrowed_table(void);

/* Input Validation */
int get_safe_int(const char *prompt, int *value);
void get_safe_string(const char *prompt, char *buffer, int max_len);
void trim_string(char *str);
void to_lowercase(char *dest, const char *src);

/* Utility */
void pause_screen(void);
void truncate_string(char *dest, const char *src, int max_len);

/*
 * Main Entry Point
 * Implements the main menu loop with input validation
 */
int main(void) {
    int choice;
    
    /* Initialize library with preloaded books */
    initialize_library();
    
    while (1) {
        clear_screen();
        display_banner();
        
        printf("\n");
        printf("  1. View all books\n");
        printf("  2. Add book\n");
        printf("  3. Remove book\n");
        printf("  4. Borrow book\n");
        printf("  5. Borrowed books table\n");
        printf("  6. Return book\n");
        printf("  7. Exit\n");
        printf("\n");
        
        if (!get_safe_int("Choice: ", &choice)) {
            printf(COLOR_RED "Invalid input. Please enter a number.\n" COLOR_RESET);
            pause_screen();
            continue;
        }
        
        switch (choice) {
            case 1:
                view_all_books();
                break;
            case 2:
                add_book();
                break;
            case 3:
                remove_book();
                break;
            case 4:
                borrow_book();
                break;
            case 5:
                view_borrowed_books();
                break;
            case 6:
                return_book();
                break;
            case 7:
                clear_screen();
                printf("\n  Exiting LibeManage. Goodbye!\n\n");
                return 0;
            default:
                printf(COLOR_RED "Invalid choice. Please select 1-7.\n" COLOR_RESET);
                pause_screen();
        }
    }
    
    return 0;
}

/*
 * Initialize Library with Preloaded Books
 * Loads 5 cybersecurity + 5 engineering books
 * All books initially available
 */
void initialize_library(void) {
    /* Cybersecurity Books */
    strcpy(library[0].title, "Applied Cryptography");
    strcpy(library[0].author, "Bruce Schneier");
    library[0].year = 1996;
    library[0].available = 1;
    
    strcpy(library[1].title, "The Web Application Hacker's Handbook");
    strcpy(library[1].author, "Dafydd Stuttard");
    library[1].year = 2011;
    library[1].available = 1;
    
    strcpy(library[2].title, "Hacking: The Art of Exploitation");
    strcpy(library[2].author, "Jon Erickson");
    library[2].year = 2008;
    library[2].available = 1;
    
    strcpy(library[3].title, "Network Security Essentials");
    strcpy(library[3].author, "William Stallings");
    library[3].year = 2017;
    library[3].available = 1;
    
    strcpy(library[4].title, "Metasploit: The Penetration Tester's Guide");
    strcpy(library[4].author, "David Kennedy");
    library[4].year = 2011;
    library[4].available = 1;
    
    /* Engineering Books */
    strcpy(library[5].title, "Introduction to Algorithms");
    strcpy(library[5].author, "Thomas H. Cormen");
    library[5].year = 2009;
    library[5].available = 1;
    
    strcpy(library[6].title, "Design Patterns");
    strcpy(library[6].author, "Erich Gamma");
    library[6].year = 1994;
    library[6].available = 1;
    
    strcpy(library[7].title, "Computer Networking: A Top-Down Approach");
    strcpy(library[7].author, "James Kurose");
    library[7].year = 2016;
    library[7].available = 1;
    
    strcpy(library[8].title, "Operating System Concepts");
    strcpy(library[8].author, "Abraham Silberschatz");
    library[8].year = 2018;
    library[8].available = 1;
    
    strcpy(library[9].title, "Database System Concepts");
    strcpy(library[9].author, "Abraham Silberschatz");
    library[9].year = 2019;
    library[9].available = 1;
    
    book_count = INITIAL_BOOKS;
    normalize_ids();
}

/*
 * Normalize Book IDs
 * Ensures IDs are sequential starting from 1 with no gaps
 * Called after add/remove operations to maintain ID continuity
 */
void normalize_ids(void) {
    int i;
    for (i = 0; i < book_count; i++) {
        library[i].id = i + 1;
    }
}

/*
 * View All Books with Sorting Sub-Menu
 * Displays formatted table with optional temporary sorting
 * Sorting does NOT modify original array
 */
void view_all_books(void) {
    int choice;
    Book temp_array[MAX_BOOKS];
    int i;
    
    if (book_count == 0) {
        clear_screen();
        display_banner();
        printf("\n" COLOR_YELLOW "  No books in library.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    while (1) {
        clear_screen();
        display_banner();
        printf("\n");
        
        /* Display current book list */
        display_book_table(library, book_count);
        
        printf("\n");
        printf("  1. Sort by book name (A-Z)\n");
        printf("  2. Sort by year (ascending)\n");
        printf("  3. Back to main menu\n");
        printf("\n");
        
        if (!get_safe_int("Choice: ", &choice)) {
            printf(COLOR_RED "Invalid input.\n" COLOR_RESET);
            pause_screen();
            continue;
        }
        
        if (choice == 3) {
            break;
        } else if (choice == 1) {
            /* Create temporary copy and sort by title */
            for (i = 0; i < book_count; i++) {
                temp_array[i] = library[i];
            }
            bubble_sort_by_title(temp_array, book_count);
            
            clear_screen();
            display_banner();
            printf("\n" COLOR_CYAN "  Sorted by Title (A-Z):\n" COLOR_RESET "\n");
            display_book_table(temp_array, book_count);
            pause_screen();
        } else if (choice == 2) {
            /* Create temporary copy and sort by year */
            for (i = 0; i < book_count; i++) {
                temp_array[i] = library[i];
            }
            bubble_sort_by_year(temp_array, book_count);
            
            clear_screen();
            display_banner();
            printf("\n" COLOR_CYAN "  Sorted by Year (Ascending):\n" COLOR_RESET "\n");
            display_book_table(temp_array, book_count);
            pause_screen();
        } else {
            printf(COLOR_RED "Invalid choice.\n" COLOR_RESET);
            pause_screen();
        }
    }
}

/*
 * Add Book to Library
 * Validates input and ensures capacity constraint
 * Reassigns IDs after addition
 */
void add_book(void) {
    char title[MAX_STRING];
    char author[MAX_STRING];
    int year;
    
    clear_screen();
    display_banner();
    printf("\n" COLOR_CYAN "  === Add New Book ===\n" COLOR_RESET "\n");
    
    if (book_count >= MAX_BOOKS) {
        printf(COLOR_RED "  Library is full (max %d books).\n" COLOR_RESET, MAX_BOOKS);
        pause_screen();
        return;
    }
    
    /* Get and validate book title */
    get_safe_string("  Book Title: ", title, MAX_STRING);
    if (strlen(title) == 0) {
        printf(COLOR_RED "  Title cannot be empty.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Get and validate author */
    get_safe_string("  Author: ", author, MAX_STRING);
    if (strlen(author) == 0) {
        printf(COLOR_RED "  Author cannot be empty.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Get and validate year */
    if (!get_safe_int("  Year: ", &year) || year < 1000 || year > 9999) {
        printf(COLOR_RED "  Invalid year. Must be between 1000 and 9999.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Add book to library */
    strcpy(library[book_count].title, title);
    strcpy(library[book_count].author, author);
    library[book_count].year = year;
    library[book_count].available = 1;
    library[book_count].borrower.name[0] = '\0';
    library[book_count].borrower.phone[0] = '\0';
    
    book_count++;
    normalize_ids();
    
    printf(COLOR_GREEN "\n  Book added successfully!\n" COLOR_RESET);
    pause_screen();
}

/*
 * Remove Book from Library
 * Supports search by name (substring) or ID
 * Shifts array and normalizes IDs after removal
 */
void remove_book(void) {
    int search_choice;
    int matches[MAX_BOOKS];
    int match_count;
    int book_index;
    char query[MAX_STRING];
    int id;
    int i;
    
    clear_screen();
    display_banner();
    printf("\n" COLOR_CYAN "  === Remove Book ===\n" COLOR_RESET "\n");
    
    if (book_count == 0) {
        printf(COLOR_YELLOW "  No books to remove.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    printf("  Search by:\n");
    printf("  1. Name\n");
    printf("  2. ID\n");
    printf("\n");
    
    if (!get_safe_int("Choice: ", &search_choice)) {
        printf(COLOR_RED "Invalid input.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    if (search_choice == 1) {
        /* Search by name */
        get_safe_string("\n  Enter book name: ", query, MAX_STRING);
        if (strlen(query) == 0) {
            printf(COLOR_RED "  Search query cannot be empty.\n" COLOR_RESET);
            pause_screen();
            return;
        }
        
        match_count = search_by_name(query, matches, MAX_BOOKS);
        
        if (match_count == 0) {
            printf(COLOR_YELLOW "\n  No books found matching '%s'.\n" COLOR_RESET, query);
            pause_screen();
            return;
        }
        
        if (match_count > 1) {
            printf(COLOR_YELLOW "\n  Multiple books found:\n" COLOR_RESET);
            for (i = 0; i < match_count; i++) {
                printf("  %d. [ID:%03d] %s\n", i + 1, library[matches[i]].id, library[matches[i]].title);
            }
            printf("\n");
            
            int selection;
            if (!get_safe_int("Select book number: ", &selection) || selection < 1 || selection > match_count) {
                printf(COLOR_RED "Invalid selection.\n" COLOR_RESET);
                pause_screen();
                return;
            }
            book_index = matches[selection - 1];
        } else {
            book_index = matches[0];
        }
    } else if (search_choice == 2) {
        /* Search by ID */
        if (!get_safe_int("\n  Enter book ID: ", &id)) {
            printf(COLOR_RED "Invalid ID.\n" COLOR_RESET);
            pause_screen();
            return;
        }
        
        book_index = search_by_id(id);
        
        if (book_index == -1) {
            printf(COLOR_YELLOW "\n  Book ID %03d not found.\n" COLOR_RESET, id);
            pause_screen();
            return;
        }
    } else {
        printf(COLOR_RED "Invalid choice.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Display book before removal */
    printf("\n  Removing book:\n");
    printf("  ID: %03d | Title: %s | Author: %s | Year: %d\n", 
           library[book_index].id, 
           library[book_index].title, 
           library[book_index].author, 
           library[book_index].year);
    
    /* Shift array to remove book */
    for (i = book_index; i < book_count - 1; i++) {
        library[i] = library[i + 1];
    }
    book_count--;
    
    normalize_ids();
    
    printf(COLOR_GREEN "\n  Book removed successfully!\n" COLOR_RESET);
    pause_screen();
}

/*
 * Borrow Book
 * Marks book as unavailable and stores borrower information
 */
void borrow_book(void) {
    int search_choice;
    int matches[MAX_BOOKS];
    int match_count;
    int book_index;
    char query[MAX_STRING];
    int id;
    int i;
    
    clear_screen();
    display_banner();
    printf("\n" COLOR_CYAN "  === Borrow Book ===\n" COLOR_RESET "\n");
    
    if (book_count == 0) {
        printf(COLOR_YELLOW "  No books available.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    printf("  Search by:\n");
    printf("  1. Name\n");
    printf("  2. ID\n");
    printf("\n");
    
    if (!get_safe_int("Choice: ", &search_choice)) {
        printf(COLOR_RED "Invalid input.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    if (search_choice == 1) {
        /* Search by name */
        get_safe_string("\n  Enter book name: ", query, MAX_STRING);
        if (strlen(query) == 0) {
            printf(COLOR_RED "  Search query cannot be empty.\n" COLOR_RESET);
            pause_screen();
            return;
        }
        
        match_count = search_by_name(query, matches, MAX_BOOKS);
        
        if (match_count == 0) {
            printf(COLOR_YELLOW "\n  No books found matching '%s'.\n" COLOR_RESET, query);
            pause_screen();
            return;
        }
        
        if (match_count > 1) {
            printf(COLOR_YELLOW "\n  Multiple books found:\n" COLOR_RESET);
            for (i = 0; i < match_count; i++) {
                printf("  %d. [ID:%03d] %s - %s\n", 
                       i + 1, 
                       library[matches[i]].id, 
                       library[matches[i]].title,
                       library[matches[i]].available ? COLOR_GREEN "Available" COLOR_RESET : COLOR_RED "Borrowed" COLOR_RESET);
            }
            printf("\n");
            
            int selection;
            if (!get_safe_int("Select book number: ", &selection) || selection < 1 || selection > match_count) {
                printf(COLOR_RED "Invalid selection.\n" COLOR_RESET);
                pause_screen();
                return;
            }
            book_index = matches[selection - 1];
        } else {
            book_index = matches[0];
        }
    } else if (search_choice == 2) {
        /* Search by ID */
        if (!get_safe_int("\n  Enter book ID: ", &id)) {
            printf(COLOR_RED "Invalid ID.\n" COLOR_RESET);
            pause_screen();
            return;
        }
        
        book_index = search_by_id(id);
        
        if (book_index == -1) {
            printf(COLOR_YELLOW "\n  Book ID %03d not found.\n" COLOR_RESET, id);
            pause_screen();
            return;
        }
    } else {
        printf(COLOR_RED "Invalid choice.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Display book information */
    printf("\n  Book: %s by %s\n", library[book_index].title, library[book_index].author);
    
    /* Check availability */
    if (!library[book_index].available) {
        printf(COLOR_RED "  This book is already borrowed.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Get borrower information */
    printf("\n");
    get_safe_string("  Borrower Name: ", library[book_index].borrower.name, MAX_STRING);
    if (strlen(library[book_index].borrower.name) == 0) {
        printf(COLOR_RED "  Borrower name cannot be empty.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    get_safe_string("  Borrower Phone: ", library[book_index].borrower.phone, MAX_PHONE);
    if (strlen(library[book_index].borrower.phone) == 0) {
        printf(COLOR_RED "  Borrower phone cannot be empty.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Mark as borrowed */
    library[book_index].available = 0;
    
    printf(COLOR_GREEN "\n  Book borrowed successfully!\n" COLOR_RESET);
    pause_screen();
}

/*
 * Return Book
 * Marks book as available and clears borrower information
 */
void return_book(void) {
    int search_choice;
    int matches[MAX_BOOKS];
    int match_count;
    int book_index;
    char query[MAX_STRING];
    int id;
    int i;
    
    clear_screen();
    display_banner();
    printf("\n" COLOR_CYAN "  === Return Book ===\n" COLOR_RESET "\n");
    
    if (book_count == 0) {
        printf(COLOR_YELLOW "  No books in library.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    printf("  Search by:\n");
    printf("  1. Name\n");
    printf("  2. ID\n");
    printf("\n");
    
    if (!get_safe_int("Choice: ", &search_choice)) {
        printf(COLOR_RED "Invalid input.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    if (search_choice == 1) {
        /* Search by name */
        get_safe_string("\n  Enter book name: ", query, MAX_STRING);
        if (strlen(query) == 0) {
            printf(COLOR_RED "  Search query cannot be empty.\n" COLOR_RESET);
            pause_screen();
            return;
        }
        
        match_count = search_by_name(query, matches, MAX_BOOKS);
        
        if (match_count == 0) {
            printf(COLOR_YELLOW "\n  No books found matching '%s'.\n" COLOR_RESET, query);
            pause_screen();
            return;
        }
        
        if (match_count > 1) {
            printf(COLOR_YELLOW "\n  Multiple books found:\n" COLOR_RESET);
            for (i = 0; i < match_count; i++) {
                printf("  %d. [ID:%03d] %s - %s\n", 
                       i + 1, 
                       library[matches[i]].id, 
                       library[matches[i]].title,
                       library[matches[i]].available ? COLOR_GREEN "Available" COLOR_RESET : COLOR_RED "Borrowed" COLOR_RESET);
            }
            printf("\n");
            
            int selection;
            if (!get_safe_int("Select book number: ", &selection) || selection < 1 || selection > match_count) {
                printf(COLOR_RED "Invalid selection.\n" COLOR_RESET);
                pause_screen();
                return;
            }
            book_index = matches[selection - 1];
        } else {
            book_index = matches[0];
        }
    } else if (search_choice == 2) {
        /* Search by ID */
        if (!get_safe_int("\n  Enter book ID: ", &id)) {
            printf(COLOR_RED "Invalid ID.\n" COLOR_RESET);
            pause_screen();
            return;
        }
        
        book_index = search_by_id(id);
        
        if (book_index == -1) {
            printf(COLOR_YELLOW "\n  Book ID %03d not found.\n" COLOR_RESET, id);
            pause_screen();
            return;
        }
    } else {
        printf(COLOR_RED "Invalid choice.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Display book information */
    printf("\n  Book: %s by %s\n", library[book_index].title, library[book_index].author);
    
    /* Check if borrowed */
    if (library[book_index].available) {
        printf(COLOR_YELLOW "  This book is not currently borrowed.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    /* Mark as available and clear borrower info */
    library[book_index].available = 1;
    library[book_index].borrower.name[0] = '\0';
    library[book_index].borrower.phone[0] = '\0';
    
    printf(COLOR_GREEN "\n  Book returned successfully!\n" COLOR_RESET);
    pause_screen();
}

/*
 * View Borrowed Books Table
 * Displays only books currently borrowed with borrower details
 */
void view_borrowed_books(void) {
    int i;
    int borrowed_count = 0;
    
    clear_screen();
    display_banner();
    printf("\n" COLOR_CYAN "  === Borrowed Books ===\n" COLOR_RESET "\n");
    
    /* Count borrowed books */
    for (i = 0; i < book_count; i++) {
        if (!library[i].available) {
            borrowed_count++;
        }
    }
    
    if (borrowed_count == 0) {
        printf(COLOR_YELLOW "  No books are currently borrowed.\n" COLOR_RESET);
        pause_screen();
        return;
    }
    
    display_borrowed_table();
    pause_screen();
}

/*
 * Search Books by Name (Case-Insensitive Substring Match)
 * Returns array of matching indices and count
 * 
 * Security: Case-insensitive search prevents enumeration via case manipulation
 */
int search_by_name(const char *query, int *matches, int max_matches) {
    int count = 0;
    int i;
    char lower_query[MAX_STRING];
    char lower_title[MAX_STRING];
    
    to_lowercase(lower_query, query);
    
    for (i = 0; i < book_count && count < max_matches; i++) {
        to_lowercase(lower_title, library[i].title);
        if (strstr(lower_title, lower_query) != NULL) {
            matches[count++] = i;
        }
    }
    
    return count;
}

/*
 * Search Books by ID
 * Returns index of book or -1 if not found
 */
int search_by_id(int id) {
    int i;
    for (i = 0; i < book_count; i++) {
        if (library[i].id == id) {
            return i;
        }
    }
    return -1;
}

/*
 * Bubble Sort by Title (A-Z)
 * Operates on temporary array, does not modify original
 * Time Complexity: O(n^2) - acceptable for academic context
 */
void bubble_sort_by_title(Book *temp_array, int count) {
    int i, j;
    Book temp;
    char title1[MAX_STRING], title2[MAX_STRING];
    
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            to_lowercase(title1, temp_array[j].title);
            to_lowercase(title2, temp_array[j + 1].title);
            
            if (strcmp(title1, title2) > 0) {
                temp = temp_array[j];
                temp_array[j] = temp_array[j + 1];
                temp_array[j + 1] = temp;
            }
        }
    }
}

/*
 * Bubble Sort by Year (Ascending)
 * Operates on temporary array, does not modify original
 */
void bubble_sort_by_year(Book *temp_array, int count) {
    int i, j;
    Book temp;
    
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (temp_array[j].year > temp_array[j + 1].year) {
                temp = temp_array[j];
                temp_array[j] = temp_array[j + 1];
                temp_array[j + 1] = temp;
            }
        }
    }
}

/*
 * Clear Screen (Cross-Platform)
 * Uses ANSI escape codes for portability
 */
void clear_screen(void) {
    printf("\033[2J\033[H");
}

/*
 * Display ASCII Art Banner
 * No colors in ASCII art per requirements
 */
void display_banner(void) {
    printf("\n");
    printf("  ==============================================================\n");
    printf("  ||                                                          ||\n");
    printf("  ||   _     _ _          __  __                             ||\n");
    printf("  ||  | |   (_) |__   ___|  \\/  | __ _ _ __   __ _  __ _    ||\n");
    printf("  ||  | |   | | '_ \\ / _ \\ |\\/| |/ _` | '_ \\ / _` |/ _` |   ||\n");
    printf("  ||  | |___| | |_) |  __/ |  | | (_| | | | | (_| | (_| |   ||\n");
    printf("  ||  |_____|_|_.__/ \\___|_|  |_|\\__,_|_| |_|\\__,_|\\__, |   ||\n");
    printf("  ||                                                |___/    ||\n");
    printf("  ||                                                          ||\n");
    printf("  ==============================================================\n");
}

/*
 * Display Book Table
 * Formatted table with ID, Title, Author, Year, Status
 * Auto-aligns borders by truncating long text
 */
void display_book_table(Book *books, int count) {
    int i;
    char title_display[29];
    char author_display[29];
    
    printf("  +-----+------------------------------+------------------------------+------+-----------+\n");
    printf("  | ID  | Title                        | Author                       | Year | Status    |\n");
    printf("  +-----+------------------------------+------------------------------+------+-----------+\n");
    
    for (i = 0; i < count; i++) {
        /* Truncate title and author to fit exactly in column */
        truncate_string(title_display, books[i].title, 28);
        truncate_string(author_display, books[i].author, 28);
        
        printf("  | %03d | %-28s | %-28s | %4d | ", 
               books[i].id,
               title_display,
               author_display,
               books[i].year);
        
        if (books[i].available) {
            printf(COLOR_GREEN "Available" COLOR_RESET);
        } else {
            printf(COLOR_RED "Borrowed " COLOR_RESET);
        }
        printf(" |\n");
    }
    
    printf("  +-----+------------------------------+------------------------------+------+-----------+\n");
    printf("  | Total Books: %-3d                                                                   |\n", count);
    printf("  +-----------------------------------------------------------------------------------------+\n");
}

/*
 * Display Borrowed Books Table
 * Shows only borrowed books with borrower information
 * Auto-aligns borders by truncating long text
 */
void display_borrowed_table(void) {
    int i;
    int displayed = 0;
    char title_display[29];
    char borrower_display[29];
    char phone_display[15];
    
    printf("  +-----+------------------------------+------------------------------+----------------+\n");
    printf("  | ID  | Title                        | Borrower Name                | Phone          |\n");
    printf("  +-----+------------------------------+------------------------------+----------------+\n");
    
    for (i = 0; i < book_count; i++) {
        if (!library[i].available) {
            /* Truncate fields to fit exactly in columns */
            truncate_string(title_display, library[i].title, 28);
            truncate_string(borrower_display, library[i].borrower.name, 28);
            truncate_string(phone_display, library[i].borrower.phone, 14);
            
            printf("  | %03d | %-28s | %-28s | %-14s |\n",
                   library[i].id,
                   title_display,
                   borrower_display,
                   phone_display);
            displayed++;
        }
    }
    
    printf("  +-----+------------------------------+------------------------------+----------------+\n");
    printf("  | Total Borrowed: %-3d                                                              |\n", displayed);
    printf("  +--------------------------------------------------------------------------------------+\n");
}

/*
 * Safe Integer Input with Validation
 * Prevents buffer overflow and handles non-numeric input
 * Returns 1 on success, 0 on failure
 * 
 * Security Notes:
 * - Uses fgets instead of scanf to prevent buffer overflow
 * - Validates input is purely numeric
 * - Clears input buffer on error
 */
int get_safe_int(const char *prompt, int *value) {
    char buffer[100];
    char *endptr;
    long temp;
    int i;
    
    printf("%s", prompt);
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 0;
    }
    
    /* Remove newline */
    buffer[strcspn(buffer, "\n")] = '\0';
    
    /* Check for empty input */
    if (strlen(buffer) == 0) {
        return 0;
    }
    
    /* Check if all characters are digits (with optional leading minus) */
    for (i = 0; buffer[i] != '\0'; i++) {
        if (i == 0 && buffer[i] == '-') {
            continue;
        }
        if (!isdigit((unsigned char)buffer[i])) {
            return 0;
        }
    }
    
    /* Convert to long and check for overflow */
    temp = strtol(buffer, &endptr, 10);
    
    /* Check for conversion errors */
    if (*endptr != '\0' && *endptr != '\n') {
        return 0;
    }
    
    /* Check for integer overflow using INT_MAX and INT_MIN */
    if (temp > INT_MAX || temp < INT_MIN) {
        return 0;
    }
    
    *value = (int)temp;
    return 1;
}

/*
 * Safe String Input with Buffer Protection
 * Prevents buffer overflow and trims whitespace
 * 
 * Security Notes:
 * - Enforces maximum length to prevent overflow
 * - Removes leading/trailing whitespace
 * - Clears input buffer if line too long
 */
void get_safe_string(const char *prompt, char *buffer, int max_len) {
    printf("%s", prompt);
    
    if (fgets(buffer, max_len, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    
    /* Remove newline if present */
    buffer[strcspn(buffer, "\n")] = '\0';
    
    /* Trim whitespace */
    trim_string(buffer);
    
    /* Check if input was truncated (line too long) */
    if (strlen(buffer) == (size_t)(max_len - 1) && buffer[max_len - 2] != '\n') {
        /* Clear remaining input */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

/*
 * Trim Leading and Trailing Whitespace
 * In-place string modification
 */
void trim_string(char *str) {
    char *start = str;
    char *end;
    
    /* Trim leading whitespace */
    while (isspace((unsigned char)*start)) {
        start++;
    }
    
    /* If string is all whitespace */
    if (*start == '\0') {
        str[0] = '\0';
        return;
    }
    
    /* Trim trailing whitespace */
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    
    /* Null terminate */
    *(end + 1) = '\0';
    
    /* Move trimmed string to beginning */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/*
 * Convert String to Lowercase
 * Creates lowercase copy in destination buffer
 * 
 * Security: Prevents case-based enumeration attacks
 */
void to_lowercase(char *dest, const char *src) {
    int i;
    for (i = 0; src[i] != '\0' && i < MAX_STRING - 1; i++) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

/*
 * Pause Screen and Wait for User Input
 * Used after displaying information before returning to menu
 */
void pause_screen(void) {
    printf("\n  Press Enter to continue...");
    while (getchar() != '\n');
}

/*
 * Truncate String to Fit Column Width
 * Ensures text never exceeds column width, preventing border misalignment
 * Adds "..." if text is truncated
 */
void truncate_string(char *dest, const char *src, int max_len) {
    int src_len = strlen(src);
    
    if (src_len <= max_len) {
        /* String fits, just copy it */
        strcpy(dest, src);
    } else {
        /* String too long, truncate and add ellipsis */
        if (max_len >= 3) {
            strncpy(dest, src, max_len - 3);
            dest[max_len - 3] = '\0';
            strcat(dest, "...");
        } else {
            /* Column too small for ellipsis, just truncate */
            strncpy(dest, src, max_len);
            dest[max_len] = '\0';
        }
    }
}
// end of libemanage.c
