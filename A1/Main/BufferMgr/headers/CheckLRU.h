#ifndef CHECKLRU_H
#define CHECKLRU_H

/* LRU cache implemented using std::map (ref: https://en.cppreference.com/w/cpp/container/map)
*/
#include "MyDB_Page.h"

#include <iostream>
#include <iterator>
#include <map>

using namespace std;

class LRUCache
{

public:
    LRUCache(size_t cacheCapacity);

    ~LRUCache();

    int getSize();

    MyDB_Page *evictLRU();

    void removePage(MyDB_Page *page);

    void updatePage(MyDB_Page *page);

    void printCache ();

private:

    size_t capacity;                   // the capacity of cache
    size_t counter;                    // the counter that tracks the "time"
    map<size_t, MyDB_Page *> lruCache; // lruNum->Page mapping

    void addPage(MyDB_Page *page);
};

#endif
