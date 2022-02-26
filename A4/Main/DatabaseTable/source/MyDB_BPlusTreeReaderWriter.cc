
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

	// // find the ordering attribute
	// auto res = forMe->getSchema()->getAttByName(orderOnAttName);

	// // remember information about the ordering attribute
	// orderingAttType = res.second;
	// whichAttIsOrdering = res.first;

	// // and the root location
	// getTable()->setRootLocation(0);
	// MyDB_INRecordPtr lastRootRecord = getINRecord();
	// MyDB_INRecordPtr lastChildRecord = getINRecord();
	// MyDB_PageReaderWriter rootNode = this->operator[](0);
	// MyDB_PageReaderWriter childNode = this->operator[](1);
	// MyDB_INRecordPtr childPtr = getINRecord();
	// childPtr->setPtr(1);
	// rootNode.append(childPtr);
	// rootNode.append(lastRootRecord);
	// childNode.append(lastChildRecord);

	// rootLocation = getTable()->getRootLocation();
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
		MyDB_PageReaderWriter rootNode = this->operator[](rootLocation);
		MyDB_PageReaderWriter childNode = this->operator[](1);
		childNode.append(appendMe);
		// MyDB_RecordIteratorAltPtr nodeIt = rootNode.getIteratorAlt();
		// MyDB_RecordPtr lhs = appendMe;
		// MyDB_RecordPtr rhs = getINRecord();
		// function<bool> compareKey = buildComparator(lhs, rhs);
		// while (true)
		// {
		// 	nodeIt->getCurrent(rhs);
		// 	if (compareKey()) {

		// 	}
		// 	if (!nodeIt->advance())
		// 	{
		// 		break;
		// 	}
		// }
	}
}

MyDB_RecordPtr MyDB_BPlusTreeReaderWriter ::split(MyDB_PageReaderWriter splitMe, MyDB_RecordPtr andMe)
{
	return nullptr;
}

MyDB_RecordPtr MyDB_BPlusTreeReaderWriter ::append(int whichPage, MyDB_RecordPtr appendMe)
{
	return nullptr;
}

MyDB_INRecordPtr MyDB_BPlusTreeReaderWriter ::getINRecord()
{
	return make_shared<MyDB_INRecord>(orderingAttType->createAttMax());
}

void MyDB_BPlusTreeReaderWriter ::printTree()
{
	/*
	empty tree
	*/
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
