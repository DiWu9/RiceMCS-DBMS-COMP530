
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
find the page with the smallest lru number and remove it from the cache
*/
void LRUCache :: evictLRU () {
    int smallestKey = this->map.begin()->first;
    this->map.erase(smallestKey);
    this->size --;
}

/* 
remove a specific page from lru cache by finding its lru number
*/
void LRUCache :: removePage ( MyDB_Page * page ) {
    // check if the page actually exist
    if (this->map.find(page->getLRU()) != this->map.end()) {
        this->map.erase(page->getLRU());
        this->size --;
    }
}

/* 
add a specific associated with the largest lru number in the cache
*/
void LRUCache :: addPage ( MyDB_Page * page ) {
    if (this->size == this->capacity) {
        this->evictLRU();
    }
    this->counter++;
    page->setLRU(this->counter);
    this->map
}

#endif