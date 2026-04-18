#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

void searchstudent() {
    int sid;
    char sql[200];
    MYSQL_RES *res;
    MYSQL_ROW row;
    printf("Enter student ID: ");
    scanf("%d", &sid);
    sprintf(sql, "SELECT name, total, present FROM students WHERE id = %d;", sid);
    if (mysql_query(conn, sql) != 0) {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (row != NULL) {
        char *name = row[0];
        int total = atoi(row[1]);
        int present = atoi(row[2]);
        float pct = 0;
        if (total > 0)
            pct = (float)present / total * 100;
        printf("Student: %s\n", name);
        printf("Total classes: %d\n", total);
        printf("Present: %d\n", present);
        printf("Attendance: %.1f%%\n", pct);
    } else {
        printf("Student not found.\n");
    }
    mysql_free_result(res);
}
