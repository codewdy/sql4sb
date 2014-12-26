#include "table.hpp"
#include <cstdio>
#include <iostream>

int main() {
    std::remove("test_table.db");
    Table* tbl;
    tbl = new Table("test_table.db", true);
    tbl->head->desc.colSize = 1;
    tbl->head->desc.colType[0].size = 4;
    tbl->head->desc.colType[0].type = TYPE_INT;
    tbl->head->desc.colType[0].null = false;
    tbl->rowSize = 100;
    tbl->setDirty(0);
    for (int i = 0; i < 1000; i++) {
        auto XX = tbl->genNewRecord();
        *(int*)XX = i;
        tbl->setDirty(XX);
    }
    tbl->writeback();
    delete tbl;
    tbl = new Table("test_table.db", false);
    std::set<int> x;
    for (auto& rec : tbl->usedRecords) {
        x.insert(*(int*)rec);
    }
    for (int i = 0; i < 1000; i++)
        if (x.find(i) == x.end())
            std::cout << "ERROR" << std::endl;
    delete tbl;
}
