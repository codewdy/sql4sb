#ifndef PARSER_H
#define PARSER_H
#include "stmt.hpp"
struct Parser {
    LiteralManager litManager;
    void clear();
    Stmt* parse(char* sql);
    Stmt* parseCreateDB(char* str);
    Stmt* parseDropDB(char* str);
    Stmt* parseUse(char* str);
    Stmt* parseCreateTable(char* str);
    Stmt* parseDropTable(char* str);
    Stmt* parseInsert(char* str);
    Stmt* parseDelete(char* str);
    Stmt* parseUpdate(char* str);
    Stmt* parseSelect(char* str);

    Condition parseCondition(char* str);
    Type parseType(char* str);
    ReadExpr parseReadExpr(char* str);
    Object parseObject(char* str);
    std::string parseTableName(char* str);
};
#endif
