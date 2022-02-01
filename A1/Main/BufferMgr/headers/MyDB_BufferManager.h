
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include "MyDB_Table.h"
#include "CheckLRU.h"
#include "MyDB_Page.h"
#include "MyDB_PageHandle.h"

#include <set>
#include <map>
#include <vector>

using namespace std;
class MyDB_BufferManager
{

public:
	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage(MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular
	// table
	MyDB_PageHandle getPage();

	// gets the i^th page in the table whichTable... the only difference
	// between this method and getPage (whicTable, i) is that the page will be
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage(MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage();

	// un-pins the specified page
	void unpin(MyDB_PageHandle unpinMe);

	void pin(MyDB_PageHandle pinMe);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager(size_t pageSize, size_t numPages, string tempFile);

	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS
	void readFromDisk(MyDB_Page *page);

	MyDB_Page *addPage(pair<MyDB_TablePtr, long> loc); // add page from disk

	void addBack(MyDB_Page *page); // add back the page that is originally in disk but get evicted

	void killPage(MyDB_Page *page);

	bool inLookUpTable(pair<MyDB_TablePtr, long> pageKey);

	void removeFromLRU (MyDB_Page *page);

	void printSlots ();

private:
	// YOUR STUFF HERE
	char *bufferPool;											  // bufferPool
	set<size_t> slotSet;										  // array to check availability of slot
	size_t pageSize;											  // page size
	size_t pageNum;												  // page number
	string tempFile;											  // temp file
	map<std::pair<MyDB_TablePtr, long>, MyDB_Page *> lookUpTable; // lookup Table
	LRUCache *cache;											  // cache with lru eviction policy
	size_t anonymousPageNum;									  // current number of anonymous page created so far
};

#endif
