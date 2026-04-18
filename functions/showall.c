#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

void showall() {
    char *sql = "SELECT id, name, total, present FROM students;";
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (mysql_query(conn, sql) != 0) {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }
    res = mysql_store_result(conn);
    printf("%-10s %-20s %8s %8s %8s\n", "ID", "Name", "Total", "Present", "Percent");
    printf("--------------------------------------------------------\n");
    while ((row = mysql_fetch_row(res)) != NULL) {
        int id = atoi(row[0]);
        char *name = row[1];
        int total = atoi(row[2]);
        int present = atoi(row[3]);
        float pct = 0;
        if (total > 0)
            pct = (float)present / total * 100;
        printf("%-10d %-20s %8d %8d %7.1f%%\n", id, name, total, present, pct);
    }
    mysql_free_result(res);
}
