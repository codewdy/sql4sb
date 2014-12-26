#include "manager.hpp"
#include <vector>
#include <iostream>
#include <cstdio>

int main() {
    std::remove(":table1.db");
    std::remove(":table2.db");
    Manager manager;

    // two columns
    std::vector<Type> types;
    Type typeint(TYPE_INT, false, 4, "id");
    Type typechar(TYPE_VARCHAR, false, 10, "name");
    types.push_back(typeint);
    types.push_back(typechar);
    manager.CreateTable("table1", types); 


    std::vector<std::vector<Object>> obj_vec;
    
    //record1
    std::vector<Object> objects;
    LiteralManager lmanager;
    Object obj4 = lmanager.GetInt(4);
    Object obj5 = lmanager.GetInt(5);
    objects.push_back(obj4);
    std::string str_ = "hello";
    Object objstr = lmanager.GetVarChar(str_);
    objects.push_back(objstr);
    obj_vec.push_back(objects);

    //record2
    objects.clear();
    Object obj2 = lmanager.GetInt(2);
    objects.push_back(obj2);
    std::string strfine = "fine";
    objstr = lmanager.GetVarChar(strfine);
    objects.push_back(objstr);
    obj_vec.push_back(objects);
    std::string str = "table1";
    // insert two records
    manager.Insert(str, obj_vec);

    //table2
    manager.CreateTable("table2", types); 
    str = "table2";
    manager.Insert(str, obj_vec);

    std::vector<Condition> conds;
    TableDesc desc;
    desc.colSize = 2;
    desc.colType[0] = typeint;
    desc.colType[1] = typechar;

    //conds 1
    ReadExpr lexpr("table1", "id");
    LiteralExpr rexpr = LiteralExpr(obj2);
    Condition cond;
    cond.l = new ReadExpr(lexpr);
    cond.r = new LiteralExpr(rexpr);
    cond.op = op_eq;
    conds.push_back(cond);
    //conds 2
    lexpr = ReadExpr("table1", "name");
    rexpr = LiteralExpr(objstr);
    cond.l = new ReadExpr(lexpr);
    cond.r = new LiteralExpr(rexpr);
    cond.op = op_eq;
    conds.push_back(cond);
    // two conditions one table
    manager.Select("table1", "", conds);

    lexpr = ReadExpr("table1", "name");
    ReadExpr readrexpr = ReadExpr("table2", "name");
    cond.l = new ReadExpr(lexpr);
    cond.r = new ReadExpr(readrexpr);
    cond.op = op_eq;
    conds.clear();
    conds.push_back(cond);
    // three conditions two tables
    manager.Select("table1", "table2", conds);

    //manager.Delete("test", conds);
}
