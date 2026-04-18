#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <mysql/mysql.h>

extern MYSQL *conn;

void connectdb();
void addstudent();
void markattendance();
void markall();
void showall();
void searchstudent();
void deletestudent();
void closedb();

#endif
