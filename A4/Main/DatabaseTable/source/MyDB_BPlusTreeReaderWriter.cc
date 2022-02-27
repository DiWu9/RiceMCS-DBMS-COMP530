
#ifndef BPLUS_C
#define BPLUS_C

#include "MyDB_INRecord.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageListIteratorSelfSortingAlt.h"
#include "RecordComparator.h"

#include <string>

MyDB_BPlusTreeReaderWriter ::MyDB_BPlusTreeReaderWriter(string orderOnAttName, MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) : MyDB_TableReaderWriter(forMe, myBuffer)
{
	// find the ordering attribute
	auto res = forMe->getSchema()->getAttByName(orderOnAttName);

	// remember information about the ordering attribute
	orderingAttType = res.second;
	whichAttIsOrdering = res.first;

	// and the root location
	rootLocation = getTable()->getRootLocation();
	
}

MyDB_RecordIteratorAltPtr MyDB_BPlusTreeReaderWriter ::getSortedRangeIteratorAlt(MyDB_AttValPtr low, MyDB_AttValPtr high)
{
	return nullptr;
}

MyDB_RecordIteratorAltPtr MyDB_BPlusTreeReaderWriter ::getRangeIteratorAlt(MyDB_AttValPtr low, MyDB_AttValPtr high)
{
	return nullptr;
}

bool MyDB_BPlusTreeReaderWriter ::discoverPages(int whichPage, vector<MyDB_PageReaderWriter> &list, MyDB_AttValPtr low, MyDB_AttValPtr high)
{
	return false;
}

void MyDB_BPlusTreeReaderWriter ::append(MyDB_RecordPtr appendMe)
{
	// INRecord: key(value), ptr(id of child)
	// methods: int getPtr (), setPtr (int fromMe), setKey (MyDB_AttValPtr toMe), getKey ()
	if (rootLocation == -1)
	{
		getTable()->setRootLocation(0);
		rootLocation = getTable()->getRootLocation();
		MyDB_PageReaderWriter rootNode = this->operator[](0);
		MyDB_PageReaderWriter childNode = this->operator[](1);
		rootNode.setType(MyDB_PageType::DirectoryPage);
		childNode.setType(MyDB_PageType::RegularPage);
		MyDB_INRecordPtr childPtr = getINRecord();
		childPtr->setPtr(1);
		rootNode.append(childPtr);
		childNode.append(appendMe);
	}
	else
	{
		MyDB_RecordPtr lptr = append(rootLocation, appendMe);
		// if root split
		if (lptr != nullptr)
		{
			MyDB_INRecordPtr rptr = getINRecord();
			rptr->setPtr(rootLocation);

			// remove the first record of the old root
			vector<MyDB_RecordPtr> oldRootRecords;
			MyDB_PageReaderWriter oldRoot = this->operator[](rootLocation);
			MyDB_RecordIteratorAltPtr itOldRoot = oldRoot.getIteratorAlt();
			while (itOldRoot->advance())
			{
				MyDB_INRecordPtr inRecord = getINRecord();
				itOldRoot->getCurrent(inRecord);
				oldRootRecords.push_back(inRecord);
			}
			oldRoot.clear();
			oldRoot.setType(MyDB_PageType::DirectoryPage);
			for (int i = 1; i < oldRootRecords.size(); i++)
			{
				oldRoot.append(oldRootRecords[i]);
			}

			MyDB_PageReaderWriter newRoot = this->operator[](getTable()->lastPage() + 1);
			getTable()->setRootLocation(getTable()->lastPage());
			rootLocation = getTable()->getRootLocation();
			newRoot.setType(MyDB_PageType::DirectoryPage);
			newRoot.append(lptr);
			newRoot.append(rptr);
		}
	}
	// cout << "Root location: " << rootLocation << endl;
	// if (this->operator[](rootLocation).getType() == MyDB_PageType::DirectoryPage) {
	// 	cout << "Root page is directory" << endl;
	// }
	// else {
	// 	cout << "Root page is regular" << endl;
	// }
	// printTree();
}

/**
 * @brief This method accepts a page to split, and one more record to add onto the end of the
page (which cannot fit). It then splits the contents of the page, incorporating the new
record, leaving all of the records with big key values in place, and creating a new page
with all of the small key values at the end of the file. A smart pointer to an internal node
record whose key value and pointer is produced by this method as a return value (so
that it can be inserted into the parent of the splitting node). This internal node record
has been set up to point to the new page.
 */
