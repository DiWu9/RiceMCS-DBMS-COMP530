
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	MyDB_PageReaderWriter temp;
	return temp;	
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return nullptr;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	MyDB_PageReaderWriter temp;
	return temp;	
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return nullptr;
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
}

#endif

