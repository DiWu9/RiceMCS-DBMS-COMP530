#ifndef PAGE_H
#define PAGE_H

using namespace std;
#include "MyDB_Table.h"

class MyDB_Page {

public:

    MyDB_Page (pair<MyDB_TablePtr, long> diskLoc);

    // Triggered if:
    // 1. Page is anonymous and refCount is 0
    // 2. Page is non-anon, refCount is 0, and evicted from LRU Cache
    ~MyDB_Page ();

    // ============== modifiers ==============

    void setPageToDirty ();

    void setByte (char * bytePtr);

    void setByteToNull ();

    void incrementRefCount ();

    void decreaseRefCount ();

    void setLRU (int counter);

    void pinPage ();

    void unpinPage ();

    // =============== getters ================

    pair<MyDB_TablePtr, long> getLoc ();

    bool isPageNullPtr ();

    bool isPageDirty ();

    bool isPinned ();

    int getRefCount ();

    int getLRU ();

    char * getByte ();

private:

    // the real disk location where the page is come from, 
    // anonymous page has pair <nullptr, 0>
    pair<MyDB_TablePtr, long> diskLoc;

    int refCount;                           // number of PageHandle point to this page
    bool isPinned;                          // whether the page is pinned
    bool isDirty;                           // the page is written
    bool isAnonymous;                       // this page is anonymous
    char * byte;                            // pointer to its position at buffer pool
    int lruNum;                             // the lower the less recently used (unique)

};

#endif