MyDB_RecordPtr MyDB_BPlusTreeReaderWriter ::split(MyDB_PageReaderWriter splitMe, MyDB_RecordPtr andMe)
{

	// append and sort phase
	MyDB_RecordPtr lhs, rhs;
	bool isDirectoryPage = splitMe.getType() == MyDB_PageType::DirectoryPage;
	if (isDirectoryPage)
	{
		lhs = getINRecord();
		rhs = getINRecord();
	}
	else
	{
		lhs = getEmptyRecord();
		rhs = getEmptyRecord();
	}
	splitMe.sortInPlace(buildComparator(lhs, rhs), lhs, rhs);

	vector<MyDB_RecordPtr> recordPtrs;
	bool andMeNotAdded = true;
	MyDB_RecordIteratorAltPtr it = splitMe.getIteratorAlt();
	function<bool()> recordCompare;
	while (true)
	{
		MyDB_RecordPtr recPtr;
		if (isDirectoryPage)
		{
			recPtr = getINRecord();
		}
		else
		{
			recPtr = getEmptyRecord();
		}
		it->getCurrent(recPtr);
		if (andMeNotAdded)
		{
			recordCompare = buildComparator(andMe, recPtr);
			if (recordCompare())
			{
				recordPtrs.push_back(andMe);
				andMeNotAdded = false;
			}
			recordPtrs.push_back(recPtr);
		}
		if (!it->advance())
		{
			break;
		}
	}
	// if andMe is the biggest record and not added during the process
	if (andMeNotAdded) {
		recordPtrs.push_back(andMe);
	}

	// find median and split phase
	int n = recordPtrs.size();
	int median = n / 2;
	MyDB_PageReaderWriter newPage = this->operator[](getTable()->lastPage() + 1);
	splitMe.clear();
	if (isDirectoryPage)
	{
		splitMe.setType(MyDB_PageType::DirectoryPage);
		newPage.setType(MyDB_PageType::DirectoryPage);
	}
	for (int i1 = 0; i1 < median; i1++)
	{
		newPage.append(recordPtrs[i1]);
	}
	for (int i2 = median; i2 < n; i2++)
	{
		splitMe.append(recordPtrs[i2]);
	}
	MyDB_INRecordPtr toReturn = getINRecord();
	toReturn->setPtr(getTable()->lastPage());
	toReturn->setKey(getKey(recordPtrs[median]));

	return toReturn;
}

/**
 * @brief append () accepts the identity of a page whichPage in the file corresponding to the
B+-Tree, as well as a record to append, and then it recursively finds the appropriate leaf
node for the record and appends the record to that node. This is all done using the
classical B+-Tree insertion algorithm. (In this sense, “append” is something of a
misnomer, because we are actually doing a classic B+-Tree insert; I used the name
“append” to be consistent with the regular file reader/writer). If the page whichPage
splits due to the insertion (a split can occur at an internal node level or at a leaf node
level) then the append () method returns a new MyDB_INRecordPtr object that points
to an appropriate internal node record. As described subsequently, internal node
records are special records that live only in the internal nodes (pages) in the B+-Tree
and they are different because they have only a key and a pointer, and no data. All leaf
nodes in our B+-Tree (those whose type is MyDB_PageType :: RegularPage) will have
only “regular” records. All others (directory or internal node pages) will hold these
special internal node records.
 */
MyDB_RecordPtr MyDB_BPlusTreeReaderWriter ::append(int whichPage, MyDB_RecordPtr appendMe)
{
	MyDB_PageReaderWriter appendTo = this->operator[](whichPage);
	// if internal page
	if (appendTo.getType() == MyDB_PageType::DirectoryPage)
	{
		MyDB_RecordIteratorAltPtr nodeIt = appendTo.getIteratorAlt();
		MyDB_INRecordPtr rhs = getINRecord();
		function<bool()> compareKey = buildComparator(appendMe, rhs);
		while (true)
		{
			nodeIt->getCurrent(rhs);
			// find the corresponding inRecord
			if (compareKey())
			{
				MyDB_RecordPtr newSplitPage = append(rhs->getPtr(), appendMe);

				if (newSplitPage == nullptr)
				{
					return nullptr;
				}
				else
				{
					bool appendSuccess = appendTo.append(newSplitPage);
					if (appendSuccess)
					{
						MyDB_RecordPtr sortlhs = getINRecord();
						MyDB_RecordPtr sortrhs = getINRecord();
						appendTo.sortInPlace(buildComparator(sortlhs, sortrhs), sortlhs, sortrhs);
						return nullptr;
					}
					else
					{
						return split(appendTo, newSplitPage);
					}
				}
			}
			nodeIt->advance();
		}
	}
	// if leaf page, just append it to end
	else
	{
		bool appendSuccess = appendTo.append(appendMe);
		if (appendSuccess)
		{
			return nullptr;
		}
		else
		{
			return split(appendTo, appendMe);
		}
	}
}

