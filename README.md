# Student Attendance Tracing System

A command-line based student attendance system built with C and MySQL. The program lets you add students, mark their attendance (single or all at once), view a full attendance report with percentage, search for individual student records, and delete students from the system. All data is stored in a MySQL database so it persists between sessions.

---

## Table of Contents

- [Project Structure](#project-structure)
- [How to Build and Run](#how-to-build-and-run)
- [Prerequisites](#prerequisites)
- [Features](#features)
- [File-by-File Explanation](#file-by-file-explanation)
  - [include/functions.h](#includefunctionsh)
  - [database/db.c](#databasedbc)
  - [functions/addstudent.c](#functionsaddstudentc)
  - [functions/markattendance.c](#functionsmarkattendancec)
  - [functions/showall.c](#functionsshowallc)
  - [functions/markall.c](#functionsmarkallc)
  - [functions/searchstudent.c](#functionssearchstudentc)
  - [functions/deletestudent.c](#functionsdeletestudentc)
  - [main.c](#mainc)
- [Database Schema](#database-schema)
- [Concepts Used](#concepts-used)
- [Possible Questions and Answers](#possible-questions-and-answers)

---

## Project Structure

```
student-attendance-tracing-system/
├── main.c                      # menu loop and program entry point
├── include/
│   └── functions.h             # header file with function declarations
├── database/
│   └── db.c                    # MySQL connection setup and close
├── functions/
│   ├── addstudent.c            # add a new student to database
│   ├── markattendance.c        # mark attendance for a single student
│   ├── markall.c               # mark attendance for all students
│   ├── showall.c               # display all students with attendance percentage
│   ├── searchstudent.c         # search a student by ID and show their report
│   └── deletestudent.c         # remove a student from database
└── README.md
```

The project is organized into four parts. `main.c` handles the menu loop and user interaction only. `database/db.c` handles MySQL connection setup and teardown. Each feature function lives in its own file inside `functions/` — one file per function. `include/functions.h` is the header file that declares all functions and the shared database connection variable so every file can use them.

---

## How to Build and Run

```bash
gcc main.c database/db.c functions/addstudent.c functions/markattendance.c functions/showall.c functions/markall.c functions/searchstudent.c functions/deletestudent.c $(mysql_config --cflags --libs) -o attendance
./attendance
```

## Prerequisites

1. MySQL server must be installed and running on your system.
2. MySQL development library (C client headers) must be installed.

On Ubuntu/Debian:
```bash
sudo apt update
sudo apt install default-libmysqlclient-dev
sudo service mysql start
```

If your MySQL root user has a password, change the `pass[]` variable in `database/db.c` to match it:
```c
char pass[] = "your_password_here";
```

---

## Features

1. **Add Student** — Register a new student with an ID and name into the system.
2. **Mark Attendance (Single)** — Find a student by ID and mark them as present or absent for the day.
3. **Mark Attendance (All)** — Go through every student one by one and mark each as present or absent.
4. **View All Students** — Display a formatted table showing every student's ID, name, total classes, number of presents, and attendance percentage.
5. **Search Student** — Look up any student by their ID to see their individual attendance report.
6. **Delete Student** — Remove a student and all their attendance records from the database.
7. **Persistent Storage** — All data is saved in MySQL automatically. Closing and reopening the program preserves everything.

---

## File-by-File Explanation

### include/functions.h

This is the header file. Its job is to declare all the functions and shared variables so that other `.c` files can use them.

At the top, `#ifndef FUNCTIONS_H` and `#define FUNCTIONS_H` form what is called an include guard. When the compiler processes this file for the first time, `FUNCTIONS_H` is not defined yet so the condition is true and everything inside gets processed. The `#define FUNCTIONS_H` line then defines it. If another file tries to include this header a second time, `FUNCTIONS_H` is already defined so `#ifndef` becomes false and the entire file content is skipped. This prevents duplicate declaration errors during compilation.

The `#include <mysql/mysql.h>` line brings in the MySQL C library header which defines types like `MYSQL`, `MYSQL_RES`, `MYSQL_ROW` and function prototypes like `mysql_init()`, `mysql_query()`, `mysql_real_connect()`, etc. This is needed here because the header declares `extern MYSQL *conn` which uses the `MYSQL` type.

The line `extern MYSQL *conn` tells the compiler that a variable named `conn` of type `MYSQL *` exists somewhere in one of the `.c` files. The `extern` keyword means "this variable is defined elsewhere, do not create a new one." The actual definition is in `database/db.c` where it says `MYSQL *conn;` without extern. Without the extern declaration in the header, every file that includes this header would try to create its own `conn` variable and the linker would fail with a "multiple definition" error.

The remaining lines are function declarations (also called prototypes). Each one tells the compiler the return type, name, and parameters of a function that is implemented in another file. For example `void connectdb()` tells the compiler that somewhere there is a function called `connectdb` that returns void and takes no parameters. When `main.c` calls `connectdb()`, the compiler uses this declaration to verify the call is correct even though the actual code for the function is in `database/db.c`.

Finally `#endif` closes the include guard that was opened by `#ifndef FUNCTIONS_H` at the top.

---

### database/db.c

This file has two responsibilities: connecting to MySQL when the program starts and closing the connection when it exits.

At the top it includes the standard headers `stdio.h`, `stdlib.h`, `string.h` and the MySQL header `mysql/mysql.h`. It also includes `../include/functions.h` using a relative path — the `../` goes up one directory from `database/` to the project root, then into `include/`.

The variable `MYSQL *conn` is the actual definition of the database connection pointer that was declared as `extern` in the header. It is defined at file scope (outside any function) making it a global variable accessible from all files that include the header.

The variables `host`, `user`, `pass`, and `dbname` store the MySQL connection credentials. `host` is set to `"localhost"` meaning the MySQL server is on the same machine. `user` is `"root"` which is the default MySQL admin user. `pass` is empty string because most local MySQL installations have no password for root by default. `dbname` is `"attendance_db"` which is the database this program creates and uses.

The `connectdb()` function performs a two-step connection process. First, it calls `mysql_init(NULL)` which allocates and initializes a `MYSQL` connection object and returns a pointer to it. If `mysql_init` returns NULL it means memory allocation failed, so the program prints an error and exits with code 1.

Next, `mysql_real_connect()` is called with 8 parameters: the connection object, host, username, password, database name (NULL here because we are not connecting to a specific database yet), port (0 means use default port 3306), unix socket (NULL means use default), and client flags (0 means no special flags). If this returns NULL the connection failed and the program prints the error using `mysql_error(conn)` which returns a human-readable error string.

Then `mysql_query()` runs `CREATE DATABASE IF NOT EXISTS attendance_db;` which creates the database only if it does not already exist. The `IF NOT EXISTS` part is important because running this on subsequent program starts would cause an error without it. After creating the database, `mysql_close()` closes this first connection.

Now the function needs to connect again, but this time with the database name specified as the fourth parameter of `mysql_real_connect()`. This is because you cannot create a database while already connected to one through the MySQL C API. After this second connection is established, `mysql_query()` runs a `CREATE TABLE IF NOT EXISTS students(...)` statement that creates the students table with four columns:
- `id INT PRIMARY KEY` — the student's unique ID number, primary key means no duplicates allowed
- `name VARCHAR(50)` — the student's name, up to 50 characters
- `total INT DEFAULT 0` — total number of classes the student has been marked for
- `present INT DEFAULT 0` — number of times the student was marked present

The `closedb()` function simply calls `mysql_close(conn)` which properly closes the MySQL connection and frees the connection object's memory. This is called when the user selects 0 to exit the program.

---

### functions/addstudent.c

This file contains the `addstudent()` function which inserts a new student record into the database.

The function declares three local variables: `sid` (integer for student ID), `name` (character array of size 50 for the student's name), and `sql` (character array of size 256 to hold the SQL query string).

It first prompts the user with `printf("Enter student ID: ")` and reads the integer using `scanf("%d", &sid)`. Then it prompts for the name and reads it using `scanf(" %[^\n]", name)`. The format specifier `%[^\n]` tells scanf to read every character until it encounters a newline character (the Enter key press). Without this, a regular `scanf("%s")` would stop reading at the first space, so a name like "Rahim Uddin" would only capture "Rahim". The space before `%[^\n]` is also important — it tells scanf to skip any leftover whitespace or newline characters that are still in the input buffer from the previous `scanf("%d")` call.

Next, `sprintf()` is used to build the SQL query string dynamically. The format string `"INSERT INTO students(id, name, total, present) VALUES(%d, '%s', 0, 0);"` contains two placeholders: `%d` gets replaced with the value of `sid` and `%s` gets replaced with the value of `name`. The result is something like `INSERT INTO students(id, name, total, present) VALUES(101, 'Rahim Uddin', 0, 0);`. The total and present columns start at 0 because the student was just added and has no attendance records yet.

Then `mysql_query(conn, sql)` executes the query against the database. It returns 0 on success and a non-zero value on failure. The `if (mysql_query(conn, sql) == 0)` check prints "Student added." on success, otherwise it prints the error message using `mysql_error(conn)`.

---

### functions/markattendance.c

This file contains the `markattendance()` function which marks attendance for a single student by their ID.

The function starts by declaring `sid` (student ID to search for), `status` (whether the student is present or absent), `query` (string buffer for SQL queries), `res` (a pointer to a `MYSQL_RES` result set structure), and `row` (a `MYSQL_ROW` which is an array of strings representing one row of query results).

First it asks for a student ID and builds a SELECT query using `sprintf()`: `SELECT name FROM students WHERE id = %d`. This query searches for a student with the given ID and returns only their name. The `WHERE id = %d` clause is the filter condition that limits results to matching rows only.

`mysql_query(conn, query)` executes the SELECT query. Then `mysql_store_result(conn)` retrieves the entire result set from the server and stores it in client memory as a `MYSQL_RES` structure. `mysql_fetch_row(res)` returns the first row as a `MYSQL_ROW`. A `MYSQL_ROW` is essentially an array of `char *` (string pointers), where each element represents one column value from the row. Since the SELECT only asked for one column (name), `row[0]` contains the student name string.

The `if (row != NULL)` check determines whether a matching student was found. If `row` is NULL, it means the SELECT query returned no rows — the student ID does not exist in the database. In that case `mysql_free_result(res)` frees the allocated result memory and the function prints "Student not found."

If the student is found, the function prints their name, frees the result set since we no longer need it, and then asks whether to mark them present (1) or absent (0). Based on the answer, it builds one of two UPDATE queries using `sprintf()`:
- If present: `UPDATE students SET total = total + 1, present = present + 1 WHERE id = %d` — increments both total and present by 1
- If absent: `UPDATE students SET total = total + 1 WHERE id = %d` — increments only total by 1

The SQL expression `total = total + 1` is important — it reads the current value of the `total` column and adds 1 to it in a single atomic operation. This is better than reading the value into a C variable, adding 1, and writing it back, because the SQL approach works correctly even if multiple programs access the database at the same time.

The UPDATE query is executed with `mysql_query()` and the result is checked. On success it prints "Attendance recorded." and on failure it prints the MySQL error message.

---

### functions/showall.c

This file contains the `showall()` function which displays a formatted table of all students and their attendance percentages.

The function starts by declaring a `char *sql` pointing to a static SQL string: `SELECT id, name, total, present FROM students;`. Unlike the other functions that use `sprintf()` to build queries dynamically, this one has a fixed query with no variable parts, so a string pointer is sufficient.

After running the query with `mysql_query()` and storing the result with `mysql_store_result()`, the function prints the table header using a formatted `printf()` call. The format string `"%-10s %-20s %8s %8s %8s"` uses width specifiers to align columns:
- `%-10s` means left-align the string in a minimum 10-character wide field
- `%-20s` means left-align in a 20-character wide field
- `%8s` means right-align (no minus sign) in an 8-character wide field

This makes all columns line up neatly. A line of dashes `--------------------------------------------------------` is printed underneath as a visual separator.

Then a `while` loop iterates through all result rows using `mysql_fetch_row(res)`. Each call to `mysql_fetch_row()` advances to the next row and returns it as a `MYSQL_ROW`. When there are no more rows it returns NULL and the loop ends.

Inside the loop, `atoi()` is called on each column to convert the string values to integers. This is necessary because MySQL returns all column values as strings regardless of their actual data type. For example, if the `id` column value is 101, `row[0]` is the string `"101"`, and `atoi("101")` returns the integer 101.

The attendance percentage is calculated as `(float)present / total * 100`. The `(float)` type cast is critical here because in C, dividing two integers performs integer division which truncates the decimal part. So `3 / 5` would give 0 instead of 0.6. By casting `present` to float first, C promotes the entire expression to floating-point division. A check `if (total > 0)` prevents division by zero for students who have been added but never had attendance marked.

The final `printf()` for each row uses the same width specifiers as the header: `%-10d` for the ID (left-aligned integer in 10 chars), `%-20s` for the name, `%8d` for total and present, and `%7.1f%%` for the percentage (7 chars wide, 1 decimal place, the `%%` prints a literal percent sign).

After the loop, `mysql_free_result(res)` releases the memory that was allocated by `mysql_store_result()`.

---

### functions/markall.c

This file contains the `markall()` function which goes through every student in the database and marks attendance for each one in sequence.

Unlike `markattendance.c` which searches for one student, this function runs `SELECT id, name FROM students;` with no WHERE clause, so it returns all students. The result is stored with `mysql_store_result()` and the first row is fetched with `mysql_fetch_row()`.

The `if (row == NULL)` check handles the case where no students have been added yet. If the first fetch returns NULL, it means the table is empty, so the result memory is freed, a message is printed, and the function returns early.

If students exist, the function enters a `do...while` loop. A `do...while` loop is used instead of a regular `while` loop because we already fetched the first row before the loop. The `do` block processes the current row (the one we already fetched), then the `while` condition fetches the next row. This pattern ensures every row is processed including the first one.

Inside the loop, `atoi(row[0])` gets the student ID and `row[1]` gets the name. The variable `s` stores the present/absent input. A `char q[200]` buffer holds the dynamically built UPDATE query string. The UPDATE logic is the same as in `markattendance.c` — present increments both total and present, absent increments only total.

Note that this function does not check the return value of `mysql_query(conn, q)` for each UPDATE. This is an intentional simplification — if one update fails, the loop continues with the next student. This is a design choice that keeps the code shorter and is acceptable for a small project where errors are unlikely.

After the loop ends (when `mysql_fetch_row()` returns NULL meaning no more rows), the result memory is freed and "Done." is printed.

---

### functions/searchstudent.c

This file contains the `searchstudent()` function which looks up a single student by their ID and displays their full attendance report.

The function asks for a student ID, then builds a SELECT query with a WHERE clause: `SELECT name, total, present FROM students WHERE id = %d`. This returns only the matching student's data — three columns instead of four because the ID is already known.

After executing the query and fetching the first row, the `if (row != NULL)` block handles a found student. The column values are extracted from the row array: `row[0]` is the name (string), `row[1]` is the total (string that gets converted to int with `atoi()`), `row[2]` is the present count (same conversion).

The percentage calculation uses the same `(float)present / total * 100` formula with the `(float)` cast for proper floating-point division. The `if (total > 0)` check prevents division by zero.

The function prints the student's information using four separate `printf()` calls. This is a deliberate choice — instead of using a formatted table row like `showall.c` does, the search result is displayed as a vertical list which is easier to read for a single student's details.

If the student is not found (`row == NULL`), it prints "Student not found." Either way, `mysql_free_result(res)` is called at the end to release the result set memory.

---

### functions/deletestudent.c

This file contains the `deletestudent()` function which removes a student record from the database.

This is the simplest function in the project. It asks for a student ID, builds a DELETE query using `sprintf()`: `DELETE FROM students WHERE id = %d`, and executes it with `mysql_query()`. The WHERE clause ensures only the student with the matching ID is deleted. Without a WHERE clause, `DELETE FROM students` would remove all rows from the table.

Unlike the array-based version of this project where deleting required shifting all subsequent elements to fill the gap, the database version simply sends the DELETE query and MySQL handles removing the row and reorganizing the storage internally. No manual shifting or reindexing is needed from the C code.

The return value of `mysql_query()` is checked — 0 means success and the student was deleted, non-zero means an error occurred (for example if the student ID does not exist, though MySQL's DELETE with a non-matching WHERE clause actually returns success with 0 affected rows).

---

### main.c

This file contains only the `main()` function which is the program entry point. It includes `include/functions.h` to access all function declarations and the shared `conn` variable.

The function starts by declaring `int choice` to store the user's menu selection, then immediately calls `connectdb()` to establish the MySQL connection and ensure the database and table exist before doing anything else.

The `while (1)` loop creates an infinite loop that displays the menu on every iteration. The `1` condition is always true, so the only way to exit is through the `break` statement. Each iteration prints the menu options numbered 1 through 6 plus 0 for exit, reads the user's choice with `scanf("%d", &choice)`, and uses an `if/else if/else` chain to call the matching function.

When the user enters 0, the `else if (choice == 0)` block executes `closedb()` to properly close the MySQL connection, prints "Data saved. Goodbye.", and calls `break` which exits the while loop. After the loop, `return 0` terminates the program with a success exit code.

The `else` at the bottom catches any invalid input (numbers other than 0-6) and prints "Invalid choice. Try again." The loop then continues and the menu is shown again.

Note that `main.c` contains no MySQL-specific code at all. It does not include `<mysql/mysql.h>` directly and does not call any MySQL functions. It relies entirely on the functions declared in the header file, which is the whole point of the multi-file structure — the main file only handles user interaction and delegates all database work to the other files.

---

## Database Schema

The program uses a MySQL database called `attendance_db` with a single table called `students`:

```sql
CREATE TABLE students (
    id      INT PRIMARY KEY,
    name    VARCHAR(50),
    total   INT DEFAULT 0,
    present INT DEFAULT 0
);
```

| Column   | Type       | Description                                      |
|----------|------------|--------------------------------------------------|
| id       | INT        | Unique student ID, primary key, no duplicates     |
| name     | VARCHAR(50)| Student name, maximum 50 characters              |
| total    | INT        | Total classes attendance has been marked for      |
| present  | INT        | Number of times student was marked present       |

The attendance percentage is not stored in the database — it is calculated on the fly using the formula: `(present / total) * 100`. This avoids data redundancy and ensures the percentage is always accurate.

---

## Concepts Used

### 1. MySQL C API
The MySQL C client library provides functions for communicating with a MySQL server from a C program. `mysql_init()` allocates and initializes a connection object. `mysql_real_connect()` opens a connection to the server with specified credentials. `mysql_query()` executes any SQL statement as a string. For SELECT queries that return data, `mysql_store_result()` retrieves the complete result set, `mysql_fetch_row()` returns one row at a time as an array of strings, and `mysql_free_result()` releases the allocated memory. `mysql_error()` returns a descriptive error string for the most recent failed function call. `mysql_close()` terminates the connection and frees resources.

### 2. SQL (Structured Query Language)
The program uses six types of SQL statements. `CREATE DATABASE IF NOT EXISTS` creates the database on first run without erroring on subsequent runs. `CREATE TABLE IF NOT EXISTS` creates the table structure with column definitions. `INSERT INTO ... VALUES(...)` adds a new row with specified column values. `SELECT ... FROM ... WHERE` retrieves rows that match a condition. `UPDATE ... SET ... WHERE` modifies values in existing rows. `DELETE FROM ... WHERE` removes rows that match a condition. The `IF NOT EXISTS` clauses prevent errors on repeated runs, and the `WHERE` clauses ensure operations only affect specific rows.

### 3. Header Files and Include Guards
A header file (`.h`) declares function prototypes and shared variables that are defined in `.c` files. This allows different source files to call each other's functions without knowing the implementation details. The `#ifndef MACRO / #define MACRO / #endif` pattern is called an include guard — it prevents the header from being processed more than once per compilation unit, which would cause duplicate declaration errors. The `extern` keyword in a declaration tells the compiler that the variable is defined in another source file and should not be allocated again.

### 4. Multi-File Compilation and Linking
C projects are typically split into multiple `.c` files for organization. Each file is compiled independently into an object file (`.o`) containing machine code but with unresolved function calls. The linker then combines all object files together, resolving cross-file function calls and library references into a single executable. The header file enables this by telling each compilation unit what functions and variables exist in other files. The build command compiles all `.c` files together in one step with gcc.

### 5. Modular Design
Each function lives in its own file in the `functions/` directory. This is called modularity — each file has a single responsibility. If there is a bug in the mark attendance feature, you only need to open `functions/markattendance.c`. If you want to change how the table looks, you open `functions/showall.c`. Changes to one file do not affect others as long as the function signatures in the header remain the same.

### 6. Type Casting
C performs integer division when both operands are integers, which truncates the decimal part. The expression `present / total` where both are `int` would give 0 for cases like 3/5. By casting one operand to float with `(float)present / total`, the compiler promotes the other operand to float as well and performs proper floating-point division, giving 0.6. This is essential for accurate percentage calculation.

### 7. atoi() String to Integer Conversion
The `mysql_fetch_row()` function returns all column values as `char *` (strings), even for columns defined as INT in the database. The `atoi()` function (ASCII to integer) converts these string representations to actual `int` values that can be used in arithmetic operations and formatted with `%d` in printf. For example, `atoi("42")` returns the integer 42.

### 8. sprintf()
`sprintf()` writes formatted output to a character array instead of to the console (unlike printf). It uses the same format specifiers (%d for int, %s for string, %f for float) but stores the result in a buffer. This allows SQL queries to be constructed at runtime with variable values embedded in them, such as building `WHERE id = 101` from a user-input variable.

### 9. scanf Format Specifiers
`scanf("%d", &variable)` reads an integer from standard input. `scanf(" %[^\n]", variable)` reads a string that may contain spaces by consuming all characters up to the next newline. The space before `%[^\n]` is essential — it tells scanf to discard any whitespace characters (including leftover newlines from previous input) before reading the actual string. Without this space, a previous scanf for an integer would leave its trailing newline in the input buffer, causing the string scanf to read an empty string.

### 10. printf Formatting
`printf` format specifiers can include width and alignment options: `%-10d` left-aligns an integer in a 10-character field (padding with spaces on the right), `%8s` right-aligns a string in an 8-character field, `%7.1f` displays a float in a 7-character field with 1 decimal place, and `%%` prints a literal percent sign. These create aligned table columns in terminal output.

---

## Possible Questions and Answers

**Q: Why did you separate each function into its own file?**
A: Because it makes the project easier to manage and debug. Each file does exactly one thing — `addstudent.c` only adds students, `showall.c` only displays the table, and so on. If there is a bug in the mark attendance feature, I open that one file and fix it without touching anything else. This is called modularity and it becomes more important as projects get bigger.

**Q: What is the purpose of the header file?**
A: The header file `functions.h` declares all the functions and the shared `conn` variable so that different `.c` files can call each other's functions. Without it, `main.c` would not know that `connectdb()` or `addstudent()` exist, and the compiler would give an error. The header acts as a contract between files — it says "these functions exist somewhere" without showing how they work.

**Q: What is an include guard?**
A: The `#ifndef FUNCTIONS_H / #define FUNCTIONS_H / #endif` pattern at the top and bottom of the header file. When the compiler encounters this file for the first time, `FUNCTIONS_H` is not defined so everything inside is processed and `FUNCTIONS_H` gets defined. If the file is included again (which happens with multiple `.c` files), `FUNCTIONS_H` is already defined so the entire content is skipped. This prevents duplicate declarations which would cause compilation errors.

**Q: What does `extern` mean?**
A: `extern MYSQL *conn` tells the compiler that a variable named `conn` of type `MYSQL *` exists in another `.c` file. It is a declaration, not a definition — it says "this exists somewhere" without actually creating it. The actual definition (the line that allocates memory) is in `database/db.c` where it says `MYSQL *conn;` without the `extern` keyword. Without `extern` in the header, every file that includes the header would try to create its own `conn` variable, and the linker would fail with a "multiple definition" error.

**Q: Why do you connect to MySQL twice in connectdb()?**
A: Because I need to create the database before I can connect to it. The first connection does not specify a database name (the fourth parameter is NULL), so I use that connection only to run `CREATE DATABASE IF NOT EXISTS attendance_db`. Then I close it and reconnect, this time specifying `attendance_db` as the database. The MySQL C API does not provide a way to create a database while already connected to one, so this two-step process is necessary.

**Q: What is the difference between mysql_query() for SELECT vs INSERT/UPDATE/DELETE?**
A: For INSERT, UPDATE, and DELETE, `mysql_query()` is all you need — it executes the query and returns 0 on success. For SELECT queries, `mysql_query()` only sends the query to the server. You then need to call `mysql_store_result()` to retrieve the result set, `mysql_fetch_row()` to read rows one at a time, and `mysql_free_result()` to release the memory when done. This is because SELECT returns data that needs to be processed, while the others just modify data and return a status.

**Q: Why do you call mysql_free_result()?**
A: `mysql_store_result()` allocates memory on the heap to store the entire result set. This memory is not automatically freed — if you do not call `mysql_free_result()` after you are done reading the results, that memory stays allocated. In a long-running loop or repeated function calls, this would cause a memory leak where more and more memory gets consumed without being released, eventually slowing down or crashing the program.

**Q: Why does MySQL return integer columns as strings?**
A: Because `mysql_fetch_row()` returns a `MYSQL_ROW` type which is defined as an array of `char *` pointers. All column values regardless of their actual SQL data type are returned as null-terminated strings. This simplifies the API implementation. To use them as numbers, you convert them with functions like `atoi()` for integers or `atof()` for floats.

**Q: Why do you cast to float in the percentage calculation?**
A: In C, dividing two integers performs integer division which discards the remainder. For example, `3 / 5` gives `0`, not `0.6`. By writing `(float)present / total`, the cast converts `present` to a float, which causes the compiler to automatically promote `total` to float as well, resulting in proper floating-point division. The result is then multiplied by 100 to get the percentage value.

**Q: What does `scanf(" %[^\n]", name)` do?**
A: The `%[^\n]` format specifier tells scanf to read and store every character until it encounters a newline (the Enter key). The `^` means "not" and `\n` is the newline character, so it reads everything that is not a newline. The space before it is equally important — it tells scanf to first skip any leftover whitespace characters in the input buffer, including the newline left behind by a previous `scanf("%d")` call. Without the space, the string scanf would immediately read an empty string from the leftover newline and skip the actual input.

**Q: What is a primary key?**
A: A primary key is a column (or set of columns) that uniquely identifies each row in a database table. In the students table, `id` is the primary key, which means no two students can have the same ID value. MySQL enforces this constraint — if you try to INSERT a student with an ID that already exists, the query fails with a "Duplicate entry" error. The primary key is also used by MySQL internally to efficiently look up and organize rows.

**Q: How does the delete function work compared to an array?**
A: In an array-based implementation, deleting an element requires shifting every subsequent element one position to the left to fill the gap, which is an O(n) operation. With MySQL, deleting a row is simply `DELETE FROM students WHERE id = X` and the database engine handles removing the row and reorganizing storage internally. The C code does not need to do any shifting or reindexing — it just sends the query and MySQL takes care of the rest.

**Q: What happens if MySQL server is not running?**
A: The `mysql_real_connect()` call returns NULL, and the program checks for this: `if (mysql_real_connect(...) == NULL)`. When NULL is returned, `mysql_error(conn)` provides a descriptive error message like "Can't connect to local MySQL server through socket" or "Connection refused". The program prints this message and exits with code 1. The user needs to start the MySQL service before running the program.

**Q: How does the data survive after closing the program?**
A: MySQL server manages data on disk. Every INSERT, UPDATE, and DELETE query is immediately written to the database files on disk by the MySQL server. When the program exits and calls `mysql_close()`, it only closes the network connection between the program and the server. The data is already stored on disk and persists independently of the program. Next time the program runs, it reconnects to the same database and all data is still there.

**Q: What is the maximum number of students this system can handle?**
A: MySQL can handle millions of rows in a table without any issue, so there is no practical limit imposed by this code. The only limits would be the MySQL server's available disk space and memory. Unlike the array-based version which had a fixed size of 100, the database version grows dynamically as records are added.

**Q: Why is the percentage calculated at display time instead of stored in the database?**
A: Because storing a calculated value that can be derived from existing data is redundant. If I stored the percentage and then marked a student absent, I would need to recalculate and update both the `total` count and the `percentage` value in the same UPDATE query. By calculating it on the fly from `present / total`, the percentage is always accurate and I never have to worry about the stored percentage getting out of sync with the actual counts.

**Q: What is the difference between mysql_store_result() and mysql_use_result()?**
A: `mysql_store_result()` retrieves all rows from the server immediately and stores them in client memory. `mysql_use_result()` does not retrieve any rows upfront — each call to `mysql_fetch_row()` fetches one row from the server on demand. `mysql_store_result()` is simpler to use and allows you to find out how many rows there are with `mysql_num_rows()`, but uses more memory for large result sets. `mysql_use_result()` uses less memory but locks the table and does not allow other queries until all rows are fetched. For this project with small data, `mysql_store_result()` is the appropriate choice.
