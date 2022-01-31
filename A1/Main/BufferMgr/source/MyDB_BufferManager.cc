
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
    if(lookUpTable.find(pair<whichTable,i>) != null){
        LRU->addPage(lookUpTable.find(pair<whichTable,i>));
        MyDB_PageHandleBase * handle = new MyDB_PageHandleBase(lookUpTable.find(pair<whichTable,i>));

    }else {
        if(slotSet.empty()){
            MyDB_Page evictedPage = LRU->evictLRU();
            if(evictedPage -> isPageDirty()){
                if(evictedPage -> isAnonymous()){
                    evictedPage -> writeBackAnonPage (anonFile);
                }else{
                    evictedPage ->  writeBackPage ();
                }
            }
            slotSet.insert(evictedPage -> getOffset ());
        }
        auto firstIterator = slotset.begin();
        slotSet.erase(firstIterator);
        MyDB_Page *page = new MyDB_Page(pair < whichTable, i >, firstIterator*);
        lookUpTable.insert({pair<whichTable, i>,page});
        LRU->addPage(page);
        page-> setByte(bufferPool + firstIterator*);
        char byte[size];
        memcpy(byte, whichTable + i /* Offset */, size /* Length */);
        //bufferPool 写入
        MyDB_PageHandleBase * handle = new MyDB_PageHandleBase(page);

    }
    return handle;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {

    if(slotSet.empty()){
        MyDB_Page evictedPage = LRU->evictLRU();
        if(evictedPage -> isPageDirty()){
            if(evictedPage -> isAnonymous()){
                evictedPage -> writeBackAnonPage (anonFile);
            }else{
                evictedPage ->  writeBackPage ();
            }
        }
        slotSet.insert(evictedPage -> getOffset ());
    }
    auto firstIterator = slotset.begin();
    slotSet.erase(firstIterator);
    MyDB_Page *page = new MyDB_Page(pair < nullptr, 0 >, firstIterator*);
    LRU->addPage(page);
    page-> setByte(bufferPool + firstIterator*);
    char byte[size];
    memcpy(byte, whichTable + i /* Offset */, size /* Length */);
    //bufferPool 写入
    MyDB_PageHandleBase * handle = new MyDB_PageHandleBase(page);


    return handle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    MyDB_PageHandle * handle = new getPage(whichTable, i);
    pin(handle);
    return handle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    MyDB_PageHandle * handle = new getPage();
    pin(handle);
    return handle;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinMe->getPage()->unpinPage();
}

void MyDB_BufferManager :: pin (MyDB_PageHandle pinMe) {
    pinMe->getPage()->pinPage();
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    size = pageSize;
    num = numPages;
    anonFile = tempFile;
    for (size_t i=0; i<num; ++i){
        slotSet.insert(i*size);
    }
    bufferPool = pageSize * numPages;
    LRUCache LRU = new LRUCache(int(bufferPool));
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
    while(!LRU,isempty()){
        MyDB_Page evictedPage = LRU->evictLRU();
        if(evictedPage -> isPageDirty()){
            if(evictedPage -> isAnonymous()){
                evictedPage -> writeBackAnonPage (anonFile);
            }else{
                evictedPage ->  writeBackPage ();
            }
        }
    }
}


//Page methods:
//
//void MyDB_BufferManager :: setPageToDirty (MyDB_Page page){
//    page -> setPageToDirty ();
//};
//
//void MyDB_BufferManager :: setByte (MyDB_Page page, char * bytePtr){
//    page -> setByte();
//};
//
//void MyDB_BufferManager :: setByteToNull (MyDB_Page page){
//    page -> setByteToNull ()
//};
//
//void MyDB_BufferManager :: setLRU (MyDB_Page page, int counter){
//    page -> setLRU (int counter);
//};
//
//void MyDB_BufferManager :: pinPage (MyDB_Page page){
//    page -> pinPage ();
//};
//
//void MyDB_BufferManager :: unpinPage (MyDB_Page page){
//    page -> unpinPage ()
//};
//
//
//void MyDB_BufferManager :: incrementRefCount (MyDB_Page page){
//    page -> incrementRefCount ();
//};
//
//void MyDB_BufferManager :: decreaseRefCount (MyDB_Page page){
//    page -> decreaseRefCount ();
//};
//
//
//pair<MyDB_TablePtr, long> MyDB_BufferManager :: getLoc (MyDB_Page page) {
//    return page -> getLoc();
//}
//
//bool MyDB_BufferManager :: isPageNullPtr (MyDB_Page page) {
//    return page -> isPageNullPtr();
//}
//
//bool MyDB_BufferManager :: isPageDirty (MyDB_Page page) {
//    return page->isPageDirty();
//}
//
//bool MyDB_BufferManager :: isPinned (MyDB_Page page) {
//    return page->isPinned();
//}
//
//int MyDB_BufferManager :: getRefCount (MyDB_Page page) {
//    return page->getRefCount();
//}
//
//int MyDB_BufferManager :: getLRU (MyDB_Page page) {
//    return page->getLRU();
//}
//
//char * MyDB_BufferManager :: getByte (MyDB_Page page) {
//    return page->getByte();
//}
#endif


