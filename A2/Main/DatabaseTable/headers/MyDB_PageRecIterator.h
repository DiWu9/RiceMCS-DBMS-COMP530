#ifndef PAGE_ITER_H
#define PAGE_ITER_H

#include <memory>

#include "MyDB_PageReaderWriter.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"

using namespace std;

class MyDB_PageRecIterator;
typedef shared_ptr <MyDB_PageRecIterator> MyDB_PageRecIteratorPtr;

class MyDB_PageRecIterator : public MyDB_RecordIterator {

public:

    void getNext();
    
    bool hasNext();

    MyDB_PageRecIterator (MyDB_PageReaderWriter& pageReaderWriter, MyDB_PageReaderWriterPtr pageReaderWriterPtr, MyDB_RecordPtr iterateIntoMe);
    ~MyDB_PageRecIterator ();

private:

    MyDB_PageReaderWriter& myPageReaderWriter;
    MyDB_PageReaderWriterPtr myPageReaderWriterPtr;
    MyDB_RecordPtr rec;
    size_t offset;

};
#endif