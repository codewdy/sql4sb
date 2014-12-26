#include <iostream>
#include <cstdlib>

#include "manager.hpp"
#include "table.hpp"
#include "object.hpp"

void checkType(Type type, const Object& obj) {
    //TODO
}

void* WriteObj(void* buf, Type type, const Object& obj) {
    memcpy(buf, obj.loc, obj.size);
    memset((char*)buf + obj.size, 0, type.size - obj.size);
    return (char*)buf + type.size;
}

void Manager::Insert(const std::string& tbl, const std::vector<std::vector<Object>>& rows) {
    auto tblX = getTable(tbl, false);
    for (auto row : rows) {
        if (row.size() != tblX->desc->colSize) {
            throw "Column Size Error;";
        }
        for (int i = 0; i < row.size(); i++) {
            checkType(tblX->desc->colType[i], row[i]);
        }
    }
    for (auto row : rows) {
        void* rec = (char*)tblX->genNewRecord();
        tblX->setDirty(rec);
        for (int i = 0; i < row.size(); i++) {
            rec = WriteObj(rec, tblX->desc->colType[i], row[i]);
        }
    }
}
void Manager::Delete(const std::string& tbl, const std::vector<Condition>& conds){
    Table* table = getTable(tbl, false);
    std::vector<void*> filtered = filterOne(tbl, conds);
    for ( const auto& record : filtered) {
        table->removeRecord(record);
    }
}

void Manager::Select(const std::string& tbl1, const std::string& tbl2, const std::vector<Condition>& conds){
    std::vector<void*> filtered;
    if ( tbl2 != "" ) {
        filtered = filterOne(tbl1, conds);
    } else {
        filtered = filterTwo(tbl1, tbl2, conds);
    }

    // TODO :
    // 输出
}

void Manager::Update(const std::string& tbl, const std::vector<Condition>& conds, const ReadExpr& lv, const Object& rv){
    std::vector<void*> filtered = filterOne(tbl, conds);
    for ( void* record : filtered ) {
        Object obj = lv.getObj(record);
        TYPE type = obj.type;
        WriteObj(&obj, type, rv);
       
    }
}


void Manager::Use(const std::string& db) {
    dbName = db;
}


void Manager::CreateTable(const std::string& tbl, const std::vector<TYPE>& types){
    Table table(tbl, true);
    HeadPage headpage;
    headpage.pageCount = 0;
    headpage.infoHeadPage = 0;
    
    TableDesc desc;
    desc.colSize = types.size();
    for ( int i=0; i<types.size(); i++ ) {
        desc.colType[i] = types[i];
    }

    headpage.desc = desc;

    table.pages[0] = &headpage;
    table.dirtyPages.insert(&headpage);
    
}


void Manager::DropTable(const std::string& tbl) {

}
