#ifndef TABLE_H
#define TABLE_H
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <string>
#include <vector>
#include "type.hpp"
const int PAGE_SIZE = 8192;
const int NAME_LEN = 10;
struct Type {
    TYPE type;
    bool null;
    int size;
    char name[NAME_LEN];
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
    static const int RowBitmapSize = 2;
    std::string filename;
    std::unordered_map<void*, Info*> recordInfoMap;
    std::unordered_set<Info*> usedRecords, emptyRecords;
    std::map<void*, int> pageIndex;
    std::vector<void*> pages;
    std::set<int> dirtyPages;
    int rowSize;
    HeadPage* head;
    InfoPage* LastInfoPage;
    Table(const std::string& _filename, bool init = false);
    void setDirty(void* dst);
    void setDirty(int page_id);
    void writeback();
    void* getPage(int page_id);
    void* genNewRecord();
    void removeRecord(void*);
    int newPage();
};
#endif
