#include <iostream>
#include <cstdlib>

#include "manager.hpp"
#include "table.hpp"
#include "object.hpp"

void checkType(Type type, const Object& obj) {
}

void WriteBinRow(void* buf, const TableDesc& desc, const std::vector<Object>& objs) {
    unsigned short nullMask = 1;
    unsigned short nullX = 0;
    void* iter = (char*)buf + 2;
    for (int i = 0; i < desc.colSize; i++) {
        if (objs[i].is_null)
            nullX |= nullMask;
        else
            memcpy(iter, objs[i].loc, desc.colSize);
        nullMask <<= 1;
        (char*&)iter += desc.colType[i].size;
    }
}

void Manager::Insert(const std::string& tbl, const std::vector<std::vector<Object>>& rows) {
    auto tblX = getTable(tbl, false);
    for (auto row : rows) {
        if (row.size() != tblX->head->desc.colSize) {
            throw "Column Size Error;";
        }
        for (int i = 0; i < row.size(); i++) {
            checkType(tblX->head->desc.colType[i], row[i]);
        }
    }
    for (auto row : rows) {
        void* rec = (char*)tblX->genNewRecord();
        tblX->setDirty(rec);
        WriteBinRow(rec, tblX->head->desc, row);
    }
    tblX->writeback();
}
void Manager::Delete(const std::string& tbl, const std::vector<Condition>& conds){
    Table* table = getTable(tbl, false);
    std::vector<void*> filtered = filterOne(tbl, conds);
    for ( const auto& record : filtered) {
        table->removeRecord(record);
    }
    table->writeback();
}

void WriteRow(void* rec, const TableDesc& desc) {
    unsigned short nullmap = *(unsigned short*)rec;
    (char*&)rec += 2;
    for (int i = 0; i < desc.colSize; i++) {
        const Type& t = desc.colType[i];
        if (nullmap & (1 << i)) {
            std::cout << "NULL ";
        } else {
            switch (t.type) {
                case TYPE_INT:
                    std::cout << *(int*)rec << " ";
                    break;
                case TYPE_VARCHAR:
                    for (char* p = (char*)rec; *p && p != (char*)rec + t.size; p++)
                        std::cout << *p;
                    std::cout << " ";
                    break;
            };
        }
        (char*&)rec += t.size;
    }
}

void Manager::Select(const std::string& tbl1, const std::string& tbl2, const std::vector<Condition>& conds){
    if ( tbl2 == "" ) {
        auto filtered = filterOne(tbl1, conds);
        auto table = getTable(tbl1, false);
        for (auto row : filtered) {
            WriteRow(row, table->head->desc);
            std::cout << std::endl;
        }
    } else {
        auto filtered = filterTwo(tbl1, tbl2, conds);
        auto table1 = getTable(tbl1, false);
        auto table2 = getTable(tbl2, false);
        for (auto row : filtered) {
            WriteRow(row.first, table1->head->desc);
            WriteRow(row.second, table2->head->desc);
            std::cout << std::endl;
        }
    }
}

void Manager::Update(const std::string& tbl, const std::vector<Condition>& conds, ReadExpr& lv, const Object& rv){
    std::vector<void*> filtered = filterOne(tbl, conds);
    Table* table = getTable(tbl, false);
    for ( void* record : filtered ) {
        if (rv.is_null) {
            *(unsigned short*)record |= lv.nullMask;
        } else {
            *(unsigned short*)record &= ~lv.nullMask;
            Object obj = lv.getObj(record);
            memcpy(obj.loc, rv.loc, lv.size);
        }
        table->setDirty(record);
    }
    table->writeback();
}


void Manager::Use(const std::string& db) {
    dbName = db;
}


void Manager::CreateTable(const std::string& tbl, const std::vector<Type>& types){
    Table* table = getTable(tbl, true);
    table->head->desc.colSize = types.size();
    table->rowSize = Table::RowBitmapSize;
    for ( int i=0; i<types.size(); i++ ) {
        table->head->desc.colType[i] = types[i];
        table->rowSize += types[i].size;
    }
    table->setDirty(0);
    table->writeback();
}


void Manager::DropTable(const std::string& tbl) {
    tables.erase(tblFileName(tbl));
    std::remove(tblFileName(tbl).c_str());
}

Table* Manager::getTable(const std::string& tbl, bool init){
    std::string t_tbl = tblFileName(tbl);
    if (init) {
        auto& ptbl = tables[t_tbl];
        if (ptbl == nullptr) {
            ptbl = new Table(t_tbl, true);
            return ptbl;
        }
        throw "Table Already Exist";
    } else {
        auto& ptbl = tables[t_tbl];
        if (ptbl == nullptr) {
            ptbl = new Table(t_tbl, true);
        }
        return ptbl;
    }
}

std::vector<void*> Manager::filterOne(const std::string& tbl, const std::vector<Condition>& conds) {
    std::vector<void*> filtered;
    Table* table = getTable(tbl, false);
    for (int k = 0; k < conds.size(); k++) {
        conds[k].l->Use(tbl, "", &table->head->desc);
        conds[k].r->Use(tbl, "", &table->head->desc);
    }
    for ( auto record : table->usedRecords ) {
        bool OK = true;
       for (int k = 0; k<conds.size(); k++ ) {
           Expr *l = conds[k].l;
           Expr *r = conds[k].r;
           if (!conds[k].op(l->getObj(record, nullptr), r->getObj(record, nullptr))) {
               OK = false;
               break;
           }
       }
       if (OK)
           filtered.push_back(record);
    }
    return std::move(filtered);
}


std::vector<std::pair<void*, void*>> Manager::filterTwo(const std::string& tbl1, const std::string& tbl2, const std::vector<Condition>& conds) {
    std::vector<std::pair<void*, void*>> filtered;
    Table* table1 = getTable(tbl1, false);
    Table* table2 = getTable(tbl2, false);
    for (int k = 0; k < conds.size(); k++) {
        conds[k].l->Use(tbl1, tbl2, &table1->head->desc, &table2->head->desc);
        conds[k].r->Use(tbl1, tbl2, &table1->head->desc, &table2->head->desc);
    }
    for ( auto record1 : table1->usedRecords ) {
        for (auto record2 : table2->usedRecords) {
            bool OK = true;
            for (int k = 0; k<conds.size(); k++ ) {
                Expr *l = conds[k].l;
                Expr *r = conds[k].r;
                if (conds[k].op(l->getObj(record1, record2),
                            r->getObj(record1, record2))) {
                    OK = false;
                    break;
                }
            }
            if (OK)
                filtered.push_back(std::make_pair(record1, record2));
        }
    }
    return std::move(filtered);
}

