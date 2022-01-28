
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
	return this->getPage()->getByte();
}

void MyDB_PageHandleBase :: wroteBytes () {
	this->getPage()->setPageToDirty();
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	this->getPage()->decreaseRefCount();
}

MyDB_Page * MyDB_PageHandleBase :: getPage () {
	return this->page;
}

#endif

