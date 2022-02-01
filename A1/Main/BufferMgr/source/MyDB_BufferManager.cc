
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"
#include "CheckLRU.h"
#include "MyDB_Page.h"
#include "MyDB_PageHandle.h"

#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

MyDB_BufferManager ::MyDB_BufferManager(size_t pageSize, size_t numPages, string tempFile)
{
    this->bufferPool = (char *)malloc(pageSize * numPages * sizeof(char));
    this->cache = new LRUCache(numPages);
    this->tempFile = tempFile;
    this->pageNum = numPages;
    this->pageSize = pageSize;
    for (size_t offset = 0; offset < numPages; offset++)
    {
        this->slotSet.insert(offset);
    }
}

MyDB_BufferManager ::~MyDB_BufferManager()
{
    for (map<std::pair<MyDB_TablePtr, long>, MyDB_Page *>::iterator i = this->lookUpTable.begin(); i != this->lookUpTable.end(); i++)
    {
        MyDB_Page *currPage = i->second;
        if (currPage->isPageDirty())
        {
            if (currPage->isPageAnonymous())
            {
                currPage->writeBackAnonPage(this->tempFile);
            }
            else
            {
                currPage->writeBackPage();
            }
        }
        currPage->setByte(nullptr);
    }
    free(this->bufferPool);
}

/*
Get non-anonymous page
*/
MyDB_PageHandle MyDB_BufferManager ::getPage(MyDB_TablePtr whichTable, long i)
{
    cout << "Test slots left: " << endl;
    for (auto it = this->slotSet.begin(); it != this->slotSet.end(); ++it)
        cout << ' ' << *it;
    cout << endl;
    pair<MyDB_TablePtr, long> key = make_pair(whichTable, i);
    std::map<std::pair<MyDB_TablePtr, long>, MyDB_Page *>::iterator it = this->lookUpTable.find(key);
    // if it exists in the lookup table
    if (it != this->lookUpTable.end())
    {
        MyDB_Page *page = it->second;
        this->cache->updatePage(page);
        shared_ptr<MyDB_PageHandleBase> pageHandle(new MyDB_PageHandleBase(page));
        return pageHandle;
    }
    // if it doesn't exist in the lookup table
    else
    {
        // if the buffer pool is full
        if (this->slotSet.empty())
        {
            MyDB_Page *evictedPage = this->cache->evictLRU();
            this->killPage(evictedPage);
        }
        // page point to bufferpool* + offset
        // add page to lookup table
        // add page to lru cache
        // erase the assigned offset from slotset
        // return pointer to the page
        MyDB_Page * newPage = this->addPage(key);
        shared_ptr<MyDB_PageHandleBase> pageHandle(new MyDB_PageHandleBase(newPage));
        return pageHandle;
    }
}

/*
Get Anonymous page
*/
MyDB_PageHandle MyDB_BufferManager ::getPage()
{
    // if the buffer pool is full
    if (this->slotSet.empty())
    {
        MyDB_Page *evictedPage = this->cache->evictLRU();
        this->killPage(evictedPage);
    }
    // page point to bufferpool* + offset
    // set page to anonymous
    // add page to lru cache
    // erase the assigned offset from slotset
    // return pointer to the page
    pair<MyDB_TablePtr, long> key = make_pair(nullptr, 0);
    MyDB_Page * newPage = this->addPage(key);
    shared_ptr<MyDB_PageHandleBase> pageHandle(new MyDB_PageHandleBase(newPage));
    return pageHandle;
}

MyDB_PageHandle MyDB_BufferManager ::getPinnedPage(MyDB_TablePtr whichTable, long i)
{
    MyDB_PageHandle pageHandle = this->getPage(whichTable, i);
    pageHandle->getPage()->pinPage();
    return pageHandle;
}

MyDB_PageHandle MyDB_BufferManager ::getPinnedPage()
{
    MyDB_PageHandle pageHandle = this->getPage();
    pageHandle->getPage()->pinPage();
    return pageHandle;
}

void MyDB_BufferManager ::unpin(MyDB_PageHandle unpinMe)
{
    unpinMe->getPage()->unpinPage();
}

void MyDB_BufferManager ::pin(MyDB_PageHandle pinMe)
{
    pinMe->getPage()->pinPage();
}

void MyDB_BufferManager ::readFromDisk(MyDB_Page *page)
{
    if (!page->isPageAnonymous())
    {
        char *dest = page->getByte();
        pair<MyDB_TablePtr, long> loc = page->getLoc();
        MyDB_TablePtr whichTable = loc.first;
        long i = loc.second;
        size_t pageSize = page->getPageSize();
        int fd = open(whichTable->getStorageLoc().c_str(), O_RDONLY);
        cout << "Addr read to: " << &dest << endl;
        lseek(fd, i, SEEK_SET);
        read(fd, dest, pageSize);
        close(fd);
    }
}

MyDB_Page *MyDB_BufferManager ::addPage(pair<MyDB_TablePtr, long> loc)
{
    MyDB_TablePtr tablePtr = loc.first;
    long i = loc.second;
    auto setIt = this->slotSet.begin();
    MyDB_Page *newPage = new MyDB_Page(this, loc, this->pageSize, *setIt);
    newPage->setByte(this->bufferPool + *setIt * pageSize);
    if (tablePtr != nullptr)
    {
        this->readFromDisk(newPage);
        this->lookUpTable.insert(make_pair(loc, newPage));
    }
    else 
    {
        newPage->setPageAnonymous();
    }
    this->cache->addPage(newPage);
    slotSet.erase(setIt);
    return newPage;
}

void MyDB_BufferManager ::addBack(MyDB_Page *page)
{
    if (this->slotSet.empty())
    {
        MyDB_Page *evictedPage = this->cache->evictLRU();
        this->killPage(evictedPage);
    }
    auto setIt = this->slotSet.begin();
    page->setOffset(*setIt);
    page->setByte(this->bufferPool + *setIt * pageSize);
    this->readFromDisk(page);
    if (!page->isPageAnonymous())
    {
        this->lookUpTable.insert(make_pair(page->getLoc(), page));
    }
    this->cache->addPage(page);
    slotSet.erase(setIt);
}

void MyDB_BufferManager ::killPage(MyDB_Page *page)
{
    if (page->getByte() != nullptr)
    {
        this->slotSet.insert(page->getOffset());
        memset(page->getByte(), 0, page->getPageSize());
        page->setByte(nullptr);
    }

    if (this->inLookUpTable(page->getLoc()))
    {
        this->lookUpTable.erase(page->getLoc());
    }
}

bool MyDB_BufferManager ::inLookUpTable(pair<MyDB_TablePtr, long> pageKey)
{
    std::map<std::pair<MyDB_TablePtr, long>, MyDB_Page *>::iterator it = this->lookUpTable.find(pageKey);
    return it != this->lookUpTable.end();
}

#endif
