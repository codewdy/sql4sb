#include "object.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>

Object LiteralManager::GetVarChar(std::string& l) {
    Object obj;
    obj.loc = &l;
    obj.size = l.size();
    obj.type = TYPE_VARCHAR;
    obj.is_null = false; // ?
    return obj;
}

Object LiteralManager::GetInt(int l) {
    Object obj;
    obj.loc = &l;
    obj.size = sizeof(l);
    obj.type = TYPE_INT;
    obj.is_null = false;
    return obj;
}

Object ReadExpr::getObj(void* l, void* r){
    if ( useLeft )
        return *((Object*)l+offset);
    else 
        return *((Object*)r+offset);
}

void ReadExpr::Use(const std::string& lname, const std::string& rname, TableDesc* ldesc, TableDesc* rdesc) {
    offset = 0;
    if (tbl == lname) {
        useLeft = true;
        for ( int i=0; i<MaxCol; i++ ) {
            if ( std::strcmp(ldesc->colType[i].name , name.c_str()) == 0 ) {
                size = ldesc->colType[i].size;
                return;
            } else {
                offset += ldesc->colType[i].size;
            }
        }
    } else {
        useLeft = false;
         for ( int i=0; i<MaxCol; i++ ) {
            if ( std::strcmp(rdesc->colType[i].name, name.c_str()) == 0 ) {
                size = ldesc->colType[i].size;
                return;
            } else {
                offset += ldesc->colType[i].size;
            }
        }
    }
}

Object LiteralExpr::getObj(void* l, void* r) {
    return obj;
}

void LiteralExpr::Use(const std::string& lname, const std::string& rname, TableDesc* ldesc, TableDesc* rdesc){
    return;
}

bool op_eq(const Object& lobj, const Object& robj){
    if ( lobj.type != robj.type ) {
        throw "Type Error";
        return false;
    }
    if ( lobj.is_null != robj.is_null )
        return false;
    if ( lobj.type == TYPE_INT ){
        int* l = (int*)lobj.loc;
        int* r = (int*)robj.loc;
        return (*l == *r);
    } else {
        if (strcmp((char*)lobj.loc, (char*)robj.loc) == 0)
            return true;
        return false;
    }
}

bool op_ne(const Object& lobj, const Object& robj){
    if ( lobj.type != robj.type ) {
        throw "Type Error";
        return true;
    }
    if ( lobj.is_null != robj.is_null )
        return true;
    if ( lobj.type == TYPE_INT ){
        int* l = (int*)lobj.loc;
        int* r = (int*)robj.loc;
        return (*l != *r);
    } else {
        if (strcmp((char*)lobj.loc, (char*)robj.loc) == 0)
            return false;
        return true;
    }
}

bool op_le(const Object& lobj, const Object& robj){
    if ( lobj.type != robj.type ) {
        throw "Type Error";
        return false;
    }
    if ( lobj.is_null != robj.is_null )
        return false;
    if ( lobj.type == TYPE_INT ){
        int* l = (int*)lobj.loc;
        int* r = (int*)robj.loc;
        return (*l <= *r);
    } else {
        if (strcmp((char*)lobj.loc, (char*)robj.loc) == -1 ||
                strcmp((char*)lobj.loc, (char*)robj.loc) == 0 )
            return true;
        return false;
    }
}

bool op_ge(const Object& lobj, const Object& robj){
        if ( lobj.type != robj.type ) {
        throw "Type Error";
        return false;
    }
    if ( lobj.is_null != robj.is_null )
        return false;
    if ( lobj.type == TYPE_INT ){
        int* l = (int*)lobj.loc;
        int* r = (int*)robj.loc;
        return (*l >= *r);
    } else {
        if (strcmp((char*)lobj.loc, (char*)robj.loc) == 1 ||
                strcmp((char*)lobj.loc, (char*)robj.loc) == 0 )
            return true;
        return false;
    }
}

bool op_lt(const Object& lobj, const Object& robj){
    if ( lobj.type != robj.type ) {
        throw "Type Error";
        return false;
    }
    if ( lobj.is_null != robj.is_null )
        return false;
    if ( lobj.type == TYPE_INT ){
        int* l = (int*)lobj.loc;
        int* r = (int*)robj.loc;
        return (*l < *r);
    } else {
        if (strcmp((char*)lobj.loc, (char*)robj.loc) == -1  )
            return true;
        return false;
    }
}

bool op_gt(const Object& lobj, const Object& robj){
        if ( lobj.type != robj.type ) {
        throw "Type Error";
        return false;
    }
    if ( lobj.is_null != robj.is_null )
        return false;
    if ( lobj.type == TYPE_INT ){
        int* l = (int*)lobj.loc;
        int* r = (int*)robj.loc;
        return (*l > *r);
    } else {
        if (strcmp((char*)lobj.loc, (char*)robj.loc) == 1 )
            return true;
        return false;
    }
}



    
