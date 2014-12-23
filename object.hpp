#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include "type.hpp"
#include "table.hpp"
struct Object {
    void* loc;
    int size;
    TYPE type;
    bool is_null;
};
struct LiteralManager {
    Object GetInt(int l);
    Object GetVarChar(const std::string& l);
    void clear();
};
struct Expr {
    virtual Object getObj(void* l, void* r = nullptr) = 0;
    virtual void Use(const std::string& lname, const std::string& rname, TableDesc* ldesc, TableDesc* rdesc = nullptr) = 0;
};
struct ReadExpr : public Expr {
    bool useLeft;
    int offset;
    int size;
    std::string tbl, name;
    ReadExpr(const std::string& _name) : tbl(""), name(_name) {}
    ReadExpr(const std::string& _tbl, const std::string& _name) : tbl(_tbl), name(_name) {}
    virtual Object getObj(void* l, void* r = nullptr);
    virtual void Use(const std::string& lname, const std::string& rname, TableDesc* ldesc, TableDesc* rdesc = nullptr);
};
struct LiteralExpr : public Expr {
    Object obj;
    LiteralExpr(Object _obj) : obj(_obj) {}
    virtual Object getObj(void* l, void* r = nullptr);
    virtual void Use(const std::string& lname, const std::string& rname, TableDesc* ldesc, TableDesc* rdesc = nullptr);
};
typedef bool (*Oper)(const Object&, const Object&);
struct Condition {
    Expr *l, *r;
    Oper op;
};
inline bool op_eq(const Object&, const Object&);
inline bool op_ne(const Object&, const Object&);
inline bool op_lt(const Object&, const Object&);
inline bool op_gt(const Object&, const Object&);
inline bool op_le(const Object&, const Object&);
inline bool op_ge(const Object&, const Object&);
inline bool op_asn(const Object&, const Object&);
#endif