MyDB_INRecordPtr MyDB_BPlusTreeReaderWriter ::getINRecord()
{
	return make_shared<MyDB_INRecord>(orderingAttType->createAttMax());
}

void MyDB_BPlusTreeReaderWriter ::printTree()
{
	if (rootLocation == -1)
	{
		cout << "\nPrint tree: empty tree." << endl;
	}
	else
	{
		string s = "\nPrint tree: \n";
		string sLayer;
		string sPage;
		vector<MyDB_PageReaderWriter> layer;
		layer.push_back(this->operator[](rootLocation));
		while (layer.size() != 0)
		{
			sLayer = "";
			vector<MyDB_PageReaderWriter> nextLayer;
			for (int i = 0; i < layer.size(); i++)
			{
				MyDB_PageReaderWriter currPage = layer[i];
				sPage = "";
				MyDB_RecordIteratorAltPtr currPageIt = currPage.getIteratorAlt();
				if (currPage.getType() == MyDB_PageType::DirectoryPage)
				{
					sPage = sPage + "|IN|";
					MyDB_INRecordPtr inRecord = getINRecord();
					while (true)
					{
						currPageIt->getCurrent(inRecord);
						int childPtr = inRecord->getPtr();
						nextLayer.push_back(this->operator[](childPtr));
						sPage = sPage + "(" + inRecord->getKey()->toString() + ", " + std::to_string(childPtr) + ")|";
						if (!currPageIt->advance())
						{
							break;
						}
					}
				}
				else
				{
					sPage = sPage + "|LF|";
					MyDB_RecordPtr normalRecord = getEmptyRecord();
					while (true)
					{
						currPageIt->getCurrent(normalRecord);
						sPage = sPage + "(" + getKey(normalRecord)->toString() + ")|";
						if (!currPageIt->advance())
						{
							break;
						}
					}
				}
				sLayer = sLayer + sPage;
			}
			s = s + sLayer + "\n";
			layer = nextLayer;
		}
		cout << s << flush;
	}
}

/**
 * @brief obtains the key attribute for an actual data record (that is, it obtains the value that the record is sorted on).
 */
MyDB_AttValPtr MyDB_BPlusTreeReaderWriter ::getKey(MyDB_RecordPtr fromMe)
{

	// in this case, got an IN record
	if (fromMe->getSchema() == nullptr)
		return fromMe->getAtt(0)->getCopy();

	// in this case, got a data record
	else
		return fromMe->getAtt(whichAttIsOrdering)->getCopy();
}

/**
 * @brief build a comparator (a lambda) for two records. Both of the records must either be MyDB_INRecord object for this tree,
 * or they must be data records for this tree, or some combination. The constructed lambda, when invoked, will return true iff
 * the key of the first record is less than the key of the second record.
 */
function<bool()> MyDB_BPlusTreeReaderWriter ::buildComparator(MyDB_RecordPtr lhs, MyDB_RecordPtr rhs)
{

	MyDB_AttValPtr lhAtt, rhAtt;

	// in this case, the LHS is an IN record
	if (lhs->getSchema() == nullptr)
	{
		lhAtt = lhs->getAtt(0);

		// here, it is a regular data record
	}
	else
	{
		lhAtt = lhs->getAtt(whichAttIsOrdering);
	}

	// in this case, the LHS is an IN record
	if (rhs->getSchema() == nullptr)
	{
		rhAtt = rhs->getAtt(0);

		// here, it is a regular data record
	}
	else
	{
		rhAtt = rhs->getAtt(whichAttIsOrdering);
	}

	// now, build the comparison lambda and return
	if (orderingAttType->promotableToInt())
	{
		return [lhAtt, rhAtt]
		{ return lhAtt->toInt() < rhAtt->toInt(); };
	}
	else if (orderingAttType->promotableToDouble())
	{
		return [lhAtt, rhAtt]
		{ return lhAtt->toDouble() < rhAtt->toDouble(); };
	}
	else if (orderingAttType->promotableToString())
	{
		return [lhAtt, rhAtt]
		{ return lhAtt->toString() < rhAtt->toString(); };
	}
	else
	{
		cout << "This is bad... cannot do anything with the >.\n";
		exit(1);
	}
}

#endif
