#ifndef PARSER_H
#define PARSER_H
#include "stmt.hpp"
struct Token {
    enum Type {
        SET,
        DELETE,
        UPDATE,
        INSERT,
        VARCHAR_LIT,
        INT_LIT,
        OPER,
        ID,
        SELECT,
        WHERE,
        FROM,
        AND,
        INTO,
        VALUES,
    } token;
    std::string raw;
};
struct Parser {
    LiteralManager litManager;
    typedef std::vector<Token> TokenList;
    typedef TokenList::iterator TokenIter;
    void clear();
    Stmt* parse(const std::string& sql);
    TokenList tokenize(const std::string& sql);
    Stmt* parseSQL(TokenIter beg, TokenIter end);
    SelectStmt* parseSelect(TokenIter beg, TokenIter end);
    DeleteStmt* parseDelete(TokenIter beg, TokenIter end);
    UpdateStmt* parseUpdate(TokenIter beg, TokenIter end);
    InsertStmt* parseInsert(TokenIter beg, TokenIter end);
    ShowTblStmt* parseInsert(TokenIter beg, TokenIter end);
    DescStmt* parseInsert(TokenIter beg, TokenIter end);
    CreateTableStmt* parseInsert(TokenIter beg, TokenIter end);
    DropTableStmt* parseInsert(TokenIter beg, TokenIter end);
    CreateDBStmt* parseInsert(TokenIter beg, TokenIter end);
    DropDBStmt* parseInsert(TokenIter beg, TokenIter end);
    
    std::pair<std::string, std::string> parseFrom(TokenIter beg, TokenIter end);
    std::vector<Condition> parseWhere(TokenIter beg, TokenIter end);
    std::string parseTableName(TokenIter beg, TokenIter end);
    std::pair<ReadExpr*, Object> parseSet(TokenIter beg, TokenIter end);
    Condition parseCond(TokenIter beg, TokenIter end);
    Expr* parseExpr(TokenIter beg, TokenIter end);
    std::vector<std::vector<Object>> parseRows(TokenIter beg, TokenIter end);
    std::vector<Object> parseRow(TokenIter beg, TokenIter end);
    TokenIter findToken(TokenIter beg, TokenIter end, Token::Type token, const std::string& raw = "");
};
#endif
