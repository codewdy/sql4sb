#include <iostream>
#include <cstdlib>

#include "parser.hpp"

int trim(char*& sql) {
    char* start = sql;
    for (; *sql==' '; sql++);
    return sql-start;
}

bool isEmpty(char*& str) {
    int size = std::strlen(str);
    if (trim(str) == size) 
        return true;
    return false;
}

char* getNextStr(char*& str) {
    if ( isEmpty(str) )
        return "";
    
    char* start = str;
    for (; *str!=' '; str++);
    int length = str-start;
    char* res = new char[20];
    memcpy(res, start, length);
    return res;
}

Stmt* Parser::parse(char* sql){
    char* type = getNextStr(sql);
    if ( std::strcmp(type, "select") == 0 ) {
        return parseSelect(sql);
    } else if (std::strcmp(type, "create") == 0) {
        char* table = getNextStr(sql);
        if (std::strcmp(table, "table") == 0) {
            return parseCreateTable(sql);
        } else {
            return parseCreateDB(sql);
        }
    }
}

Stmt* Parser::parseCreateTable(char* sql) {
    char* table = getNextStr(sql);
    CreateTableStmt stmt;
    std::string tbl(table);
    stmt.tbl = tbl;
    return &stmt;
}

Stmt* Parser::parseCreateDB(char* str){
}

Stmt* Parser::parseSelect(char* str){
}



int main() {
    char* hello = "create table test";
    while ( !isEmpty(hello) ) {
        std::cout << getNextStr(hello) << std::endl;
    }
    return 0;
}

