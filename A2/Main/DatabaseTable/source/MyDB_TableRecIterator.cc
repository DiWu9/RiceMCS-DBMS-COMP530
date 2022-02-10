#ifndef TABLE_ITER_C
#define TABLE_ITER_C

#include "MyDB_Record.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;


// each time returnVal->next () is called, the resulting record will be placed into the record pointed to by iterateIntoMe
void MyDB_TableRecIterator :: getNext() {
    while (!this->pageIt->hasNext()) {
        this->ithPage++;
        this->pageIt = this->myTableReaderWriter[this->ithPage].getIterator(this->rec);
    }
    //cout << this->ithPage << "th page." << endl;
    this->pageIt->getNext();
}

bool MyDB_TableRecIterator :: hasNext() {
    //cout << "ith page: " << this->ithPage << endl;
    if (this->pageIt->hasNext()) {
        return true;
    }
    else {
        int i = this->ithPage + 1;
        int indexOfLastPage = this->myTableReaderWriter.getIndexOfLastPage();
        while (i <= indexOfLastPage) {
            //cout << "ith page: " << this->ithPage << endl;
            MyDB_RecordIteratorPtr tempIt = this->myTableReaderWriter[i].getIterator(this->rec);
            if (tempIt->hasNext()) {
                return true;
            }
            i++;
        }
        return false;
    }
}

MyDB_TableRecIterator :: MyDB_TableRecIterator (MyDB_TableReaderWriter& tableReaderWriterPtr, MyDB_RecordPtr iterateIntoMe) : myTableReaderWriter(tableReaderWriterPtr) {
    this->rec = iterateIntoMe;
    this->ithPage = 0;
    this->pageIt = this->myTableReaderWriter[this->ithPage].getIterator(this->rec);
}

MyDB_TableRecIterator :: ~MyDB_TableRecIterator() {}

#endif