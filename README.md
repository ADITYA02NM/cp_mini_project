# 📚 LibeManage - Library Book Management System in C

![Library](https://img.icons8.com/color/96/library.png)

## Project Title & Overview

**LibeManage** is a command-line library book management system written in pure C. It provides a complete solution for managing library books, tracking borrowers, and maintaining inventory without requiring any external dependencies or databases.

### What Problem It Solves
- Manual tracking of library books is error-prone and time-consuming
- Need for quick search, borrow/return operations without database overhead
- Academic projects require understanding of data structures, algorithms, and C programming fundamentals

### Why CLI-Based C Solution?
- **Performance**: Direct memory management, no interpreter overhead
- **Portability**: Compiles on any system with a C compiler (Windows, Linux, macOS)
- **Learning**: Demonstrates core CS concepts: structures, pointers, arrays, sorting, searching
- **Simplicity**: No external dependencies, single-file deployment
- **Real-world skills**: Input validation, memory safety, edge case handling

---

## Key Features ✨

- ✅ **View All Books** - Display complete library inventory in formatted tables
- ✅ **Temporary Bubble Sorting** - Sort by title (A-Z) or year (ascending) without modifying original data
- ✅ **Add Books** - Add new books with validation (title, author, year)
- ✅ **Remove Books** - Delete books with confirmation and automatic ID renormalization
- ✅ **Borrow Books** - Mark books as borrowed and store borrower details
- ✅ **Return Books** - Mark books as available and clear borrower information
- ✅ **Borrowed Books Table** - View only currently borrowed books with borrower details
- ✅ **Dual Search** - Search by name (case-insensitive substring) or ID (exact match)
- ✅ **Auto ID Normalization** - Maintains sequential IDs (001, 002, 003...) with no gaps
- ✅ **Input Validation** - Prevents crashes from invalid numeric/string input
- ✅ **Colored CLI** - ANSI colors for status (Available/Borrowed) and menu highlights
- ✅ **Auto-Aligned Tables** - Dynamic text truncation ensures perfect border alignment
- ✅ **Preloaded Data** - 10 books (5 cybersecurity + 5 engineering) on startup

---

## Project Architecture & Data Design 🏗️

### Data Structures

#### `Borrower` Structure
```c
typedef struct {
    char name[MAX_STRING];   // Borrower's full name (max 100 chars)
    char phone[MAX_PHONE];   // Contact number (max 20 chars)
} Borrower;
```
**Purpose**: Stores borrower information when a book is checked out. Nested inside `Book` structure for efficient data coupling.

#### `Book` Structure
```c
typedef struct {
    int id;                  // Unique sequential ID (auto-managed)
    char title[MAX_STRING];  // Book title (max 100 chars)
    char author[MAX_STRING]; // Author name (max 100 chars)
    int year;                // Publication year (validated 1000-9999)
    int available;           // Status: 1 = available, 0 = borrowed
    Borrower borrower;       // Nested borrower info (empty if available)
} Book;
```

**Field Explanations**:
- `id`: Automatically normalized after add/remove operations
- `title` & `author`: Fixed-size arrays prevent buffer overflow
- `year`: Validated to be 4-digit integer
- `available`: Boolean flag (1/0) for quick status checks
- `borrower`: Only populated when `available = 0`

### Why Arrays?
```c
Book library[MAX_BOOKS];  // Global array of 100 books
int book_count = 0;        // Tracks current number of books
```

**Rationale**:
- **Static allocation**: No malloc/free complexity, no memory leaks
- **Cache-friendly**: Contiguous memory improves access speed
- **Academic clarity**: Easier to explain and debug than linked lists
- **Bounded capacity**: 100 books is reasonable for small library/demo purposes
- **Direct indexing**: O(1) access by position

### Global State Management
```c
Book library[MAX_BOOKS];  // Stores all book records
int book_count = 0;       // Current number of books (0-100)
```

**Why Global?**
- Simplifies function signatures (no need to pass library pointer everywhere)
- Single source of truth for library state
- Acceptable for single-threaded CLI application
- Avoids complex pointer-to-pointer passing

---

## Execution Flow (Step-by-Step) 🔄

### Program Startup
```
1. main() starts
2. initialize_library() loads 10 books
3. normalize_ids() assigns IDs 001-010
4. Infinite menu loop begins (while(1))
```

### Main Menu Loop
```
Display banner → Show 7 options → Get user choice → Execute function → Repeat
```

### How Each Option Works

**Option 1: View Books**
- Shows table → Asks for sort option → Creates temp copy → Sorts temp → Display → Original unchanged

**Option 2: Add Book**
- Check capacity → Get title → Get author → Get year → Validate → Add to array → Renumber IDs

**Option 3: Remove Book**
- Search → Display match → Shift array left → Decrement count → Renumber IDs

**Option 4: Borrow Book**
- Search → Check available → Get borrower info → Set `available=0` → Store borrower

**Option 5: Borrowed Books**
- Loop through all books → Print only where `available=0` → Show borrower details

**Option 6: Return Book**
- Search → Check borrowed → Set `available=1` → Clear borrower data

**Option 7: Exit**
- Clear screen → Print goodbye → `return 0` (exit program)

---

## Sorting & Searching Logic 🔍

### Bubble Sort Explained

**How It Works:**
```
Compare adjacent items → Swap if wrong order → Repeat until sorted
```

**Simple Example:**
```
[3, 1, 2]  →  Compare 3 and 1 → Swap  →  [1, 3, 2]
[1, 3, 2]  →  Compare 3 and 2 → Swap  →  [1, 2, 3]  ✓ Sorted
```

**Code Pattern:**
```c
for (i = 0; i < count - 1; i++) {           // Outer loop: passes
    for (j = 0; j < count - i - 1; j++) {   // Inner loop: comparisons
        if (array[j] > array[j+1]) {
            swap(array[j], array[j+1]);     // Swap if out of order
        }
    }
}
```

### Why Sorting is Temporary

**Problem:** User wants sorted view but needs to keep original order.

**Solution:**
```c
Book temp_array[MAX_BOOKS];              // Create temporary array
for (i = 0; i < book_count; i++) {
    temp_array[i] = library[i];           // Copy all books
}
bubble_sort_by_title(temp_array, book_count);  // Sort the COPY
// Original library[] unchanged!
```

### Case-Insensitive Search

**Why?** User might type "CRYPTO" but book is "Cryptography"

**How:**
```c
to_lowercase(query, "CRYPTO");      // → "crypto"
to_lowercase(title, "Cryptography");  // → "cryptography"
strstr("cryptography", "crypto");     // ✓ Match found!
```

### Search by Name vs ID

| Feature | By Name | By ID |
|---------|---------|-------|
| Match Type | Partial (substring) | Exact |
| Case Sensitive | No | N/A |
| Multiple Results | Yes (show list) | No (unique ID) |
| User Input | "crypto" | 5 |

---

## Input Validation & Stability 🛡️

### How Crashes are Prevented

| Problem | Solution |
|---------|----------|
| Buffer overflow | `fgets(buffer, MAX_SIZE, ...)` limits input |
| Non-numeric input | Check each character with `isdigit()` |
| Empty strings | Check `strlen() == 0` before processing |
| Integer overflow | Compare with `INT_MAX` and `INT_MIN` |
| Invalid menu choice | Use `default:` in switch-case |

### Key Safety Features

**fgets vs scanf:**
```c
// UNSAFE ❌
scanf("%s", title);  // No size limit!

// SAFE ✅  
fgets(title, 100, stdin);  // Max 100 chars
```

**Empty Input Check:**
```c
if (strlen(title) == 0) {
    printf("Error: Title cannot be empty\n");
    return;  // Don't crash, just exit function
}
```

**Invalid Number Check:**
```c
// Check each character is a digit
for (i = 0; buffer[i] != '\0'; i++) {
    if (!isdigit(buffer[i])) {
        return 0;  // Invalid, return error code
    }
}
```

---

## Borrow / Return Workflow 📖

### Borrowing a Book (Simplified)

```
1. Search for book (name/ID)
2. Check if available (available == 1)
   → If borrowed: Show error, exit
3. Get borrower name
4. Get borrower phone
5. Update:
   - available = 0
   - borrower.name = "John Doe"
   - borrower.phone = "555-1234"
6. Done!
```

**Before Borrow:**
```c
available: 1          // Available
borrower.name: ""     // Empty
borrower.phone: ""    // Empty
```

**After Borrow:**
```c
available: 0               // Borrowed
borrower.name: "John Doe"  // Stored
borrower.phone: "555-1234" // Stored
```

### Returning a Book (Simplified)

```
1. Search for book
2. Check if borrowed (available == 0)
   → If not borrowed: Show warning, exit
3. Update:
   - available = 1
   - borrower.name = ""  (clear)
   - borrower.phone = "" (clear)
4. Done!
```

**How Data is Cleared:**
```c
library[index].borrower.name[0] = '\0';   // First char = null terminator
library[index].borrower.phone[0] = '\0';  // Makes string empty ""
```

---

## C Programming Concepts Used 📘

This project demonstrates the following 1st year engineering C topics:

### Basic Concepts
- **Variables & Data Types**: `int`, `char`, arrays
- **Preprocessor Directives**: `#include`, `#define` for constants
- **Standard Libraries**: `stdio.h`, `string.h`, `ctype.h`, `stdlib.h`, `limits.h`

### Control Structures
- **Loops**: `for`, `while` loops for iteration
- **Conditionals**: `if-else`, `switch-case` for decision making
- **Break/Continue**: Loop control statements

### Functions
- **Function Declaration & Definition**: Prototypes and implementations
- **Parameter Passing**: By value and by address (pointers)
- **Return Types**: `void`, `int` functions

### Arrays & Strings
- **1D Arrays**: `Book library[MAX_BOOKS]`
- **Character Arrays**: String manipulation with `char title[100]`
- **Array Indexing**: Accessing elements with `library[i]`
- **String Functions**: `strcpy()`, `strcmp()`, `strlen()`, `strcat()`, `strstr()`

### Structures
- **struct definition**: `typedef struct` for Book and Borrower
- **Nested structures**: Borrower inside Book
- **Structure arrays**: Array of Book structures
- **Member access**: Using dot operator (`.`)

### Pointers
- **Pointer basics**: Function parameters with `*`
- **Array-pointer relationship**: Passing arrays to functions
- **String pointers**: Working with character pointers

### File Organization
- **Global variables**: `library[]`, `book_count`
- **Function prototypes**: Organized declarations
- **Modular code**: Separate functions for each task

---

## Function Reference (One-Liner Explanations) 🔧

### Main Program Flow
| Function | Purpose | Concept Used |
|----------|---------|--------------|
| `main()` | Entry point, runs menu loop until exit | while loop, switch-case |
| `initialize_library()` | Loads 10 preloaded books at startup | Arrays, strcpy, structures |
| `normalize_ids()` | Renumbers all book IDs sequentially (1, 2, 3...) | for loop, array indexing |

### Core Features
| Function | Purpose | Concept Used |
|----------|---------|--------------|
| `view_all_books()` | Displays table of books with sorting sub-menu | Nested loops, function calls |
| `add_book()` | Adds new book after validating inputs | Input validation, array manipulation |
| `remove_book()` | Deletes book and shifts array to fill gap | Array shifting, for loop |
| `borrow_book()` | Marks book unavailable and stores borrower info | Nested structures, string copy |
| `return_book()` | Marks book available and clears borrower data | Structure member access, string manipulation |
| `view_borrowed_books()` | Shows only borrowed books in a table | Conditional logic, for loop with if |

### Search Functions
| Function | Purpose | Concept Used |
|----------|---------|--------------|
| `search_by_name()` | Finds books matching partial name (case-insensitive) | String functions (strstr), to_lowercase |
| `search_by_id()` | Finds exact book by ID number | Linear search, for loop |

### Sorting Functions (Bubble Sort)
| Function | Purpose | Concept Used |
|----------|---------|--------------|
| `bubble_sort_by_title()` | Sorts books alphabetically A-Z | Nested for loops, strcmp, swapping |
| `bubble_sort_by_year()` | Sorts books by year (oldest first) | Nested for loops, comparison, swapping |

**Key Point**: Sorting creates a **temporary copy** of the array, so original data is never modified.

```c
// Example: Creating temp array before sorting
Book temp_array[MAX_BOOKS];
for (i = 0; i < book_count; i++) {
    temp_array[i] = library[i];  // Copy each book
}
bubble_sort_by_title(temp_array, book_count);  // Sort the copy
```

### Input Validation (Crash Prevention)
| Function | Purpose | Concept Used |
|----------|---------|--------------|
| `get_safe_int()` | Reads integer safely without crashes | fgets, validation loop, isdigit, strtol |
| `get_safe_string()` | Reads string with buffer overflow protection | fgets with size limit, strcspn |
| `trim_string()` | Removes spaces from start/end of strings | while loop, isspace, pointer arithmetic |
| `to_lowercase()` | Converts string to all lowercase | for loop, tolower function |

**Why fgets instead of scanf?**
```c
// UNSAFE (can overflow buffer):
scanf("%s", title);  // ❌ No size limit

// SAFE (limits input size):
fgets(title, MAX_STRING, stdin);  // ✅ Max 100 chars
```

### Display Functions
| Function | Purpose | Concept Used |
|----------|---------|--------------|
| `display_banner()` | Shows ASCII art title | printf, escape characters |
| `display_book_table()` | Prints formatted table of books | for loop, printf formatting, conditional (if-else) |
| `display_borrowed_table()` | Prints table of only borrowed books | for loop with if condition |
| `truncate_string()` | Shortens long text to fit table columns | String manipulation, strncpy, strcat |
| `clear_screen()` | Clears terminal screen | ANSI escape codes |
| `pause_screen()` | Waits for Enter key before continuing | while loop, getchar |

---

## Compilation & Execution ⚙️

### How to Compile

**Basic Compilation** (recommended):
```bash
gcc libemanage.c -o libemanage
```

**With Warning Flags** (for development):
```bash
gcc libemanage.c -o libemanage -Wall -Wextra
```

**Cross-Platform Notes**:
- **Linux/macOS**: Use `gcc` or `clang`
- **Windows (MinGW)**: Use `gcc` from MinGW installation
- **Windows (MSVC)**: Use `cl libemanage.c` (may need minor adjustments for ANSI colors)

### How to Run

**Linux/macOS**:
```bash
./libemanage
```

**Windows (Command Prompt)**:
```cmd
libemanage.exe
```

**Windows (PowerShell)**:
```powershell
.\libemanage.exe
```

### Expected Behavior After Running

1. **Screen clears** and displays ASCII banner
2. **Main menu appears** with 7 options
3. **Preloaded books** are already in the system (10 books)
4. **User can navigate** using number keys (1-7)
5. **Program loops** until user selects "7. Exit"

**First Screen**:
```
==============================================================
||   _     _ _          __  __                             ||
||  | |   (_) |__   ___|  \/  | __ _ _ __   __ _  __ _    ||
||  | |   | | '_ \ / _ \ |\/| |/ _` | '_ \ / _` |/ _` |   ||
||  | |___| | |_) |  __/ |  | | (_| | | | | (_| | (_| |   ||
||  |_____|_|_.__/ \___|_|  |_|\__,_|_| |_|\__,_|\__, |   ||
||                                                |___/    ||
==============================================================

  1. View all books
  2. Add book
  3. Remove book
  4. Borrow book
  5. Borrowed books table
  6. Return book
  7. Exit

Choice:
```

---

## Demo Guide (How to Explain in Class) 🎓

### Suggested Order of Actions for Demo

**Phase 1: Show Existing Data (2 minutes)**
1. Run program → Show main menu
2. Select **"1. View all books"** → Display 10 preloaded books
3. Select **"1. Sort by name"** → Show sorted view
4. Press Enter → Select **"2. Sort by year"** → Show year-sorted view
5. Press Enter → Select **"3. Back to main menu"**

**What to Say**:
> "The program starts with 10 preloaded books - 5 cybersecurity and 5 engineering. Notice the IDs are sequential from 001 to 010. When we sort by name or year, it creates a temporary copy of the data, so the original order is preserved. This demonstrates our understanding of array manipulation and bubble sort algorithms."

---

**Phase 2: Add & Remove Operations (3 minutes)**
1. Select **"2. Add book"**
2. Enter: Title = "Clean Code", Author = "Robert Martin", Year = 2008
3. View all books → Show new book with ID 011
4. Select **"3. Remove book"**
5. Choose search by name → Enter "Clean Code"
6. Confirm removal
7. View all books → Show ID normalization (IDs now 001-010 again)

**What to Say**:
> "Adding a book increments the count and assigns the next available ID. When we remove a book, the array shifts left to fill the gap, and normalize_ids() ensures there are no gaps in the ID sequence. This demonstrates dynamic array management and the importance of maintaining data integrity."

---

**Phase 3: Borrow/Return Workflow (4 minutes)**
1. Select **"4. Borrow book"**
2. Search by name → "crypto" → Shows both cryptography books
3. Select first match → Enter borrower: Name = "John Doe", Phone = "555-1234"
4. Select **"5. Borrowed books table"** → Show borrowed book with borrower details
5. View all books → Show "Borrowed" status in red
6. Select **"6. Return book"** → Search by ID → Enter 001
7. Confirm return → View all books → Status back to green "Available"

**What to Say**:
> "The borrow function demonstrates our nested Borrower structure. We search for the book using case-insensitive matching - 'crypto' matches 'Cryptography'. After borrowing, the book's status changes to 0 (borrowed) and borrower information is stored. The borrowed books table filters and displays only checked-out items. Returning clears the borrower data and sets status back to 1 (available)."

---

**Phase 4: Input Validation Demo (2 minutes)**
1. From main menu → Enter "abc" → Show error handling
2. Select add book → Leave title empty → Show validation error
3. Select add book → Enter year as "12" → Show year validation error
4. Select search → Enter invalid choice → Show error and re-prompt

**What to Say**:
> "Notice how the program never crashes. We use fgets() instead of scanf() to prevent buffer overflow. All numeric inputs are validated character-by-character. Empty strings are caught before processing. Invalid menu choices don't crash the program - they show an error and re-prompt. This demonstrates defensive programming and security-conscious coding practices."

---

### Key Technical Points to Emphasize to Examiner

| Topic | What to Highlight |
|-------|------------------|
| **Data Structures** | "We use nested structures - Borrower inside Book - for logical data coupling. This is more memory-efficient than separate parallel arrays." |
| **Memory Management** | "Static array allocation means no malloc/free complexity, no memory leaks. The MAX_BOOKS constraint is acceptable for a library management system." |
| **Sorting Algorithm** | "Bubble sort is O(n²) but optimal for small datasets and easy to understand. We sort a temporary copy to preserve original insertion order." |
| **Search Efficiency** | "Linear search is O(n) but appropriate for unsorted data. Case-insensitive substring matching makes the system user-friendly." |
| **Input Security** | "We use fgets() with size limits, validate every input, and check for integer overflow using INT_MAX/INT_MIN from limits.h." |
| **Code Organization** | "Functions are single-responsibility. Each function has a clear purpose, making the code maintainable and easy to debug." |
| **Global State** | "The library array is global to avoid passing pointers through every function. This is acceptable for single-threaded CLI applications." |
| **ID Normalization** | "After add/remove, we renumber all IDs to maintain sequential order with no gaps. This prevents confusion and ensures data consistency." |

---

## Limitations & Future Enhancements 🚀

### Current Limitations

| Limitation | Impact |
|-----------|--------|
| **No File Persistence** | Data lost when program exits - all books reset to initial 10 |
| **Max 100 Books** | Hard limit due to fixed-size array |
| **No Multi-User Support** | Only one person can use the system at a time |
| **Linear Search Only** | Slower for large datasets (not an issue with 100 books) |
| **No Date Tracking** | Doesn't record when book was borrowed/returned |
| **No Due Date System** | No overdue book tracking or late fees |
| **No Book Categories** | Can't filter by genre, subject, or department |
| **No User Authentication** | Anyone can borrow/return any book |

### Future Enhancements

#### 1. File Persistence
**Implementation**:
```c
void save_library_to_file(void) {
    FILE *fp = fopen("library.dat", "wb");
    fwrite(&book_count, sizeof(int), 1, fp);
    fwrite(library, sizeof(Book), book_count, fp);
    fclose(fp);
}

void load_library_from_file(void) {
    FILE *fp = fopen("library.dat", "rb");
    if (fp != NULL) {
        fread(&book_count, sizeof(int), 1, fp);
        fread(library, sizeof(Book), book_count, fp);
        fclose(fp);
    }
}
```

**Benefits**: Data persists between sessions, no need to re-enter books.

---

#### 2. Advanced Sorting Options
- **Quick Sort**: O(n log n) for larger datasets
- **Merge Sort**: Stable sorting for tie-breaking scenarios
- **Multi-field Sort**: Sort by author, then by year

**Example**:
```c
void quick_sort_by_title(Book *arr, int low, int high) {
    // Implement quicksort with pivot selection
}
```

---

#### 3. Statistics Dashboard
**Features**:
- Total books vs. available vs. borrowed
- Most borrowed books
- Average year of publication
- Borrowing trends

**Example Display**:
```
=== Library Statistics ===
Total Books: 100
Available: 73
Borrowed: 27
Most Borrowed: "Applied Cryptography" (15 times)
Average Publication Year: 2010
```

---

#### 4. Category/Genre System
**Structure Enhancement**:
```c
typedef struct {
    int id;
    char title[MAX_STRING];
    char author[MAX_STRING];
    int year;
    char category[50];  // NEW: "Cybersecurity", "Engineering", etc.
    int available;
    Borrower borrower;
} Book;
```

**New Feature**: Filter books by category in view menu.

---

#### 5. Due Date System
**Enhancement**:
```c
typedef struct {
    char name[MAX_STRING];
    char phone[MAX_PHONE];
    char borrow_date[11];   // "YYYY-MM-DD"
    char due_date[11];      // "YYYY-MM-DD"
    int is_overdue;         // 1 if overdue, 0 otherwise
} Borrower;
```

**Benefits**: Track overdue books, calculate late fees.

---

#### 6. User Authentication
**Implementation**:
```c
typedef struct {
    char username[50];
    char password_hash[65];  // SHA-256 hash
    int role;  // 0 = student, 1 = librarian, 2 = admin
} User;
```

**Features**:
- Librarians can add/remove books
- Students can only borrow/return
- Admins have full access

---

#### 7. Binary Search (Requires Sorted Data)
**Prerequisite**: Keep library sorted by ID or title.

**Implementation**:
```c
int binary_search_by_id(int id) {
    int low = 0, high = book_count - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (library[mid].id == id) return mid;
        if (library[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}
```

**Benefit**: O(log n) search instead of O(n).

---

#### 8. Export to CSV
**Feature**: Export library data to Excel-compatible format.

**Implementation**:
```c
void export_to_csv(void) {
    FILE *fp = fopen("library.csv", "w");
    fprintf(fp, "ID,Title,Author,Year,Status\n");
    for (int i = 0; i < book_count; i++) {
        fprintf(fp, "%03d,%s,%s,%d,%s\n",
                library[i].id,
                library[i].title,
                library[i].author,
                library[i].year,
                library[i].available ? "Available" : "Borrowed");
    }
    fclose(fp);
}
```

---

## Author & License 📝

**Author**: Aditya Gowda  
**Project**: LibeManage - Library Book Management System  
**Language**: C (C99 Standard)  
**Created**: 17 january 2025  
**Course**: Programming in c  
**Institution**: Banglore Institute of Technology - CSE(ICB)

### License

This project is open-source and available under the **MIT License**.

```
MIT License

Copyright (c) 2025 Aditya Gowda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

### Acknowledgments

- **Bubble Sort Algorithm**: Based on standard computer science curriculum
- **Input Validation Techniques**: Inspired by secure coding best practices
- **ANSI Color Codes**: Standard terminal escape sequences

---

### Contact

For questions, suggestions, or bug reports:
- **Email**: adityagowda02@gmail.com
- **GitHub**: https://github.com/ADITYA02NM/cp_mini_project

---

**⭐ If you found this project helpful, please consider giving it a star on GitHub!**

---

*End of README.md*
