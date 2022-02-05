#ifndef REC_ITER_H
#define REC_ITER_H

//other sources need to be included?
#include <memory>
#include "MyDB_Page.h"
using namespace std;


class MyDB_PageRecIterator;
typedef shared_ptr <MyDB_RecordIterator> MyDB_RecordIteratorPtr;

class MyDB_PageRecIterator : public virtual MyDB_RecordIterator{

public:

    //add bytes consumed rec
    void getNext();

    //whether there is next rec
    bool hasNext();

    // destructor and contructor
    MyDB_PageRecIterator (MyDB_PagePtr page, size_t byte, size_t size) {};

    ~MyDB_PageRecIterator () {};

private:
    MyDB_PagePtr myPage;
    size_t byteConsumed;
    size_t pageSize;

};
