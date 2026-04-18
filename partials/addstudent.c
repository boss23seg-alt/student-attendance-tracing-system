#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

void addstudent() {
    int sid;
    char name[50];
    char sql[256];
    printf("Enter student ID: ");
    scanf("%d", &sid);
    printf("Enter student name: ");
    scanf(" %[^\n]", name);
    sprintf(sql, "INSERT INTO students(id, name, total, present) VALUES(%d, '%s', 0, 0);", sid, name);
    if (mysql_query(conn, sql) == 0)
        printf("Student added.\n");
    else
        printf("Error: %s\n", mysql_error(conn));
}
