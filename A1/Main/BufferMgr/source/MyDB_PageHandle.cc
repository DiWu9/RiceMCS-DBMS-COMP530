
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include <iostream>
#include "MyDB_Page.h"
#include "MyDB_PageHandle.h"

using namespace std;

void *MyDB_PageHandleBase ::getBytes()
{
	return this->page->getByte();
}

void MyDB_PageHandleBase ::wroteBytes()
{
	this->getPage()->setPageToDirty();
}

MyDB_PageHandleBase ::MyDB_PageHandleBase(MyDB_Page *pagePtr)
{
	this->page = pagePtr;
	this->page->incrementRefCount();
}

MyDB_PageHandleBase ::~MyDB_PageHandleBase()
{
	this->getPage()->decreaseRefCount();
	// If the number of references to a pinned page goes down to zero, then the page should
	// become unpinned.
	if (this->getPage()->getRefCount() == 0)
	{
		this->getPage()->unpinPage();
	}
	// If refCount = 0 and page is anonymous, kill the page
	if (this->getPage()->isPageAnonymous())
	{
		this->getPage()->killPage();
		this->getPage()->evictFromLRU();
	}
}

MyDB_Page *MyDB_PageHandleBase ::getPage()
{
	return this->page;
}

#endif
