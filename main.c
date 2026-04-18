#include <stdio.h>
#include "include/functions.h"

int main() {
    int choice;
    connectdb();
    while (1) {
        printf("--- Student Attendance System ---\n");
        printf("1. Add Student\n");
        printf("2. Mark Attendance (Single)\n");
        printf("3. Mark Attendance (All)\n");
        printf("4. View All Students\n");
        printf("5. Search Student\n");
        printf("6. Delete Student\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        if (choice == 1)
            addstudent();
        else if (choice == 2)
            markattendance();
        else if (choice == 3)
            markall();
        else if (choice == 4)
            showall();
        else if (choice == 5)
            searchstudent();
        else if (choice == 6)
            deletestudent();
        else if (choice == 0) {
            closedb();
            printf("Data saved. Goodbye.\n");
            break;
        }
        else
            printf("Invalid choice. Try again.\n");
    }
    return 0;
}
