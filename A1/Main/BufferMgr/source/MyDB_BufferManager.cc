
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
    this->bufferPool = (char *) malloc ((pageSize * numPages + 1) * sizeof(char));
    memset(bufferPool, '0', pageSize * numPages);
    for (size_t offset = 0; offset < numPages; offset++)
    {
        this->slotSet.insert(offset);
    }

    this->cache = new LRUCache(numPages);
    this->tempFile = tempFile;
    this->pageNum = numPages;
    this->pageSize = pageSize;
    this->anonymousPageNum = 0;
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
            this->cache->evictLRU();
        }
        MyDB_Page *newPage = this->addPage(key);
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
        this->cache->evictLRU();
    }
    pair<MyDB_TablePtr, long> key = make_pair(nullptr, this->anonymousPageNum);
    this->anonymousPageNum++;
    MyDB_Page *newPage = this->addPage(key);
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
        void *dest = page->getByte();
        pair<MyDB_TablePtr, long> loc = page->getLoc();
        MyDB_TablePtr whichTable = loc.first;
        long i = loc.second;
        int fd = open(whichTable->getStorageLoc().c_str(), O_RDONLY);
        if (fd == -1)
        {
            cout << "Failed to read file. " << endl;
        }
        else
        {
            lseek(fd, i * this->pageSize, SEEK_SET);
            read(fd, dest, this->pageSize);
        }
        close(fd);
    }
}

/*
create a new page and load bytes to buffer pool
*/
MyDB_Page *MyDB_BufferManager ::addPage(pair<MyDB_TablePtr, long> loc)
{
    auto setIt = this->slotSet.begin();
    size_t offset = *setIt;
    MyDB_Page *newPage = new MyDB_Page(this, loc, this->pageSize, offset);
    newPage->setByte(&this->bufferPool[pageSize * offset]);
    MyDB_TablePtr tablePtr = loc.first;
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
    this->slotSet.erase(setIt);
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
    size_t offset = *setIt;
    page->setOffset(offset);
    page->setByte(&this->bufferPool[pageSize * offset]);
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
        if (page->isPageDirty())
        {
            if (page->isPageAnonymous())
            {
                page->writeBackAnonPage(this->tempFile);
            }
            else
            {
                page->writeBackPage();
            }
        }
        memset(page->getByte(), '0', page->getPageSize());
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

void MyDB_BufferManager ::removeFromLRU (MyDB_Page *page)
{
    this->cache->removePage(page);
}

void MyDB_BufferManager ::printSlots () {
    cout << "offset slots remaining: " << endl;
    for (auto it = this->slotSet.begin(); it != this->slotSet.end(); it++)
    {
        cout << ' ' << *it;
    }
    cout << endl;
}

#endif
