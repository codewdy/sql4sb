#include <iostream>
#include <cstdlib>

#include "manager.hpp"
#include "stmt.hpp"

void InsertStmt::Run(Manager& manager) {
    manager.Insert(tbl, rows);
}

void DeleteStmt::Run(Manager& manager) {
    manager.Delete(tbl, conds);
}

void SelectStmt::Run(Manager& manager) {
    manager.Select(tbl1, tbl2, conds);
}

void UpdateStmt::Run(Manager& manager) {
    manager.Update(tbl, conds, lv, obj);
}

void CreateTableStmt::Run(Manager& manager) {
    manager.CreateTable(tbl, types);
}

void DropTableStmt::Run(Manager& manager) {
    manager.DropTable(tbl);
}

void UseStmt::Run(Manager& manager) {
    manager.Use(db);
}

void DropDBStmt::Run(Manager& manager) {
    manager.DropDB(db);
}
