#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

void markall() {
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (mysql_query(conn, "SELECT id, name FROM students;") != 0) {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        printf("No students added yet.\n");
        return;
    }
    printf("Marking attendance for all students.\n");
    do {
        int id = atoi(row[0]);
        char *name = row[1];
        int s;
        printf("%s (ID: %d) - 1=Present 0=Absent: ", name, id);
        scanf("%d", &s);
        char q[200];
        if (s == 1)
            sprintf(q, "UPDATE students SET total = total + 1, present = present + 1 WHERE id = %d;", id);
        else
            sprintf(q, "UPDATE students SET total = total + 1 WHERE id = %d;", id);
        mysql_query(conn, q);
    } while ((row = mysql_fetch_row(res)) != NULL);
    mysql_free_result(res);
    printf("Done.\n");
}
