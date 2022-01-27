#ifndef PAGE_C
#define PAGE_C

#include <iostream>
#include "MyDB_Page.h"
using namespace std;

MyDB_Page :: MyDB_Page () {
    this->refCount = 0;
    this->isPinned = false;
    this->isDirty = false;
    this->isAnonymous = false;
    this->byte = nullptr;
}

MyDB_Page :: ~ MyDB_Page () {
    
}

void MyDB_Page :: setPageToAnonymous () {
    this->isAnonymous = true;
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

void MyDB_Page :: incrementRefCount () {
    this->refCount ++;
}

void MyDB_Page :: decreaseRefCount () {
    this->refCount --;
}

bool MyDB_Page :: isPageNullPtr () {
    return this->byte == nullptr;
}

bool MyDB_Page :: isPageDirty () {
    return this->isDirty;
}

int MyDB_Page :: getLRU () {
    return this->lruNum;
}

#endif