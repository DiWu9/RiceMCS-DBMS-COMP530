#ifndef PAGE_H
#define PAGE_H

using namespace std;

class MyDB_Page {

public:

    MyDB_Page () {
        refCount = 0;
        isAnonymous = false;
        isDirty = false;
        byte = nullptr;
    }

    // Trigger if:
    // 1. Page is anonymous and refCount is 0
    // 2. Page is non-anon, refCount is 0, and evicted from LRU Cache
    ~MyDB_Page () {

    }

    // ============== modifiers ==============

    void setPageToAnonymous () {
        isAnonymous = true;
    }

    void setPageToDirty () {
        isDirty = true;
    }

    void setByte (char * bytePtr) {
        byte = bytePtr;
    }

    void setByteToNull () {
        byte = nullptr;
    }

    void incrementRefCount () {
        refCount ++;
    }

    void decreaseRefCount () {
        if (refCount > 0) {
            refCount --;
        }
    }

    // ============== getters ==============

    bool isPageNullPtr () {
        return byte == nullptr;
    }

    bool isPageDirty () {
        return isDirty;
    }

private:

    int refCount;
    bool isAnonymous;
    bool isDirty; // the page is written
    char * byte;

};

#endif

