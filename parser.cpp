#include <iostream>
#include <cstdlib>
#include <unordered_map>

#include "parser.hpp"


Stmt* Parser::parse(const std::string& sql) {
    std::cout << "ok";
    
    TokenList l  = tokenize(sql);
    return parseSQL(l.begin(), l.end());
}
static const std::unordered_map<std::string, Token::Type> specialToken = {
    {"update", Token::UPDATE},
    {"set", Token::SET},
    {"select", Token::SELECT},
    {"delete", Token::DELETE},    
    {"where", Token::WHERE},
    {"from", Token::FROM},
    {"and", Token::AND},
};
static const std::unordered_set<char> IDChar = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '_',
};
static const std::unordered_set<std::string> Op = {
    ">", "<", "=", "!=", ">=", "<=", ",", "*"
};
static std::string toLower(const std::string& str) {
    std::string ret;
    ret.resize(str.size());
    for (int i = 0; i < str.size(); i++) {
        char ch = str[i];
        if (ch >= 'A' && ch <= 'Z')
            ch ^= 0x20;
        ret[i] = ch;
    }
    return std::move(ret);
}
Parser::TokenList Parser::tokenize(const std::string& sql) {
    std::cout << "ok";
    
    TokenList ret;
    auto iter = sql.begin();
    while (true) {
        for (; iter != sql.end() && isspace(*iter); iter++);
        if (iter == sql.end())
            break;
        Token t;
        if (*iter == '"' || *iter == '\'') {
            char flag = *iter;
            auto beg = ++iter;
            for (; iter != sql.end() && *iter != flag; iter++);
            if (iter == sql.end())
                throw "String Parse Error";
            t.raw = std::string(beg, iter++);
            t.token = Token::VARCHAR_LIT;
        } else if (isdigit(*iter)) {
            auto beg = iter;
            for (; iter != sql.end() && isdigit(*iter); iter++);
            t.raw = std::string(beg, iter);
            t.token = Token::INT_LIT;
        } else if (IDChar.find(*iter) != IDChar.end()) {
            auto beg = iter;
            for (; iter != sql.end() && IDChar.find(*iter) != IDChar.end(); iter++);
            t.raw = toLower(std::string(beg, iter));
            auto i = specialToken.find(t.raw);
            if (i != specialToken.end()) {
                t.token = i->second;
            } else {
                t.token = Token::ID;
            }
        } else if (iter + 1 != sql.end() && Op.find(std::string(iter, iter + 2)) != Op.end()) {
            t.raw = std::string(iter, iter + 2);
            t.token = Token::OPER;
            iter += 2;
        } else if (Op.find(std::string(iter, iter + 1)) != Op.end()) {
            t.raw = std::string(iter, iter + 1);
            t.token = Token::OPER;
            iter += 1;
        } else {
            throw "Tokenize Error";
        }
        ret.push_back(t);
    }
    return ret;
}
Stmt* Parser::parseSQL(Parser::TokenIter beg, Parser::TokenIter end) {
    std::cout << "ok";
    
    switch (beg->token) {
        case Token::SELECT:
            return parseSelect(beg + 1, end);
        case Token::DELETE:
            return parseDelete(beg + 1, end);
        case Token::UPDATE:
            return parseUpdate(beg + 1, end);
        case Token::INSERT:
        //    return parseInsert(beg + 1, end);
            
        default:
            throw "Syntax Error";
    }
}

UpdateStmt* Parser::parseUpdate(Parser::TokenIter beg, Parser::TokenIter end) {
    std::cout << "ok";
    auto setLoc = findToken(beg, end, Token::SET);
    auto whereLoc = findToken(setLoc, end, Token::WHERE);
    auto tablename = parseTableName(beg, setLoc);
    std::cout << tablename;
    auto set = parseSet(setLoc + 1, whereLoc);
    auto where = parseWhere(whereLoc + 1, end);
    UpdateStmt* ret = new UpdateStmt;
    ret->tbl = tablename;
    ret->lv = *set.first;
    ret->obj = set.second;
    ret->conds = std::move(where);
    return ret;
}


DeleteStmt* Parser::parseDelete(Parser::TokenIter beg, Parser::TokenIter end) {
    auto fromLoc = findToken(beg, end, Token::FROM);
    auto whereLoc = findToken(fromLoc, end, Token::WHERE);
    auto from = parseFrom(fromLoc + 1, whereLoc);
    auto where = parseWhere(whereLoc + 1, end);
    DeleteStmt* ret = new DeleteStmt;
    ret->tbl = from.first;
    //ret->tbl2 = from.second;
    ret->conds = std::move(where);
    return ret;
}

