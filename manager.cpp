#include <iostream>
#include <cstdlib>
#include <fstream>

#include "manager.hpp"
#include "table.hpp"
#include "object.hpp"

struct DBInfo {
    int tableCount;
    char tables[20][20];
};
void WriteRow(void* rec, const TableDesc& desc, const std::vector<bool>& write);

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
            memcpy(iter, objs[i].loc, desc.colType[i].size);
        nullMask <<= 1;
        (char*&)iter += desc.colType[i].size;
    }
}

Manager::Manager() {
    dbName = "test";
    DBInfo info;
    info.tableCount = 0;
    std::fstream out("test.dbx", std::ios::in | std::ios::binary);
    if (!out) {
        out.close();
        out.open("test.dbx", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write((char*)(void*)&info, sizeof(info));
    }
    out.close();
}

std::string Manager::tblFileName(const std::string& tbl) {
    return dbName + ":" + tbl + ".db";
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
    std::vector<bool> write;
    for ( int i=0; i<table->head->desc.colSize; i++ )
        write.push_back(true);
    for ( const auto& record : filtered) {
        table->removeRecord(record);
        WriteRow(record, table->head->desc, write);
        std::cout << std::endl;

    }
    table->writeback();
}

void WriteRow(void* rec, const TableDesc& desc, const std::vector<bool>& write) {
    unsigned short nullmap = *(unsigned short*)rec;
    (char*&)rec += 2;
    for (int i = 0; i < desc.colSize; i++) {
        const Type& t = desc.colType[i];
        if (write[i])
        {
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
        }
        (char*&)rec += t.size;
    }
}

void Manager::Select(const std::string& tbl1, const std::string& tbl2, const std::vector<Condition>& conds, std::set<std::pair<std::string, std::string>>* sel){
    if ( tbl2 == "" ) {
        auto filtered = filterOne(tbl1, conds);
        auto table = getTable(tbl1, false);
        std::vector<bool> write;
        for (int i = 0; i < table->head->desc.colSize; i++) {
            if (sel == nullptr) {
                write.push_back(true);
                continue;
            }
            std::string name = table->head->desc.colType[i].name;
            if (sel->find(std::make_pair("", name)) != sel->end()) {
                write.push_back(true);
                continue;
            }
            if (sel->find(std::make_pair(tbl1, name)) != sel->end()) {
                write.push_back(true);
                continue;
            }
            write.push_back(false);
        }
        for (auto row : filtered) {
            WriteRow(row, table->head->desc, write);
            std::cout << std::endl;
        }
    } else {
        auto filtered = filterTwo(tbl1, tbl2, conds);
        auto table1 = getTable(tbl1, false);
        auto table2 = getTable(tbl2, false);
        std::vector<bool> write1;
        for (int i = 0; i < table1->head->desc.colSize; i++) {
            if (sel == nullptr) {
                write1.push_back(true);
                continue;
            }
            std::string name = table1->head->desc.colType[i].name;
            if (sel->find(std::make_pair("", name)) != sel->end()) {
                write1.push_back(true);
                continue;
            }
            if (sel->find(std::make_pair(tbl1, name)) != sel->end()) {
                write1.push_back(true);
                continue;
            }
            write1.push_back(false);
        }
        std::vector<bool> write2;
        for (int i = 0; i < table2->head->desc.colSize; i++) {
            if (sel == nullptr) {
                write2.push_back(true);
                continue;
            }
            std::string name = table2->head->desc.colType[i].name;
            if (sel->find(std::make_pair("", name)) != sel->end()) {
                write2.push_back(true);
                continue;
            }
            if (sel->find(std::make_pair(tbl2, name)) != sel->end()) {
                write2.push_back(true);
                continue;
            }
            write2.push_back(false);
        }
        for (auto row : filtered) {
            WriteRow(row.first, table1->head->desc, write1);
            WriteRow(row.second, table2->head->desc, write2);
            std::cout << std::endl;
        }
    }
}

void Manager::Update(const std::string& tbl, const std::vector<Condition>& conds, ReadExpr& lv, const Object& rv){
    std::vector<void*> filtered = filterOne(tbl, conds);
    Table* table = getTable(tbl, false);
    
    std::vector<bool> write;
    for ( int i=0; i<table->head->desc.colSize; i++ )
        write.push_back(true);

    for ( void* record : filtered ) {
        WriteRow(record, table->head->desc, write);
        
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
    std::fstream dbf(dbName + ".dbx", std::ios::in | std::ios::out | std::ios::binary);
    DBInfo info;
    dbf.read((char*)(void*)&info, sizeof(info));
    strcpy(info.tables[info.tableCount++], tbl.c_str());
    dbf.seekp(0);
    dbf.write((char*)(void*)&info, sizeof(info));
}


void Manager::DropTable(const std::string& tbl) {
    tables.erase(tblFileName(tbl));
    std::remove(tblFileName(tbl).c_str());
    std::fstream dbf(dbName + ".dbx", std::ios::in | std::ios::out | std::ios::binary);
    DBInfo info;
    dbf.read((char*)(void*)&info, sizeof(info));
    for (int i = 0; i < info.tableCount; i++)
        if (info.tables[i] == tbl)
            memcpy(info.tables[i], info.tables[--info.tableCount], 20);
    dbf.seekp(0);
    dbf.write((char*)(void*)&info, sizeof(info));
}

void Manager::ShowTables() {
    std::fstream dbf(dbName + ".dbx", std::ios::in | std::ios::out | std::ios::binary);
    DBInfo info;
    dbf.read((char*)(void*)&info, sizeof(info));
    for (int i = 0; i < info.tableCount; i++)
        std::cout << info.tables[i] << std::endl;
}

void Manager::CreateDB(const std::string& dbName) {
    std::fstream out(dbName + ".dbx", std::ios::out | std::ios::binary | std::ios::trunc);
    DBInfo info;
    info.tableCount = 0;
    out.write((char*)(void*)&info, sizeof(info));
    out.close();
}

void Manager::DropDB(const std::string& dbName) {
    std::remove((dbName + ".dbx").c_str());
}

void Manager::Desc(const std::string& tbl) {
    Table* table = getTable(tbl, false);
    for (int i = 0; i < table->head->desc.colSize; i++) {
        std::cout << table->head->desc.colType[i].name;
        if (table->head->desc.colType[i].type == TYPE_INT)
            std::cout << " INT";
        else
            std::cout << " STRING";
        std::cout << std::endl;
    }
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
                if (!conds[k].op(l->getObj(record1, record2),
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

