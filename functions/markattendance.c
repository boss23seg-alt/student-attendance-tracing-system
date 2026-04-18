#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

void markattendance() {
    int sid, status;
    char query[200];
    MYSQL_RES *res;
    MYSQL_ROW row;
    printf("Enter student ID: ");
    scanf("%d", &sid);
    sprintf(query, "SELECT name FROM students WHERE id = %d;", sid);
    if (mysql_query(conn, query) != 0) {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (row != NULL) {
        printf("%s found.\n", row[0]);
        mysql_free_result(res);
        printf("1 = Present, 0 = Absent: ");
        scanf("%d", &status);
        if (status == 1)
            sprintf(query, "UPDATE students SET total = total + 1, present = present + 1 WHERE id = %d;", sid);
        else
            sprintf(query, "UPDATE students SET total = total + 1 WHERE id = %d;", sid);
        if (mysql_query(conn, query) == 0)
            printf("Attendance recorded.\n");
        else
            printf("Error: %s\n", mysql_error(conn));
    } else {
        mysql_free_result(res);
        printf("Student not found.\n");
    }
}
