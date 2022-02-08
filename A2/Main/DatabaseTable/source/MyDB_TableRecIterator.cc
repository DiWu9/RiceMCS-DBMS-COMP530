#ifndef TABLE_ITER_C
#define TABLE_ITER_C

#include "MyDB_RecordIterator.h"
#include "MyDB_TableRecIterator.h"

// each time returnVal->next () is called, the resulting record will be placed into the record pointed to by iterateIntoMe
void MyDB_TableRecIterator :: getNext() {
    if (this->pageIt->hasNext()) {
        this->pageIt->getNext();
    }
    else {
        this->ithPage ++;
        this->pageIt = this->myTableReaderWriter[this->ithPage].getIterator(this->rec);
        this->pageIt->getNext();
    }
}

bool MyDB_TableRecIterator :: hasNext() {
    if (this->pageIt->hasNext()) {
        return true;
    }
    else {
        cout << "Last Page Index: " << this->myTableReaderWriter.myTable->lastPage() << endl;
        return this->ithPage < this->myTableReaderWriter.myTable->lastPage();
    }
}

MyDB_TableRecIterator :: MyDB_TableRecIterator (MyDB_TableReaderWriter& tableReaderWriterPtr, MyDB_RecordPtr iterateIntoMe) : myTableReaderWriter(tableReaderWriterPtr) {
    this->rec = iterateIntoMe;
    this->ithPage = 0;
    this->pageIt = this->myTableReaderWriter[this->ithPage].getIterator(this->rec);
}

MyDB_TableRecIterator :: ~MyDB_TableRecIterator() {}

#endif