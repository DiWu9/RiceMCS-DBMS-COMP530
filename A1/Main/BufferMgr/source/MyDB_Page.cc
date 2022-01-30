#ifndef PAGE_C
#define PAGE_C

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "MyDB_Page.h"
using namespace std;

MyDB_Page :: MyDB_Page (pair<MyDB_TablePtr, long> loc, int size, int offset) {
    this->diskLoc = loc;
    this->offset = offset;
    this->pageSize = size;
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

string MyDB_Page :: getStorageLocation () {
    return this->getLoc().first.getStorageLoc();
}

int MyDB_Page :: getPageSize () {
    return this->pageSize;
}

int MyDB_Page :: getOffset () {
    return this->offset;
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

void MyDB_Page :: writeBackPage () {
    int fd = open(this->getStorageLocation(), O_WRONLY);
    lseek(fd, this->getLoc.second * this->pageSize(), SEEK_SET);
    write(fd, this->getByte, this->pageSize());
    close(fd);
}

void MyDB_Page :: writeBackAnonPage (string tempFile) {
    int fd = open(tempFile, O_WRONLY | O_CREAT);
    lseek(fd, 0, SEEK_CUR);
    write(fd, this->getByte(), this->pageSize());
    close(fd);
}

#endif