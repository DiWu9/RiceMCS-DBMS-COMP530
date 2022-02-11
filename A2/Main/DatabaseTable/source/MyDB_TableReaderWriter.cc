
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <iostream>

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_TableRecIterator.h"

using namespace std;

void MyDB_TableReaderWriter :: printPages () {
	cout << "=========== Print Pages ===============" << endl;
	for (size_t i = 0; i <= this->getIndexOfLastPage(); i++) {
		cout << "Page "  << i << " offset: " << this->operator[](i).getOffsetToEnd() << endl;
	}
}

size_t MyDB_TableReaderWriter :: getIndexOfLastPage () {
	return this->myTable->lastPage();
}

MyDB_PageReaderWriterPtr MyDB_TableReaderWriter :: getIthPageReaderWriter (size_t i) {
	while (this->getIndexOfLastPage() < i) {
		this->myTable->setLastPage(this->getIndexOfLastPage()+1);
		make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, this->myTable->lastPage())->clear();
	}
	MyDB_PageReaderWriterPtr page = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, i);
	return page;
}

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;
	this->myMgr = myBuffer;

	// there has never been anything written to the table (the table has just been initialized and setLastPage
	// has never been called)
	if (this->getIndexOfLastPage() == (size_t) -1) {
		this->myTable->setLastPage(0);
		this->lastPage = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, 0);
		this->lastPage->clear();
	}
	else {
		size_t lastIndex = this->getIndexOfLastPage();
		this->lastPage = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, lastIndex);
	}
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	return *make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, i);
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	MyDB_SchemaPtr mySchema = this->myTable->getSchema();
	return make_shared <MyDB_Record>(mySchema);
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	return this->operator[](this->getIndexOfLastPage());
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// append record to the last page
	// if append failed, add a new page and append the record
	if (!this->lastPage->append(appendMe)) {
		//cout << "last page " << this->getIndexOfLastPage() << ": offset: " << this->operator[](this->getIndexOfLastPage()).getOffsetToEnd() << endl;
		//this->printPages();
		this->myTable->setLastPage(this->getIndexOfLastPage()+1);
		this->lastPage = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, this->getIndexOfLastPage());
		this->lastPage->clear();
		this->lastPage->append(appendMe);
	}
	//cout << "last page " << this->getIndexOfLastPage() << ": offset: " << this->operator[](this->getIndexOfLastPage()).getOffsetToEnd() << endl;
}

// load a text file into this table... overwrites the current contents
void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	ifstream toRead;
	toRead.open(fromMe);
	string myText;
	MyDB_RecordPtr currRec = this->getEmptyRecord();
	while (getline(toRead, myText)) {
		currRec->fromString(myText);
		this->append(currRec);
	}
	//cout << this->getIndexOfLastPage() << "th page's offset: " << this->lastPage->getOffsetToEnd() << endl;
	toRead.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	MyDB_RecordIteratorPtr it = make_shared <MyDB_TableRecIterator> (*this, iterateIntoMe);
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