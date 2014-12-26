#include "manager.hpp"
#include <vector>
#include <iostream>
#include <cstdio>

int main() {
    std::remove("test");
    Manager manager;
    std::vector<Type> types;
    Type type(TYPE_INT, false, 4, "id");
    types.push_back(type);
    manager.CreateTable("test", types); 

    std::vector<Object> objects;
    std::vector<std::vector<Object>> obj_vec;
    LiteralManager lmanager;
    Object obj4 = lmanager.GetInt(4);
    objects.push_back(obj4);
    obj_vec.push_back(objects);
    std::string str = "test";
    manager.Insert(str, obj_vec);

    TableDesc desc;
    desc.colSize = 1;
    desc.colType[0] = type;
    ReadExpr lexpr("test", "id");
    Table* table = manager.getTable("test", false);
    lexpr.Use("test", "", &desc, NULL); 
    LiteralExpr rexpr(obj4);
    Condition cond;
    cond.l = &lexpr;
    cond.r = &rexpr;
    cond.op = op_eq;
    std::vector<Condition> conds;
    conds.push_back(cond);
    //manager.Update(str, conds, lexpr, obj);  

    manager.Select("test", "", conds);

    //manager.Delete("test", conds);
}
