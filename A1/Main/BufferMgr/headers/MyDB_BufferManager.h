
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"

using namespace std;

class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whicTable, i) is that the page will be 
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

    void pin (MyDB_PageHandle pinMe);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);
	
	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS 



private:

	// YOUR STUFF HERE
    char * bufferPool;          //size of the bufferPool
    std::set<size_t> slotSet;      //array to check availability of slot
    size_t size;                //page size
    size_t num;                 //page number
    string anonFile;            //temp file
    std::map<std::pair<MyDB_TablePtr, long>, MyDB_Page> lookUpTable; //lookup Table


};

#endif
//
//Page method:
//void setPageToDirty (MyDB_Page page);
//
//void setByte (MyDB_Page page, char * bytePtr);
//
//void setByteToNull (MyDB_Page page);
//
//pair<MyDB_TablePtr, long> getLoc (MyDB_Page page);
//
//bool isPageNullPtr (MyDB_Page page);
//
//bool isPageDirty (MyDB_Page page);
//
//bool isPinned (MyDB_Page page);
//
//int getRefCount (MyDB_Page page);
//
//int getLRU (MyDB_Page page);
//
//char * getByte (MyDB_Page page);
//
////Page Handle Methods:
//
//void setPageToDirty (MyDB_PageHandle handle);
//
//void setByte (MyDB_PageHandle handle, char * bytePtr);
//
//void setByteToNull (MyDB_PageHandle handle);
//
//void incrementRefCount (MyDB_PageHandle handle);
//
//void decreaseRefCount (MyDB_PageHandle handle);
//
//void setLRU (MyDB_PageHandle handle, int counter);
//
//void pinPage (MyDB_PageHandle handle);
//
//void wroteBytes(MyDB_PageHandle handle);
//
////get
//
//pair<MyDB_TablePtr, long> getLoc (MyDB_PageHandle handle);
//
//bool isPageNullPtr (MyDB_PageHandle handle);
//
//bool isPageDirty (MyDB_PageHandle handle);
//
//bool isPinned (MyDB_PageHandle handle);
//
//int getRefCount (MyDB_PageHandle handle);
//
//int getLRU (MyDB_PageHandle handle);
//
//char * getByte (MyDB_PageHandle handle);
//
//MyDB_Page * getPage(MyDB_PageHandle handle);
//
//
//void incrementRefCount (MyDB_Page page);
//
//void decreaseRefCount (MyDB_Page page);
//
//void setLRU (MyDB_Page page, int counter);
//
//void pinPage (MyDB_Page page);
//
//void unpinPage (MyDB_Page page);
//
////get
