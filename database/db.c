#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../include/functions.h"

MYSQL *conn;
char host[] = "localhost";
char user[] = "root";
char pass[] = "";
char dbname[] = "attendance_db";

void connectdb() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        printf("MySQL init failed.\n");
        exit(1);
    }
    if (mysql_real_connect(conn, host, user, pass, NULL, 0, NULL, 0) == NULL) {
        printf("Connection error: %s\n", mysql_error(conn));
        exit(1);
    }
    mysql_query(conn, "CREATE DATABASE IF NOT EXISTS attendance_db;");
    mysql_close(conn);
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0) == NULL) {
        printf("Connection error: %s\n", mysql_error(conn));
        exit(1);
    }
    mysql_query(conn,
        "CREATE TABLE IF NOT EXISTS students("
        "id INT PRIMARY KEY,"
        "name VARCHAR(50),"
        "total INT DEFAULT 0,"
        "present INT DEFAULT 0)"
    );
}

void closedb() {
    mysql_close(conn);
}
