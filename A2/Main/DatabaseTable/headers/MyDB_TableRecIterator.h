#ifndef TABLE_ITER_H
#define TABLE_ITER_H

#include <memory>

#include "MyDB_RecordIterator.h"

using namespace std;

class MyDB_TableRecIterator;
typedef shared_ptr <MyDB_TableRecIterator> MyDB_TableRecIteratorPtr;

class MyDB_TableRecIterator : public MyDB_RecordIterator {

public:

    void getNext();
    
    bool hasNext();

    MyDB_TableRecIterator();
    ~MyDB_TableRecIterator();

private:

};

#endif