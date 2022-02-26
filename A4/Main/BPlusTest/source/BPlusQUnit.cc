
#ifndef BPLUS_TEST_H
#define BPLUS_TEST_H

#include "MyDB_AttType.h"
#include "MyDB_BufferManager.h"
#include "MyDB_Catalog.h"
#include "MyDB_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Record.h"
#include "MyDB_Table.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include "MyDB_Schema.h"
#include "QUnit.h"
#include "Sorting.h"
#include <iostream>

#define FALLTHROUGH_INTENDED \
	do                       \
	{                        \
	} while (0)

int main(int argc, char *argv[])
{

	int start = 0;
	if (argc > 1 && argv[1][0] >= '0' && argv[1][0] <= '9')
	{
		start = argv[1][0] - '0';
	}

	QUnit::UnitTest qunit(cerr, QUnit::normal);

	// create a catalog
	MyDB_CatalogPtr myCatalog = make_shared<MyDB_Catalog>("catFile");

	// now make a schema
	MyDB_SchemaPtr mySchema = make_shared<MyDB_Schema>();
	mySchema->appendAtt(make_pair("suppkey", make_shared<MyDB_IntAttType>()));
	mySchema->appendAtt(make_pair("name", make_shared<MyDB_StringAttType>()));
	mySchema->appendAtt(make_pair("address", make_shared<MyDB_StringAttType>()));
	mySchema->appendAtt(make_pair("nationkey", make_shared<MyDB_IntAttType>()));
	mySchema->appendAtt(make_pair("phone", make_shared<MyDB_StringAttType>()));
	mySchema->appendAtt(make_pair("acctbal", make_shared<MyDB_DoubleAttType>()));
	mySchema->appendAtt(make_pair("comment", make_shared<MyDB_StringAttType>()));

	// use the schema to create a table
	MyDB_TablePtr myTable = make_shared<MyDB_Table>("supplier", "supplier.bin", mySchema);

	cout << "Using small page size.\n";

	switch (start)
	{
	case 0:
	{
		cout << "TEST 0... test single append " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);

		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		string s = "10001|Supplier#000010001|00000000|999|12-345-678-9012|1234.56|the special record|";
		temp->fromString(s);
		supplierTable.append(temp);
		supplierTable.printTree();

		temp = supplierTable.getEmptyRecord();
		s = "1|Supplier#000000001|sdrGnXCDRcfriBvY0KL,ipCanOTyK t NN1|17|27-918-335-1736|5755.94|requests haggle carefully. accounts sublate finally. carefully ironic pa|";
		temp->fromString(s);
		supplierTable.append(temp);
		supplierTable.printTree();

		temp = supplierTable.getEmptyRecord();
		s = "2|Supplier#000000002|TRMhVHz3XiFuhapxucPo1|5|15-679-861-2259|4032.68|furiously stealthy frays thrash alongside of the slyly express deposits. blithely regular req|";
		temp->fromString(s);
		supplierTable.append(temp);
		supplierTable.printTree();

		//3|Supplier#000000003|BZ0kXcHUcHjx62L7CjZSql7gbWQ6RPn5X|1|11-383-516-1199|4192.40|furiously regular instructions impress slyly! carefu|
		//4|Supplier#000000004|qGTQJXogS83a7MBnEweGHKevK|15|25-843-787-7479|4641.08|final ideas cajole. furiously close dep|
	}
		FALLTHROUGH_INTENDED;
	// case 1:
	// {
	// 	cout << "TEST 1... creating tree for small table, on suppkey " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

	// 	int counter = 0;
	// 	while (myIter->advance())
	// 	{
	// 		myIter->getCurrent(temp);
	// 		counter++;
	// 	}
	// 	bool result = (counter == 10000);
	// 	if (result)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(result);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 2:
	// {
	// 	cout << "TEST 2... creating tree for small table, on nationkey " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("nationkey", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

	// 	int counter = 0;
	// 	while (myIter->advance())
	// 	{
	// 		myIter->getCurrent(temp);
	// 		counter++;
	// 	}
	// 	bool result = (counter == 10000);
	// 	if (result)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(result);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 3:
	// {
	// 	cout << "TEST 3... creating tree for small table, on comment " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

	// 	int counter = 0;
	// 	while (myIter->advance())
	// 	{
	// 		myIter->getCurrent(temp);
	// 		counter++;
	// 	}
	// 	bool result = (counter == 10000);
	// 	if (result)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(result);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 4:
	// {
	// 	cout << "TEST 4... creating tree for large table, on comment " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplierBig.tbl");

	// 	// there should be 320000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

	// 	int counter = 0;
	// 	while (myIter->advance())
	// 	{
	// 		myIter->getCurrent(temp);
	// 		counter++;
	// 	}
	// 	bool result = (counter == 320000);
	// 	if (result)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(result);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 5:
	// {
	// 	cout << "TEST 5... creating tree for large table, on comment asking some queries" << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplierBig.tbl");

	// 	// there should be 320000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

	// 	MyDB_StringAttValPtr low = make_shared<MyDB_StringAttVal>();
	// 	low->set("slyly ironic");
	// 	MyDB_StringAttValPtr high = make_shared<MyDB_StringAttVal>();
	// 	high->set("slyly ironic~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	// 	myIter = supplierTable.getRangeIteratorAlt(low, high);
	// 	int counter = 0;
	// 	bool foundIt = false;
	// 	while (myIter->advance())
	// 	{
	// 		myIter->getCurrent(temp);
	// 		counter++;
	// 		if (temp->getAtt(0)->toInt() == 4171)
	// 			foundIt = true;
	// 	}
	// 	bool result = foundIt && (counter = 4192);
	// 	if (result)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(result);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 6:
	// {
	// 	cout << "TEST 6... creating tree for small table, on suppkey, checking for sorted order " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

	// 	int counter = 0;
	// 	MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
	// 	low->set(1);
	// 	MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
	// 	high->set(10000);

	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
	// 	bool res = true;
	// 	while (myIter->advance())
	// 	{
	// 		myIter->getCurrent(temp);
	// 		counter++;
	// 		if (counter != temp->getAtt(0)->toInt())
	// 		{
	// 			res = false;
	// 			cout << "Found key of " << temp->getAtt(0)->toInt() << ", expected " << counter << "\n";
	// 		}
	// 	}
	// 	if (res && (counter == 10000))
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(res && (counter == 10000));
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 7:
	// {
	// 	cout << "TEST 7... creating tree for small table, on suppkey, running point queries " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

	// 	int counter = 0;
	// 	bool res = true;
	// 	for (int i = 1; i < 101; i++)
	// 	{
	// 		MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
	// 		low->set(i * 19);
	// 		MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
	// 		high->set(i * 19);

	// 		MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
	// 		while (myIter->advance())
	// 		{
	// 			myIter->getCurrent(temp);
	// 			counter++;
	// 			if (i * 19 != temp->getAtt(0)->toInt())
	// 			{
	// 				res = false;
	// 				cout << "Found key of " << temp->getAtt(0)->toInt() << ", expected " << i * 19 << "\n";
	// 			}
	// 		}
	// 	}
	// 	if (res && (counter == 100))
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(res && (counter == 100));
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 8:
	// {
	// 	cout << "TEST 8... creating tree for small table, on comment, running point queries with no answer " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

	// 	int counter = 0;
	// 	for (int i = 0; i < 26; i++)
	// 	{
	// 		MyDB_StringAttValPtr low = make_shared<MyDB_StringAttVal>();
	// 		char a = 'a' + i;
	// 		low->set(string(&a));
	// 		MyDB_StringAttValPtr high = make_shared<MyDB_StringAttVal>();
	// 		high->set(string(&a));

	// 		MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
	// 		while (myIter->advance())
	// 		{
	// 			myIter->getCurrent(temp);
	// 			counter++;
	// 		}
	// 	}
	// 	if (counter == 0)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(counter == 0);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 9:
	// {
	// 	cout << "TEST 9... creating tree for small table, on suppkey, running point queries under and over range " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
	// 	supplierTable.loadFromTextFile("supplier.tbl");

	// 	// there should be 10000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

	// 	int counter = 0;
	// 	for (int i = -1; i <= 10001; i += 10002)
	// 	{
	// 		MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
	// 		low->set(i);
	// 		MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
	// 		high->set(i);

	// 		MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
	// 		while (myIter->advance())
	// 		{
	// 			myIter->getCurrent(temp);
	// 			counter++;
	// 		}
	// 	}
	// 	if (counter == 0)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(counter == 0);
	// }
	// 	FALLTHROUGH_INTENDED;
	// case 10:
	// {
	// 	cout << "TEST 10... mega test using tons of range queries " << flush;
	// 	MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024 * 128, 128, "tempFile");
	// 	MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);

	// 	// load it from a text file
	// 	supplierTable.loadFromTextFile("supplierBig.tbl");

	// 	// there should be 320000 records
	// 	MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
	// 	MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

	// 	// now, we check 100 different random suppliers queries
	// 	bool allOK = true;
	// 	for (int time = 0; time < 2; time++)
	// 	{
	// 		for (int i = 0; i < 100; i++)
	// 		{

	// 			// we are looping through twice; the first time, ask only point queries
	// 			srand48(i);
	// 			int lowBound = lrand48() % 10000;
	// 			int highBound = lrand48() % 10000;
	// 			if (time % 2 == 0)
	// 				highBound = lowBound;

	// 			// make sure the low bound is less than the high bound
	// 			if (lowBound > highBound)
	// 			{
	// 				int temp = lowBound;
	// 				lowBound = highBound;
	// 				highBound = temp;
	// 			}

	// 			// ask a range query
	// 			MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
	// 			low->set(lowBound);
	// 			MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
	// 			high->set(highBound);

	// 			if (i % 2 == 0)
	// 				myIter = supplierTable.getRangeIteratorAlt(low, high);
	// 			else
	// 				myIter = supplierTable.getSortedRangeIteratorAlt(low, high);

	// 			// verify we got exactly the correct count back
	// 			int counter = 0;
	// 			while (myIter->advance())
	// 			{
	// 				myIter->getCurrent(temp);
	// 				counter++;
	// 			}

	// 			if (counter != 32 * (highBound - lowBound + 1))
	// 				allOK = false;
	// 		}
	// 	}
	// 	if (allOK)
	// 		cout << "\tTEST PASSED\n";
	// 	else
	// 		cout << "\tTEST FAILED\n";
	// 	QUNIT_IS_TRUE(allOK);
	// }
	}
}

#endif
