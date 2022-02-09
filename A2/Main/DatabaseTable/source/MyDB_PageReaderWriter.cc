
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageHandle.h"
#include "MyDB_Record.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_PageReaderWriter.h"

using namespace std;


MyDB_PageReaderWriter :: MyDB_PageReaderWriter (MyDB_BufferManagerPtr mgrPtr, MyDB_TablePtr tablePtr, long ithPage) {
	this->myMgr = mgrPtr;
	this->myPage = this->myMgr->getPage(tablePtr, ithPage);
	this->pageSize = this->myMgr->getPageSize();
	this->myPageHead = (PageHeader *) this->myPage->getBytes();
}

MyDB_PageReaderWriter :: ~MyDB_PageReaderWriter () {}

// empties out the contents of this page, so that it has no records in it
// the type of the page is set to MyDB_PageType :: RegularPage
void MyDB_PageReaderWriter :: clear () {
	this->setType(MyDB_PageType :: RegularPage);
	this->myPageHead->offsetToEnd = 0;
	this->myPage->wroteBytes();
}

size_t MyDB_PageReaderWriter :: getOffsetToEnd () {
	return this->myPageHead->offsetToEnd;
}

// gets the type of this page... this is just a value from an ennumeration
// that is stored within the page
MyDB_PageType MyDB_PageReaderWriter :: getType () {
	return this->myPageHead->pageType;
}


// return an itrator over this page... each time returnVal->next () is
// called, the resulting record will be placed into the record pointed to
// by iterateIntoMe
MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	MyDB_PageRecIteratorPtr it = make_shared <MyDB_PageRecIterator> (*this, iterateIntoMe);
	return it;
}

// sets the type of the page
void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) {
	this->myPageHead->pageType = toMe;
	this->myPage->wroteBytes();
}

// appends a record to this page... return false is the append fails because
// there is not enough space on the page; otherwise, return true
bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) {
	char * toAppend = & this->myPageHead->recs[0] + this->myPageHead->offsetToEnd;
	size_t appendSize = appendMe->getBinarySize();
	//cout << "rec size: " << appendSize << endl;
	if (toAppend + appendSize <= (char *) this->myPage->getBytes() + this->pageSize) {
		void* newPos = appendMe->toBinary(toAppend);
		this->myPageHead->offsetToEnd += ((char*) newPos - (char*) toAppend);
		//cout << "offset: " << this->myPageHead->offsetToEnd << endl;
		this->myPage->wroteBytes();
		return true;
	}
	return false;
}

#endif
