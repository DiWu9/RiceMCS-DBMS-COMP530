#ifndef PAGE_ITER_C
#define PAGE_ITER_C

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_PageRecIterator.h"

using namespace std;

// each time returnVal->next () is called, the resulting record will be placed into the record pointed to by iterateIntoMe
void MyDB_PageRecIterator :: getNext() {
    PageHeader* pageHead = this->myPageReaderWriter.myPageHead;
    void *pos = &pageHead->recs[this->offset];
    void *nextPos = this->rec->fromBinary(pos);
    this->offset += ((char *) nextPos) - ((char *) pos);
}

bool MyDB_PageRecIterator :: hasNext() {
    return this->offset < this->myPageReaderWriter.myPageHead->offsetToEnd;
}

MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_PageReaderWriter& pageReaderWriterPtr, MyDB_RecordPtr iterateIntoMe) : myPageReaderWriter(pageReaderWriterPtr) {
    this->rec = iterateIntoMe;
    this->offset = 0;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator() {}

#endif
