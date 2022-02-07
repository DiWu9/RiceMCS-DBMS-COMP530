
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageHandle.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_PageReaderWriter.h"

MyDB_PageReaderWriter :: MyDB_PageReaderWriter(MyDB_BufferManagerPtr mgrPtr, MyDB_PageHandle pageHandle) {
	this->myMgr = mgrPtr;
	this->myPage = pageHandle;
	this->pageSize = this->myMgr->getPageSize();
	this->myPageHead = (PageHeader *) this->myPage->getBytes();
	this->myPageHead->pageType = MyDB_PageType::RegularPage;
	this->myPageHead->offsetToEnd = 0;
}

// empties out the contents of this page, so that it has no records in it
// the type of the page is set to MyDB_PageType :: RegularPage
void MyDB_PageReaderWriter :: clear () {
	memset(this->myPage->getBytes(), '0', this->pageSize);
	this->setType(MyDB_PageType :: RegularPage);
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
}

// appends a record to this page... return false is the append fails because
// there is not enough space on the page; otherwise, return true
bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) {
	char * toAppend = & this->myPageHead->recs[0] + this->myPageHead->offsetToEnd;
	if (toAppend + appendMe->getBinarySize() <= (char *) this->myPage->getBytes() + this->pageSize) {
		appendMe->toBinary(toAppend);
		this->myPage->wroteBytes();
		this->myPageHead->offsetToEnd += appendMe->getBinarySize();
		return true;
	}
	return false;
}

#endif
