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

class LRUCache {

public:

    LRUCache ( int cacheCapacity );

    ~LRUCache ();

    void evictLRU();

    void removePage( MyDB_Page * page );

    void addPage( MyDB_Page * page );

private:

    int capacity;                       // the capacity of cache
    int size;                           // the current size of cache
    int counter;                        // the counter that tracks the "time"
    std::map<int, MyDB_Page *> map;     // lruNum->Page mapping

};

#endif
