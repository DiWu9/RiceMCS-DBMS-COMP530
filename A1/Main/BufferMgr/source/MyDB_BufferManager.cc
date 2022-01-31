
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"
#include "CheckLRU.h"
#include "MyDB_Page.h"
#include "MyDB_PageHandle.h"

#include <string>

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
    for (map<std::pair<MyDB_TablePtr, long>, MyDB_Page *>::iterator i = this->lookUpTable.begin(); i != this->lookUpTable.end(); i++) {
        MyDB_Page * currPage = i->second;
        if (currPage->isPageDirty()) {
            if (currPage->isPageAnonymous()) {
                currPage->writeBackAnonPage(this->tempFile);
            }
            else {
                currPage->writeBackPage();
            }
        }
        // free (currPage->getByte());
        currPage->setByte(nullptr);
    }


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
            MyDB_Page *evictedPage = this->cache->evictLRU();
            // if no pages are evicted: which could happend when there are no unpinned page in the bp
            if (evictedPage == nullptr)
            {
                cout << "LRUCache error: evict page failed!";
                abort();
            }
            // if the lru page is evicted
            // remove the pair from lookup table
            // adding page's offset back to slotset
            // write page back to disk if it's dirty
            else
            {
                this->lookUpTable.erase(evictedPage->getLoc());
                this->slotSet.insert(evictedPage->getOffset());
                if (evictedPage->isPageAnonymous())
                {
                    evictedPage->writeBackAnonPage(this->tempFile);
                }
                else
                {
                    evictedPage->writeBackPage();
                }
            }
        }
        // page point to bufferpool* + offset
        // add page to lookup table
        // add page to lru cache
        // erase the assigned offset from slotset
        // return pointer to the page
        auto setIt = this->slotSet.begin();
        MyDB_Page *newPage = new MyDB_Page(key, this->pageSize, *setIt);
        newPage->setByte(this->bufferPool + *setIt * pageSize);
        this->lookUpTable.insert(make_pair(key, newPage));
        this->cache->addPage(newPage);
        slotSet.erase(setIt);
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
        if (evictedPage == nullptr)
        {
            cout << "LRUCache error: evict page failed!";
            abort();
        }
        else
        {
            this->lookUpTable.erase(evictedPage->getLoc());
            this->slotSet.insert(evictedPage->getOffset());
            if (evictedPage->isPageAnonymous())
            {
                evictedPage->writeBackAnonPage(this->tempFile);
            }
            else
            {
                evictedPage->writeBackPage();
            }
        }
    }
    // page point to bufferpool* + offset
    // set page to anonymous
    // add page to lru cache
    // erase the assigned offset from slotset
    // return pointer to the page
    auto setIt = this->slotSet.begin();
    pair<MyDB_TablePtr, long> key = make_pair(nullptr, 0);
    MyDB_Page *newPage = new MyDB_Page(key, this->pageSize, *setIt);
    newPage->setPageAnonymous();
    newPage->setByte(this->bufferPool + *setIt * pageSize);
    this->cache->addPage(newPage);
    slotSet.erase(setIt);
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

#endif
