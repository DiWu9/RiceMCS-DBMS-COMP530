#ifndef PAGE_C
#define PAGE_C

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"

using namespace std;

MyDB_Page ::MyDB_Page(MyDB_BufferManager *bufferManager, pair<MyDB_TablePtr, long> loc, size_t size, size_t offset)
{
    this->bm = bufferManager;
    this->diskLoc = loc;
    this->offset = offset;
    this->pageSize = size;
    this->refCount = 0;
    this->isPinned = false;
    this->isDirty = false;
    this->isAnonymous = false;
    this->byte = nullptr;
}

MyDB_Page ::~MyDB_Page()
{
}

void MyDB_Page ::killPage()
{
    this->bm->killPage(this);
}

void MyDB_Page ::evictFromLRU()
{
    this->bm->removeFromLRU(this);
}

void MyDB_Page ::setPageToDirty()
{
    this->isDirty = true;
}

void MyDB_Page ::setByte(char *bytePtr)
{
    this->byte = bytePtr;
}

void MyDB_Page ::setByteToNull()
{
    this->byte = nullptr;
}

void MyDB_Page ::setLRU(size_t counter)
{
    this->lruNum = counter;
}

void MyDB_Page ::pinPage()
{
    this->isPinned = true;
}

void MyDB_Page ::unpinPage()
{
    this->isPinned = false;
}

void MyDB_Page ::setPageAnonymous()
{
    this->isAnonymous = true;
}

void MyDB_Page ::incrementRefCount()
{
    this->refCount++;
}

void MyDB_Page ::decreaseRefCount()
{
    this->refCount--;
}

void MyDB_Page ::setOffset(size_t i)
{
    this->offset = i;
}

pair<MyDB_TablePtr, long> MyDB_Page ::getLoc()
{
    return this->diskLoc;
}

string MyDB_Page ::getStorageLocation()
{
    return this->getLoc().first->getStorageLoc();
}

size_t MyDB_Page ::getPageSize()
{
    return this->pageSize;
}

size_t MyDB_Page ::getOffset()
{
    return this->offset;
}

bool MyDB_Page ::isPageNullPtr()
{
    return this->byte == nullptr;
}

bool MyDB_Page ::isPageDirty()
{
    return this->isDirty;
}

bool MyDB_Page ::isPagePinned()
{
    return this->isPinned;
}

bool MyDB_Page ::isPageAnonymous()
{
    return this->isAnonymous;
}

size_t MyDB_Page ::getRefCount()
{
    return this->refCount;
}

size_t MyDB_Page ::getLRU()
{
    return this->lruNum;
}

void *MyDB_Page ::getByte()
{
    if (this->byte == nullptr)
    {
        this->bm->addBack(this);
        return this->byte;
    }
    else
    {
        return this->byte;
    }
}

void MyDB_Page ::writeBackPage()
{
    int fd = open(this->getStorageLocation().c_str(), O_WRONLY | O_CREAT);
    lseek(fd, this->diskLoc.second * this->pageSize, SEEK_SET);
    write(fd, this->getByte(), this->getPageSize());
    close(fd);
}

void MyDB_Page ::writeBackAnonPage(string tempFile)
{
    int fd = open(tempFile.c_str(), O_WRONLY | O_CREAT);
    lseek(fd, this->diskLoc.second * this->pageSize, SEEK_SET);
    write(fd, this->getByte(), this->getPageSize());
    close(fd);
}

void MyDB_Page ::printByte()
{
    char *byte = (char *)this->byte;
    for (size_t i = 0; i < this->pageSize; i++)
    {
        cout << byte[i];
    }
    cout << endl;
}

#endif