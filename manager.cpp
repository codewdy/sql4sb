#include "manager.hpp"

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
