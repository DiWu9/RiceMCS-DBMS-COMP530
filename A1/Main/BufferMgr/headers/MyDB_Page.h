#ifndef PAGE_H
#define PAGE_H

using namespace std;
#include "MyDB_Table.h"

class MyDB_Page
{

public:
    MyDB_Page(pair<MyDB_TablePtr, long> diskLoc, size_t size, size_t offset);

    /* Triggered if:
        1. Page is anonymous and refCount is 0
        2. Page is non-anon, refCount is 0, and evicted from LRU Cache */
    ~MyDB_Page();

    /* =============== SETTERS ================ */

    void setPageToDirty();

    void setByte(char *bytePtr);

    void setByteToNull();

    void incrementRefCount();

    void decreaseRefCount();

    void setLRU(size_t counter);

    void pinPage();

    void unpinPage();

    /* =============== GETTERS ================ */

    pair<MyDB_TablePtr, long> getLoc();

    string getStorageLocation();

    size_t getPageSize();

    size_t getOffset();

    bool isPageNullPtr();

    bool isPageDirty();

    bool isPagePinned();

    bool isPageAnonymous();

    size_t getRefCount();

    size_t getLRU();

    char *getByte();

    /* =============== WRITEBACKS ================
    when a page is evicted from the LRU or the buffer pool is destructed,
    it should be write back to the disk or tempfile if its dirty */

    void writeBackPage(); /* called if dirty and non-anonymous page */

    void writeBackAnonPage(string tempFile); /* called if dirty and anonymous page */

private:
    /* the real disk location where the page is come from,
       anonymous page has pair <nullptr, 0> */
    pair<MyDB_TablePtr, long> diskLoc;

    size_t offset;    /* position relative to head of buffer pool */
    size_t pageSize;  /* page size                                */
    size_t refCount;  /* number of PageHandle point to this page  */
    bool isPinned;    /* whether the page is pinned               */
    bool isDirty;     /* the page is written                      */
    bool isAnonymous; /* this page is anonymous                   */
    char *byte;       /* pointer to its position at buffer pool   */
    size_t lruNum;    /* the lower the less recently used (unique)*/
};

#endif
