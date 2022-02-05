#ifndef REC_ITER_C
#define REC_ITER_C

#include "MyDB_PageRecIterator.h"


void MyDB_PageRecIterator :: getNext()  {
    void *pos = bytesConsumed + (char *) myPage->getBytes ();
    void *nextPos = myRec->fromBinary (pos);
    bytesConsumed += ((char *) nextPos) - ((char *) pos);
}

bool MyDB_PageRecIterator :: hasNext()  {
    return bytesConsumed < pageSize;
}

MyDB_PageRecIterator :: MyDB_PageRecIterator(MyDB_PagePtr page, size_t byte, size_t size) {
    myPage = page;
    bytesConsumed = byte;
    pageSize = size;
};

MyDB_PageRecIterator :: ~MyDB_PageRecIterator(){}

#endif
