
#ifndef CHECKLRU_C
#define CHECKLRU_C

#include "CheckLRU.h"

#include <map>

using namespace std;

LRUCache :: LRUCache (int cacheCapacity) {
    this->capacity = cacheCapacity;
    this->size = 0;
    this->counter = 0;
}

LRUCache :: ~LRUCache () {

}

/* 
Find the unpinned page with the smallest lru number and remove it from the cache
Note: pinned page will not be evicted from RAM and written back to disk
*/
void LRUCache :: evictLRU () {
    for (std::map<int,MyDB_Page *>::iterator i=this->lruCache.begin(); i != this->lruCache.end(); i++) {
        if (!i->second->isPinned()) {
            this->lruCache.erase(i->first);
            this->size --;
            break;
        }
    }
}

/* 
Remove a specific page from lru cache by finding its lru number
*/
void LRUCache :: removePage ( MyDB_Page * page ) {
    // check if the page actually exist
    if (this->lruCache.find(page->getLRU()) != this->lruCache.end()) {
        this->lruCache.erase(page->getLRU());
        this->size --;
    }
}

/* 
Add a specific associated with the largest lru number in the cache
*/
void LRUCache :: addPage ( MyDB_Page * page ) {
    if (this->size == this->capacity) {
        this->evictLRU();
    }
    this->counter++; // make sure the inserted page has the largest counter (lru number) so far
    page->setLRU(this->counter);
    this->lruCache.insert(std::pair<int, MyDB_Page *>(this->counter, page));
}

#endif