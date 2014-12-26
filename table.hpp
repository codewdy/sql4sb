#ifndef TABLE_H
#define TABLE_H
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>
#include "type.hpp"
const int PAGE_SIZE = 8192;
struct Type {
    TYPE type;
    bool null;
    int size;
    void operator = (Type& ty) {
        type = ty.type;
        null = ty.null;
        size = ty.size;
    }
};
const int MaxCol = (PAGE_SIZE - 16) / sizeof(Type);
struct TableDesc {
    int colSize;
    Type colType[MaxCol];
};
struct HeadPage {
    int infoHeadPage;
    int pageCount;
    TableDesc desc;
};
struct Info {
    int page_id;
    int offset;
    bool free;
};
const int MaxInfo = (PAGE_SIZE - 16) / sizeof(Info);
struct InfoPage {
    int nextPage;
    int size;
    Info infos[MaxInfo];
};
struct Table {
    std::string filename;
    std::unordered_map<void*, Info*> recordInfoMap;
    std::unordered_set<void*> usedRecords, emptyRecords;
    std::map<void*, int> pageIndex;
    std::unordered_map<int, void*> pages;
    std::unordered_set<void*> dirtyPages;
    int rowSize;
    TableDesc* desc;
    Table(const std::string& _filename, bool init = false);
    void setDirty(void* dst);
    void writeback();
    void getPage(int page_id);
    void* genNewRecord();
    void removeRecord(void*);
    int newPage();
};
#endif
