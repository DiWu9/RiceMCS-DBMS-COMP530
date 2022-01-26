#ifndef CHECKLRU_H
#define CHECKLRU_H

/* LRU cache implemented using std::map (ref: https://en.cppreference.com/w/cpp/container/map)
When 
*/
#include "MyDB_Page.h"

#include <iostream>
#include <iterator>
#include <map>

using namespace std;

class PageContainer {

public:

    PageContainer ();

    ~PageContainer ();

private:

    MyDB_Page * page;                   // pointer to page object
    int lruNum;                         // page's LRU number

};

class LRUCache {

public:

    LRUCache ();

    ~LRUCache ();

    void evictLRU();

    void addPage();

private:

    int capacity;                       // the capacity of cache
    int size;                           // the current size of cache
    int counter;                        // the counter that tracks the "time"
    std::map<int, PageContainer *> map; // lru->PageContainer mapping

};

#endif
