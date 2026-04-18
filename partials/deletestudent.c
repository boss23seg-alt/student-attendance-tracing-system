#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

void deletestudent() {
    int sid;
    char sql[200];
    printf("Enter student ID to delete: ");
    scanf("%d", &sid);
    sprintf(sql, "DELETE FROM students WHERE id = %d;", sid);
    if (mysql_query(conn, sql) == 0)
        printf("Student deleted.\n");
    else
        printf("Error: %s\n", mysql_error(conn));
}
