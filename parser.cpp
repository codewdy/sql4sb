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

Stmt* parse(char* sql){
    char* type = getNextStr(sql);
    if ( std::strcmp(type, "select") == 0 )
        parseSelect(sql);
}

Stmt* parseCreateDB(char* str){
}


int main() {
    char* hello = "  hello  = test   ";
    while ( !isEmpty(hello) ) {
        std::cout << getNextStr(hello) << std::endl;
    }
    return 0;
}

