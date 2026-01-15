# 📚 LibeManage - Library Book Management System in C

![Library](https://img.icons8.com/color/96/library.png)

**LibeManage** is a **command-line library book management system** written in **C**, designed to manage books, borrowers, and transactions efficiently. This project demonstrates **structured programming, arrays, structs, pointers, bubble sorting, and input validation** in C.

---

## 🌟 Features

- Add, remove, and view books
- Borrow and return books with **borrower details** (name & phone)
- Case-insensitive **search by name or ID**
- **Temporary sorting** by Name (A–Z) or Year
- Auto-maintained **Book IDs**
- Borrowed books table
- Input validation to prevent crashes
- Simple, clear **ASCII CLI interface**
- Preloaded with **5 Cybersecurity** and **5 Engineering** books
- Maximum capacity of **100 books**
- Colored terminal output for better UX

---

## 🛠 Installation

1. Clone this repository:
```bash
git clone https://github.com/yourusername/libemanage.git
cd libemanage
````

2. Compile the program (Windows PowerShell / Linux):

```bash
gcc libemanage.c -o libemanage
```

3. Run:

```bash
.\libemanage   # Windows
./libemanage    # Linux/Mac
```

---

## 🎮 How to Use

On running the program, you will see the **LibeManage ASCII banner** and main menu:

```
1. View All Books
2. Add Book
3. Remove Book
4. Borrow Book
5. Borrowed Table
6. Return Book
7. Exit
```

### 1️⃣ View All Books

* Displays all books with **ID, Title, Author, Year, Status**
* Option to **temporarily sort** by:

  * Name (A–Z)
  * Year

```c
// Example
showBooks(library, totalBooks);
tempSort(1); // Sort by Name
tempSort(2); // Sort by Year
```

### 2️⃣ Add Book

* Enter **Book Name, Author, Year**
* Book is added and **IDs normalized automatically**

```c
Book b;
strcpy(b.title, "New Book");
strcpy(b.author, "Author Name");
b.year = 2023;
b.available = 1;
library[totalBooks++] = b;
normalizeIDs();
```

### 3️⃣ Remove Book

* Search by **Name** or **ID**
* Remove the book
* Remaining books **IDs re-assigned sequentially**

```c
int idx = findByID(3);
if (idx != -1) {
    // shift array to remove book
}
normalizeIDs();
```

### 4️⃣ Borrow Book

* Search by **Name** or **ID**
* Enter **Borrower Name & Phone**
* Book status changes to **Borrowed**

```c
library[idx].available = 0;
strcpy(library[idx].borrower.name, "John Doe");
strcpy(library[idx].borrower.phone, "1234567890");
```

### 5️⃣ Borrowed Table

* Displays **borrowed books** with borrower info

```c
for (int i = 0; i < totalBooks; i++)
    if (!library[i].available)
        printBook(&library[i]);
```

### 6️⃣ Return Book

* Search by **Name** or **ID**
* Marks book as **Available** and clears borrower info

```c
library[idx].available = 1;
strcpy(library[idx].borrower.name, "");
strcpy(library[idx].borrower.phone, "");
```

---

## ⚡ Technical Details

* Language: **C**
* Concepts used:

  * `struct` and `union`
  * Arrays & pointers
  * Bubble sorting
  * Case-insensitive string search
  * Defensive input handling
  * ASCII art interface
  * ANSI color codes for terminal
* Max books: 100
* Single-file program: `libemanage.c`

---

## 🧠 Notes

* Sorting is **temporary**; original order of `library[]` remains unchanged.
* IDs always reassign after adding/removing books.
* Borrower info is **linked to the book** only.
* Preloaded books ensure the system has **demo data** on first run.

---

## 🎯 Future Improvements

* File persistence (save/load books and borrowers)
* Dashboard with borrow statistics
* Advanced sorting & filtering
* Multi-user roles
* Improved CLI graphics

---

## 👩‍💻 Author

Aditya Gowda – Engineering Student / Developer
GitHub: ADITYA02NM (https://github.com/ADITYA02NM)

---



