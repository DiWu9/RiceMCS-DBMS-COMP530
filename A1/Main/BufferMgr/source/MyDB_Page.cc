#ifndef PAGE_C
#define PAGE_C

#include <iostream>
#include "MyDB_Page.h"
using namespace std;

MyDB_Page :: MyDB_Page (pair<MyDB_TablePtr, long> loc) {
    this->diskLoc = loc;
    this->refCount = 0;
    this->isPinned = false;
    this->isDirty = false;
    this->isAnonymous = false;
    this->byte = nullptr;
}

MyDB_Page :: ~ MyDB_Page () {
    
}

void MyDB_Page :: setPageToDirty () {
    this->isDirty = true;
}

void MyDB_Page :: setByte (char * bytePtr) {
    this->byte = bytePtr;
}

void MyDB_Page :: setByteToNull () {
    this->byte = nullptr;
}

void MyDB_Page :: setLRU (int counter) {
    this->lruNum = counter;
}

void MyDB_Page :: pinPage () {
    this->isPinned = true;
}

void MyDB_Page :: unpinPage () {
    this->isPinned = false;
}

void MyDB_Page :: incrementRefCount () {
    this->refCount ++;
}

void MyDB_Page :: decreaseRefCount () {
    this->refCount --;
}

pair<MyDB_TablePtr, long> MyDB_Page :: getLoc () {
    return this->diskLoc;
}

bool MyDB_Page :: isPageNullPtr () {
    return this->byte == nullptr;
}

bool MyDB_Page :: isPageDirty () {
    return this->isDirty;
}

bool MyDB_Page :: isPinned () {
    return this->isPinned;
}

int MyDB_Page :: getRefCount () {
    return this->refCount;
}

int MyDB_Page :: getLRU () {
    return this->lruNum;
}

char * MyDB_Page :: getByte () {
    return this->byte;
}

#endif