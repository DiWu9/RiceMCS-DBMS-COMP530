
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
	char * byte = this->getPage()->getByte();
	if (byte == nullptr) {
		// (implement this)
		// if the page is not currently in the buffer, then the contents of the page are loaded from 
		// secondary storage.
		// can get page's disk location by MyDB_Page :: getLoc ()
	}
	else {
		return this->getPage()->getByte();
	}
}

void MyDB_PageHandleBase :: wroteBytes () {
	this->getPage()->setPageToDirty();
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	this->getPage()->decreaseRefCount();
	// If the number of references to a pinned page goes down to zero, then the page should
	// become unpinned.  
	if (this->getPage()->getRefCount() == 0) {
		this->getPage()->unpinPage();
	}
}

MyDB_Page * MyDB_PageHandleBase :: getPage () {
	return this->page;
}

#endif

