#ifndef PAGE_ITER_C
#define PAGE_ITER_C

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_PageRecIterator.h"

// each time returnVal->next () is called, the resulting record will be placed into the record pointed to by iterateIntoMe
void MyDB_PageRecIterator :: getNext() {
    PageHeader* pageHead = this->myPageReaderWriter.myPageHead;
    size_t recSize = this->rec->getBinarySize();
    this->rec->fromBinary(& pageHead->recs[0] + this->offset);
    this->offset += recSize;
}

bool MyDB_PageRecIterator :: hasNext() {
    cout << "Offset: " << this->offset << endl;
    cout << "Page size: " << this->myPageReaderWriter.pageSize << endl;
    return this->offset + this->rec->getBinarySize() <= this->myPageReaderWriter.pageSize;
}

MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_PageReaderWriter& pageReaderWriterPtr, MyDB_RecordPtr iterateIntoMe) : myPageReaderWriter(pageReaderWriterPtr) {
    this->rec = iterateIntoMe;
    this->offset = 0;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator() {}

#endif
