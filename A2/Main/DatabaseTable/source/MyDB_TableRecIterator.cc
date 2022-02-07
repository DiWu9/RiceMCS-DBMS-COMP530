#ifndef TABLE_ITER_C
#define TABLE_ITER_C

#include "MyDB_RecordIterator.h"
#include "MyDB_TableRecIterator.h"

// return an itrator over this page... each time returnVal->next () is
// called, the resulting record will be placed into the record pointed to
// by iterateIntoMe
void MyDB_TableRecIterator :: getNext() {
}

bool MyDB_TableRecIterator :: hasNext() {
}

MyDB_TableRecIterator :: MyDB_TableRecIterator () {
}

MyDB_TableRecIterator :: ~MyDB_TableRecIterator() {}

#endif