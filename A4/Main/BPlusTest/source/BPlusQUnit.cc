
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
		cout << "TEST 0... append helper test" << flush;

		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);

		cout << supplierTable.getTable()->getRootLocation() << endl;

		vector<string> records{
			"1|Supplier#000010001|00000000|999|12-345-678-9012|1234.56|the special record|",
			"2|Supplier#000000001|sdrGnXCDRcfriBvY0KL,ipCanOTyK t NN1|17|27-918-335-1736|5755.94|requests haggle carefully. accounts sublate finally. carefully ironic pa|",
			"3|Supplier#000000002|TRMhVHz3XiFuhapxucPo1|5|15-679-861-2259|4032.68|furiously stealthy frays thrash alongside of the slyly express deposits. blithely regular req|",
			"4|Supplier#000000020|JtPqm19E7tF 152Rl1wQZ8j0H|3|13-715-945-6730|530.82|finally regular asymptotes play furiously among the carefully special warhorses. slyly|",
			"5|Supplier#000000516|tVeEUWbqbWF|5|15-165-647-2301|371.38|slyly dogged requests wake. final id|",
			"6|Supplier#000000997|UM64eAcogoCYh2Q UbWEgRXMQ5icygh4y73|3|13-221-322-7971|3659.56|theodolites nag furiously; carefully regular somas cajo|",
			"7|Supplier#000000998|GKQCo1WXi6QjLXqAVCN8TvagB|15|25-430-605-1180|3282.62|slyly ironic packages about the slyly ironic packages sleep slyl|",
			"8|Supplier#000000021|TZoQwNFFO i,baXpbpin02,hvuhE,GRVIKm |2|12-253-590-5816|9365.80|bravely ironic Tiresias run carefully. regular deposits integrate with the fluffily even f|",
			"9|Supplier#000000022|DGIIamGrSFFUm53Va,0PnOc|4|14-144-830-2814|-966.20|slyly special waters wake fluffily along the unusual package|",
			"10|Supplier#000000003|BZ0kXcHUcHjx62L7CjZSql7gbWQ6RPn5X|1|11-383-516-1199|4192.40|furiously regular instructions impress slyly! carefu|",
			"11|Supplier#000000004|qGTQJXogS83a7MBnEweGHKevK|15|25-843-787-7479|4641.08|final ideas cajole. furiously close dep|",
			"12|Supplier#000000018|ckk5mWb4qewgn1vs|16|26-729-551-1115|7040.82|carefully pending deposits haggle regular the|",
			"13|Supplier#000000217|75 1HUeYEeOThfHQOrywmEaQN,WNZCJN|15|25-113-702-9259|3084.60|furiously final deposits are fluffily s|",
			"14|Supplier#000000515|W7cx14qPRNMTEXpp4Yr7Nje2Z2NLWUoAB7Y1,y|17|27-470-220-5233|2127.89|regular, ironic requests wake ca|",
			"15|Supplier#000000019|NN17XNz0DpmnSFp47kM7G|24|34-278-310-2731|6150.38|quickly regular pinto beans mold blithely slyly pending packages. unusual platelets are furiou|",
			"16|Supplier#000000023| zMyxL7O3O0SUAFoTrY7gVO mnM8XsH|9|19-559-422-5776|5926.41|carefully special requests haggle. furiously busy theodolites haggle b|",
			"17|Supplier#000000024|pWDbwg 5EgcbQPlpH|0|10-620-939-2254|9170.71|quietly even theodolites alongside of the blithely special pin|",
			"18|Supplier#000000218|PTop5LFKme0460SQc6TUlHm VgVm3xS6uQT7|10|20-180-767-8426|8843.88|unusual, bold requests haggle finally final accounts. blithely special pinto beans n|",
			"19|Supplier#000000514|bWm 31dd01Z6nb|14|24-677-367-2786|1669.85|blithely fluffy requests impress blithely even|",
			"20|Supplier#000000025|aoagce3elDACNssVvTLcQl55Up6EYA|22|32-431-945-3541|9198.31|even excuses wake about the carefully special packa|",
			"21|Supplier#000000999|4iqRxcx0Ln27co0jyckg3d3DC8wox6gysED9G|2|12-991-892-1050|3898.69|slyly final dependencies against the furio|",
			"22|Supplier#000001000| MBWkbj 3M|17|27-971-649-2792|7307.62|blithely final instructions |",
			"23|Supplier#000000023| zMyxL7O3O0SUAFoTrY7gVO mnM8XsH|9|19-559-422-5776|5926.41|carefully special requests haggle. furiously busy theodolites haggle b|",
			"24|Supplier#000000024|pWDbwg 5EgcbQPlpH|0|10-620-939-2254|9170.71|quietly even theodolites alongside of the blithely special pin|",
			"25|Supplier#000000025|aoagce3elDACNssVvTLcQl55Up6EYA|22|32-431-945-3541|9198.31|even excuses wake about the carefully special packa|",
			"26|Supplier#000000026|J50ejtquU1SXRxsmdGA,Se,QZsepLjU|21|31-758-894-4436|21.18|slyly unusual pinto beans sleep carefully alongside of the furiously sly frets. regular, regula|",
			"27|Supplier#000000027|GoWoHhvdj6AUfWujBscphWdFjHl7K|18|28-708-999-2028|1887.62|furiously thin packages use |",
			"28|Supplier#000000028|lpJwCaJ0T2jdr5|0|10-538-384-8460|-891.99|regular, regular excuses boost quickl|",
			"29|Supplier#000000029|VVSymB3fbwaNUuBmNAoY|1|11-555-705-5922|-811.62|blithely pending theodolites are carefully ironic requests! fluffily express instructions ab|",
			"30|Supplier#000000030|TWdEo, EaLd,LHXtGCPf8|16|26-940-594-4852|8080.14|express, bold deposits sleep according to th|",
			"31|Supplier#000000031|M9hIErTsOJuqBA9gjOaMDO5Bb|16|26-515-530-4159|5916.91|blithely bold deposits boost fluffily slyly final|",
			"32|Supplier#000000032|uwCnX8y18u,zGadpgSQKHVP1F2fxK|23|33-484-637-7873|3556.47|furiously thin asymptotes are a|",
			"33|Supplier#000000033|LLMgB3vXW,0g,8nuv3qU3QZaEBZvU2qRLX9|7|17-138-897-9374|8564.12|ironic instructions are. special pearls above |",
			"34|Supplier#000000034|F29MXhwqYcXFfWjKungGH b6nceg9o9B03BR|10|20-519-982-2343|237.31|asymptotes are. special, ironic ideas on the close|",
			"35|Supplier#000000035|btEEk4uH55bVcxqqo355 x0WMmrK6,bOU|21|31-720-790-5245|4381.41|slyly bold pinto beans wake. dependencies nag furiously according to the asymptotes. car|",
			"36|Supplier#000000036|Fs8BpqhwPHOuX6g84XPfmMv9nUSnRZGqoSTS|13|23-273-493-3679|2371.51|slyly special braids among the regular, blithe p|",
			"37|Supplier#000000037|PAHtpWJZD5|0|10-470-144-1330|3017.47|ironic requests alongside of the blithely express accounts wake fluffily above the carefully express|",
			"38|Supplier#000000038|vmOuWWwn,l4j7TOSEw4l3|4|14-361-296-6426|2512.41|blithely bold excuses haggle ironic, pending excuses. carefully bold deposits wake blithel|",
			"39|Supplier#000000039|2e0rD92Bmb5 ZtqmjQoR,AHlOJHHM6WMj|8|18-851-856-5633|6115.65|ironic, express instructions can sleep bl|",
			"40|Supplier#000000040|ttiM4tPPBG85XTwFZDE8lpu8KOYgB|22|32-231-247-6991|-290.06|carefully regular accounts sleep ca|"};

		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		for (int i = 0; i < records.size(); i++)
		{
			temp->fromString(records[i]);
			supplierTable.append(temp);
		}

		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();
		int counter = 0;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
		}
		
		bool result = (counter == records.size());
		if (result)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(result);
	}
		FALLTHROUGH_INTENDED;
	case 1:
	{
		cout << "TEST 1... creating tree for small table, on suppkey " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);

		cout << supplierTable.getTable()->getRootLocation() << endl;

		supplierTable.loadFromTextFile("supplier.tbl");

		

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

		int counter = 0;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
		}
		bool result = (counter == 10000);
		if (result)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(result);
	}
		FALLTHROUGH_INTENDED;
	case 2:
	{
		cout << "TEST 2... creating tree for small table, on nationkey " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("nationkey", myTable, myMgr);
		
		cout << supplierTable.getTable()->getRootLocation() << endl;

		supplierTable.loadFromTextFile("supplier.tbl");

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

		int counter = 0;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
		}
		bool result = (counter == 10000);
		if (result)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(result);
	}
		FALLTHROUGH_INTENDED;
	case 3:
	{
		cout << "TEST 3... creating tree for small table, on comment " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
		supplierTable.loadFromTextFile("supplier.tbl");

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

		int counter = 0;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
		}
		bool result = (counter == 10000);
		if (result)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(result);
	}
		FALLTHROUGH_INTENDED;
	case 4:
	{
		cout << "TEST 4... creating tree for large table, on comment " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
		supplierTable.loadFromTextFile("supplierBig.tbl");

		// there should be 320000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

		int counter = 0;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
		}
		bool result = (counter == 320000);
		if (result)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(result);
	}
		FALLTHROUGH_INTENDED;
	case 5:
	{
		cout << "TEST 5... creating tree for large table, on comment asking some queries" << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
		supplierTable.loadFromTextFile("supplierBig.tbl");

		// there should be 320000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

		MyDB_StringAttValPtr low = make_shared<MyDB_StringAttVal>();
		low->set("slyly ironic");
		MyDB_StringAttValPtr high = make_shared<MyDB_StringAttVal>();
		high->set("slyly ironic~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

		myIter = supplierTable.getRangeIteratorAlt(low, high);
		int counter = 0;
		bool foundIt = false;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
			if (temp->getAtt(0)->toInt() == 4171)
				foundIt = true;
		}
		bool result = foundIt && (counter = 4192);
		if (result)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(result);
	}
		FALLTHROUGH_INTENDED;
	case 6:
	{
		cout << "TEST 6... creating tree for small table, on suppkey, checking for sorted order " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
		supplierTable.loadFromTextFile("supplier.tbl");

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

		int counter = 0;
		MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
		low->set(1);
		MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
		high->set(10000);

		MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
		bool res = true;
		while (myIter->advance())
		{
			myIter->getCurrent(temp);
			counter++;
			if (counter != temp->getAtt(0)->toInt())
			{
				res = false;
				cout << "Found key of " << temp->getAtt(0)->toInt() << ", expected " << counter << "\n";
			}
		}
		if (res && (counter == 10000))
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(res && (counter == 10000));
	}
		FALLTHROUGH_INTENDED;
	case 7:
	{
		cout << "TEST 7... creating tree for small table, on suppkey, running point queries " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
		supplierTable.loadFromTextFile("supplier.tbl");

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

		int counter = 0;
		bool res = true;
		for (int i = 1; i < 101; i++)
		{
			MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
			low->set(i * 19);
			MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
			high->set(i * 19);

			MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
			while (myIter->advance())
			{
				myIter->getCurrent(temp);
				counter++;
				if (i * 19 != temp->getAtt(0)->toInt())
				{
					res = false;
					cout << "Found key of " << temp->getAtt(0)->toInt() << ", expected " << i * 19 << "\n";
				}
			}
		}
		if (res && (counter == 100))
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(res && (counter == 100));
	}
		FALLTHROUGH_INTENDED;
	case 8:
	{
		cout << "TEST 8... creating tree for small table, on comment, running point queries with no answer " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("comment", myTable, myMgr);
		supplierTable.loadFromTextFile("supplier.tbl");

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

		int counter = 0;
		for (int i = 0; i < 26; i++)
		{
			MyDB_StringAttValPtr low = make_shared<MyDB_StringAttVal>();
			char a = 'a' + i;
			low->set(string(&a));
			MyDB_StringAttValPtr high = make_shared<MyDB_StringAttVal>();
			high->set(string(&a));

			MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
			while (myIter->advance())
			{
				myIter->getCurrent(temp);
				counter++;
			}
		}
		if (counter == 0)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(counter == 0);
	}
		FALLTHROUGH_INTENDED;
	case 9:
	{
		cout << "TEST 9... creating tree for small table, on suppkey, running point queries under and over range " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);
		supplierTable.loadFromTextFile("supplier.tbl");

		// there should be 10000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

		int counter = 0;
		for (int i = -1; i <= 10001; i += 10002)
		{
			MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
			low->set(i);
			MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
			high->set(i);

			MyDB_RecordIteratorAltPtr myIter = supplierTable.getSortedRangeIteratorAlt(low, high);
			while (myIter->advance())
			{
				myIter->getCurrent(temp);
				counter++;
			}
		}
		if (counter == 0)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(counter == 0);
	}
		FALLTHROUGH_INTENDED;
	case 10:
	{
		cout << "TEST 10... mega test using tons of range queries " << flush;
		MyDB_BufferManagerPtr myMgr = make_shared<MyDB_BufferManager>(1024 * 128, 128, "tempFile");
		MyDB_BPlusTreeReaderWriter supplierTable("suppkey", myTable, myMgr);

		// load it from a text file
		supplierTable.loadFromTextFile("supplierBig.tbl");

		// there should be 320000 records
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord();
		MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt();

		// now, we check 100 different random suppliers queries
		bool allOK = true;
		for (int time = 0; time < 2; time++)
		{
			for (int i = 0; i < 100; i++)
			{

				// we are looping through twice; the first time, ask only point queries
				srand48(i);
				int lowBound = lrand48() % 10000;
				int highBound = lrand48() % 10000;
				if (time % 2 == 0)
					highBound = lowBound;

				// make sure the low bound is less than the high bound
				if (lowBound > highBound)
				{
					int temp = lowBound;
					lowBound = highBound;
					highBound = temp;
				}

				// ask a range query
				MyDB_IntAttValPtr low = make_shared<MyDB_IntAttVal>();
				low->set(lowBound);
				MyDB_IntAttValPtr high = make_shared<MyDB_IntAttVal>();
				high->set(highBound);

				if (i % 2 == 0)
					myIter = supplierTable.getRangeIteratorAlt(low, high);
				else
					myIter = supplierTable.getSortedRangeIteratorAlt(low, high);

				// verify we got exactly the correct count back
				int counter = 0;
				while (myIter->advance())
				{
					myIter->getCurrent(temp);
					counter++;
				}

				if (counter != 32 * (highBound - lowBound + 1))
					allOK = false;
			}
		}
		if (allOK)
			cout << "\tTEST PASSED\n";
		else
			cout << "\tTEST FAILED\n";
		QUNIT_IS_TRUE(allOK);
	}
	}
}

#endif
