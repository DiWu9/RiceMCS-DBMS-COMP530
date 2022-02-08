
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <iostream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_TableRecIterator.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;
	this->myMgr = myBuffer;

	// there has never been anything written to the table (the table has just been initialized and setLastPage
	// has never been called)
	if (this->myTable->lastPage() == -1) {
		this->myTable->setLastPage(0);
		this->lastPage = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myMgr->getPage(this->myTable, 0));
		this->lastPage->clear();
	}
	else {
		int lastIndex = this->myTable->lastPage();
		this->lastPage = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myMgr->getPage(this->myTable, lastIndex));
	}
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	return MyDB_PageReaderWriter(this->myMgr, this->myMgr->getPage(this->myTable, i));
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	MyDB_SchemaPtr mySchema = this->myTable->getSchema();
	return make_shared <MyDB_Record>(mySchema);
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	return *this->lastPage;
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// append record to the last page
	// if append failed, add a new page and append the record
	if (!this->lastPage->append(appendMe)) {
		int newLastIndex = this->myTable->lastPage()+1;
		this->myTable->setLastPage(newLastIndex);
		this->lastPage = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myMgr->getPage(this->myTable, newLastIndex));
		this->lastPage->append(appendMe);
	}
}

// load a text file into this table... overwrites the current contents
// ref (https://www.w3schools.com/cpp/cpp_files.asp)
void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	ifstream toRead;
	toRead.open(fromMe);
	string myText;
	MyDB_RecordPtr currRec = this->getEmptyRecord();
	while (getline(toRead, myText)) {
		currRec->fromString(myText);
	}
	this->append(currRec);
	toRead.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	MyDB_TableRecIteratorPtr it = make_shared <MyDB_TableRecIterator> (*this, iterateIntoMe);
	return it;
}

// dump the contents of this table into a text file
void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	ofstream toWrite;
	toWrite.open(toMe);
	MyDB_RecordPtr currRec = this->getEmptyRecord();
	MyDB_RecordIteratorPtr it = this->getIterator(currRec);
	while (it->hasNext()) {
		it->getNext();
	}
	toWrite.close();
}

#endif

