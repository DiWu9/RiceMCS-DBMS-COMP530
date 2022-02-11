
#ifndef TABLE_ITER_C
#define TABLE_ITER_C

#include "MyDB_Record.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

// each time returnVal->next () is called, the resulting record will be placed into the record pointed to by iterateIntoMe
void MyDB_TableRecIterator :: getNext() {
    //cout << "ith page: " << this->ithPage << endl;
    this->pageIt->getNext();
}

bool MyDB_TableRecIterator :: hasNext() {
    while (this->ithPage < this->myTableReaderWriter.getIndexOfLastPage()) {
        if (this->pageIt->hasNext()) {
            return true;
        }
        this->ithPage ++;
        this->pageRW = this->myTableReaderWriter.getIthPageReaderWriter(this->ithPage);
        this->pageIt = this->pageRW->getIterator(this->rec);
    }
    // now it's the pageIt for the last page
    return this->pageIt->hasNext();
}

MyDB_TableRecIterator :: MyDB_TableRecIterator (MyDB_TableReaderWriter& tableReaderWriterPtr, MyDB_RecordPtr iterateIntoMe) : myTableReaderWriter(tableReaderWriterPtr) {
    this->rec = iterateIntoMe;
    this->ithPage = 0;
    this->pageRW = this->myTableReaderWriter.getIthPageReaderWriter(0);
    this->pageIt = this->pageRW->getIterator(this->rec);
}

MyDB_TableRecIterator :: ~MyDB_TableRecIterator() {}

#endif