SelectStmt* Parser::parseSelect(Parser::TokenIter beg, Parser::TokenIter end) {
    auto fromLoc = findToken(beg, end, Token::FROM);
    auto whereLoc = findToken(fromLoc, end, Token::WHERE);
    auto from = parseFrom(fromLoc + 1, whereLoc);
    auto where = parseWhere(whereLoc + 1, end);
    SelectStmt* ret = new SelectStmt;
    ret->tbl1 = from.first;
    ret->tbl2 = from.second;
    ret->conds = std::move(where);
    return ret;
}
InsertStmt* Parser::parseInsert(Parser::TokenIter beg, Parser::TokenIter end) {
    
}
std::pair<std::string, std::string> Parser::parseFrom(Parser::TokenIter beg, Parser::TokenIter end) {
    auto COMMA = findToken(beg, end, Token::OPER, ",");
    if (COMMA == end)
        return std::make_pair(beg->raw, "");
    else
        return std::make_pair(beg->raw, (COMMA + 1)->raw);
}

std::string Parser::parseTableName(Parser::TokenIter beg, Parser::TokenIter end) {
    return beg->raw;
}

std::vector<Condition> Parser::parseWhere(Parser::TokenIter beg, Parser::TokenIter end) {
    std::vector<Condition> ret;
    if (beg != end) {
        while (true) {
            auto COMMA = findToken(beg, end, Token::AND);
            ret.push_back(parseCond(beg, COMMA));
            if (COMMA == end)
                break;
            beg = COMMA + 1;
        }
    }
    return std::move(ret);
}
Condition Parser::parseCond(Parser::TokenIter beg, Parser::TokenIter end) {
    Condition ret;
    auto iter = beg;
    TokenIter OPER;
    while (true) {
        OPER = findToken(iter, end, Token::OPER);
        if (OPER == end)
            throw "Syntax Error";
        if (OPER->raw == "=") {
            ret.op = op_eq;
        } else if (OPER->raw == "!=") {
            ret.op = op_ne;
        } else if (OPER->raw == "<") {
            ret.op = op_lt;
        } else if (OPER->raw == ">") {
            ret.op = op_gt;
        } else if (OPER->raw == "<=") {
            ret.op = op_le;
        } else if (OPER->raw == ">=") {
            ret.op = op_ge;
        } else {
            iter = OPER + 1;
            continue;
        }
        break;
    }
    ret.l = parseExpr(beg, OPER);
    ret.r = parseExpr(OPER + 1, end);
    return ret;
}

std::pair<ReadExpr*, Object> Parser::parseSet(Parser::TokenIter beg, Parser::TokenIter end) {
    auto iter = beg;
    TokenIter OPER;
   // while (true) {
        OPER = findToken(iter, end, Token::OPER);
        if (OPER == end)
            throw "Syntax Error";
        if (OPER->raw == "=") {
           
        } else {
            throw "Syntax Error";
        }
   // }
    ReadExpr* readexpr = (ReadExpr*)parseExpr(beg, OPER);
    Object obj = ((LiteralExpr*)parseExpr(OPER + 1, end))->obj;
    return std::pair<ReadExpr*, Object>(readexpr, obj);
}

Expr* Parser::parseExpr(Parser::TokenIter beg, Parser::TokenIter end) {
    if (end == beg + 1) {
        if (beg->token == Token::ID)
            return new ReadExpr(beg->raw);
        if (beg->token == Token::VARCHAR_LIT)
            return new LiteralExpr(litManager.GetVarChar(beg->raw));
        if (beg->token == Token::INT_LIT)
            return new LiteralExpr(litManager.GetInt(std::stoi(beg->raw)));
    } else if (end == beg + 3) {
        return new ReadExpr(beg->raw, (beg + 2)->raw);
    } else {
        throw "Syntax Error";
    }
}
Parser::TokenIter Parser::findToken(Parser::TokenIter beg, Parser::TokenIter end, Token::Type token, const std::string& raw) {
    for (auto iter = beg; iter != end; iter++)
        if (iter->token == token && (raw == "" || iter->raw == raw))
            return iter;
    return end;
}
