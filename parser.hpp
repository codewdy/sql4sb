#ifndef PARSER_H
#define PARSER_H
#include "stmt.hpp"
struct Parser {
    LiteralManager litManager;
    void clear();
    Stmt* parse(const std::string& sql);
};
#endif
