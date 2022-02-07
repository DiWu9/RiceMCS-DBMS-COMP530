#ifndef PAGE_ITER_C
#define PAGE_ITER_C

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_PageRecIterator.h"

// return an itrator over this page... each time returnVal->next () is
// called, the resulting record will be placed into the record pointed to
// by iterateIntoMe
void MyDB_PageRecIterator :: getNext() {
    PageHeader* pageHead = this->myPageReaderWriter.myPageHead;
    this->rec->fromBinary(& pageHead->recs[0] + this->offset);
    this->offset += this->rec->getBinarySize();
}

bool MyDB_PageRecIterator :: hasNext() {
    PageHeader* pageHead = this->myPageReaderWriter.myPageHead;
    return this->offset + this->rec->getBinarySize() <= pageHead->offsetToEnd;
}

MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_PageReaderWriter& pageReaderWriterPtr, MyDB_RecordPtr iterateIntoMe) : myPageReaderWriter(pageReaderWriterPtr) {
    this->rec = iterateIntoMe;
    this->offset = 0;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator() {}

#endif
