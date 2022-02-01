#ifndef PAGE_H
#define PAGE_H

using namespace std;
#include "MyDB_Table.h"

// https://stackoverflow.com/questions/625799/resolve-build-errors-due-to-circular-dependency-amongst-classes
class MyDB_BufferManager;
class MyDB_Page
{

public:
    MyDB_Page(MyDB_BufferManager *bufferManager, pair<MyDB_TablePtr, long> diskLoc, size_t size, size_t offset);

    ~MyDB_Page();

    void killPage(); // remove the page from its buffer manager

    void evictFromLRU(); // evict the page from buffer manager's LRU cache

    /* =============== SETTERS ================ */

    void setPageToDirty();

    void setByte(char *bytePtr);

    void setByteToNull();

    void incrementRefCount();

    void decreaseRefCount();

    void setLRU(size_t counter);

    void pinPage();

    void unpinPage();

    void setPageAnonymous();

    void setOffset(size_t i);

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

    void *getByte();

    /* =============== WRITEBACKS ================
    when a page is evicted from the LRU or the buffer pool is destructed,
    it should be write back to the disk or tempfile if its dirty */

    void writeBackPage(); /* called if dirty and non-anonymous page */

    void writeBackAnonPage(string tempFile); /* called if dirty and anonymous page */

    void printByte();

private:
    MyDB_BufferManager *bm;

    /* the real disk location where the page is come from,
       anonymous page has pair <nullptr, ithAnonPage> */
    pair<MyDB_TablePtr, long> diskLoc;

    size_t offset;           /* position relative to head of buffer pool */
    size_t pageSize;         /* page size                                */
    size_t refCount;         /* number of PageHandle point to this page  */
    bool isPinned;           /* whether the page is pinned               */
    bool isDirty;            /* the page is written                      */
    bool isAnonymous;        /* this page is anonymous                   */
    void *byte;              /* pointer to its position at buffer pool   */
    size_t lruNum;           /* the lower the less recently used (unique)*/
};

#endif
