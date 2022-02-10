
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
	//MyDB_PageReaderWriterPtr page = make_shared <MyDB_PageReaderWriter>(this->myMgr, this->myTable, this->myTable->lastPage());
	for (int i = 0; i <= this->myTable->lastPage(); i++) {
		MyDB_PageReaderWriterPtr page = this->pageRefs[i];
		//cout << "last page offset: " << this->last().getOffsetToEnd() << endl;
		//cout << "last page index: " << this->myTable->lastPage() << endl;
		//cout << "Page "  << i << " offset: " << page->getOffsetToEnd() << endl;
	}
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIthPageIterator (MyDB_RecordPtr iterateIntoMe, size_t i) {
	MyDB_PageReaderWriterPtr page = this->pageRefs[i];
	return make_shared <MyDB_PageRecIterator>(*page, iterateIntoMe);
}

size_t MyDB_TableReaderWriter :: getIndexOfLastPage () {
	return this->myTable->lastPage();
}

size_t MyDB_TableReaderWriter :: getVecSize () {
	return this->pageRefs.size();
}

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;
	this->myMgr = myBuffer;
	this->counter = 0;

	// there has never been anything written to the table (the table has just been initialized and setLastPage
	// has never been called)
	if (this->getIndexOfLastPage() == -1) {
		this->myTable->setLastPage(0);
		MyDB_PageReaderWriterPtr pageZero = make_shared <MyDB_PageReaderWriter>(*this, 0);
		pageZero->clear();
		this->lastPage = pageZero;
		this->pageRefs.push_back(pageZero);
	}
	else {
		int lastIndex = this->getIndexOfLastPage();
		for (int i = 0; i <= lastIndex; i++) {
			MyDB_PageReaderWriterPtr pageI = make_shared <MyDB_PageReaderWriter>(*this, i);
			this->pageRefs.push_back(pageI);
		}
		this->lastPage = this->pageRefs[lastIndex];
	}
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	if (this->getIndexOfLastPage() < i) {
		while (this->getIndexOfLastPage() < i) {
			this->myTable->setLastPage (this->getIndexOfLastPage() + 1);
			MyDB_PageReaderWriterPtr pageI = make_shared <MyDB_PageReaderWriter>(*this, this->myTable->lastPage());
			pageI->clear();
			this->pageRefs.push_back(pageI);
		}
		this->lastPage = this->pageRefs[this->getIndexOfLastPage()];
		return *this->lastPage;
	}
	else {
		return *this->pageRefs[i];
	}
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
	//bool success = true;
	if (!this->lastPage->append(appendMe)) {
		this->myTable->setLastPage(this->getIndexOfLastPage()+1);
		MyDB_PageReaderWriterPtr newLastPage = make_shared <MyDB_PageReaderWriter>(*this, this->getIndexOfLastPage()+1);
		newLastPage->clear();
		newLastPage->append(appendMe);
		this->pageRefs.push_back(newLastPage);
		this->lastPage = newLastPage;
	}
	// if (!success) {
	// 	this->counter++;
	// 	cout << "append fail " << this->counter << endl;
	// }
	//cout << this->getIndexOfLastPage() << "th page." << endl;
	//cout << "page's offset to end: " << this->lastPage->getOffsetToEnd() << endl;
	//cout << "---------------------------------------" << endl;
}

// load a text file into this table... overwrites the current contents
void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	ifstream toRead;
	toRead.open(fromMe);
	string myText;
	MyDB_RecordPtr currRec = this->getEmptyRecord();
	//int i = 1;
	while (getline(toRead, myText)) {
		//cout << "line " << i << endl;
		currRec->fromString(myText);
		this->append(currRec);
		//cout << "last page: " << this->getIndexOfLastPage() << endl;
		//cout << "last page offset: " << this->lastPage->getOffsetToEnd() << endl;
		//i++;
	}
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

