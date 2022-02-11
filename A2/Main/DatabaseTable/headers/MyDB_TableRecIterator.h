#ifndef TABLE_ITER_H
#define TABLE_ITER_H

#include <memory>

#include "MyDB_Record.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

class MyDB_TableRecIterator;
typedef shared_ptr <MyDB_TableRecIterator> MyDB_TableRecIteratorPtr;

class MyDB_TableRecIterator : public MyDB_RecordIterator {

public:

    void getNext();
    
    bool hasNext();

    MyDB_TableRecIterator (MyDB_TableReaderWriter& tableReaderWriterPtr, MyDB_RecordPtr iterateIntoMe);
    ~MyDB_TableRecIterator ();

private:

    MyDB_TableReaderWriter& myTableReaderWriter;
    MyDB_RecordPtr rec;
    MyDB_RecordIteratorPtr pageIt;
    MyDB_PageReaderWriterPtr pageRW;
    size_t ithPage;

};

#endif