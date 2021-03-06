
#ifndef CHECKLRU_C
#define CHECKLRU_C

#include "CheckLRU.h"
#include "MyDB_Page.h"

#include <map>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

LRUCache ::LRUCache(size_t cacheCapacity)
{
    this->capacity = cacheCapacity;
    this->counter = 0;
}

LRUCache ::~LRUCache()
{
}

int LRUCache ::getSize()
{
    return this->lruCache.size();
}

/*
Find the unpinned page with the smallest lru number and remove it from the cache
Note: pinned page will not be evicted from RAM and written back to disk

Return the page being evicted; nullptr if no page is evicted.
*/
MyDB_Page *LRUCache ::evictLRU()
{
    for (std::map<size_t, MyDB_Page *>::iterator i = this->lruCache.begin(); i != this->lruCache.end(); i++)
    {
        MyDB_Page *page = i->second;
        if (!page->isPagePinned())
        {
            page->killPage();
            this->lruCache.erase(i->first);
            return page;
        }
    }
    cout << "Error: nothing can be evicted from LRU." << endl;
    return nullptr;
}

/*
Remove a specific page from lru cache by finding its lru number
*/
void LRUCache ::removePage(MyDB_Page *page)
{
    // check if the page actually exist
    if (this->lruCache.find(page->getLRU()) != this->lruCache.end())
    {
        this->lruCache.erase(page->getLRU());
        return;
    }
    return;
}

/*
Add a specific associated with the largest lru number in the cache
*/
void LRUCache ::addPage(MyDB_Page *page)
{
    if (this->capacity == this->getSize())
    {
        this->evictLRU();
    }
    this->counter++; // make sure the inserted page has the largest counter (lru number) so far
    page->setLRU(this->counter);
    this->lruCache.insert(std::pair<int, MyDB_Page *>(this->counter, page));
}

/*
Update the lru number of a page by removing and adding
*/
void LRUCache ::updatePage(MyDB_Page *page)
{
    this->removePage(page);
    this->addPage(page);
}

void LRUCache ::printCache()
{
    for (std::map<size_t, MyDB_Page *>::iterator i = this->lruCache.begin(); i != this->lruCache.end(); i++)
    {
        cout << "LRU Num: " << i->first << "; Page: " << i->second->getOffset() << endl;
    }
}

#endif