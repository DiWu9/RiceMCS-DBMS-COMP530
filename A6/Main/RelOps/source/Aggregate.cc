
#ifndef AGG_CC
#define AGG_CC

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "Aggregate.h"
#include <unordered_map>

using namespace std;

Aggregate :: Aggregate (MyDB_TableReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
		vector <pair <MyDB_AggType, string>> aggsToComputeIn,
		vector <string> groupingsIn, string selectionPredicateIn) {
    
    input = inputIn;
    output = outputIn;
    aggsToCompute = aggsToComputeIn;
    groupings = groupingsIn;
    selectionPredicate = selectionPredicateIn;

}

void Aggregate :: run () {

    int i;
    int numGroupClause = groupings.size();

    // make sure the number of attributes is OK
    vector<pair<string, MyDB_AttTypePtr>> attributesOut = output->getTable()->getSchema()->getAtts();
    if (attributesOut.size() != aggsToCompute.size() + numGroupClause) {
        cout << "error, the output schema doesn't match the aggregations and groupings. \n";
        return;
    }

    // create a schema to store aggregation records
    // 1. adding grouping attributes
    MyDB_SchemaPtr aggSchema = make_shared<MyDB_Schema>();
    for (i = 0; i < numGroupClause; i++) {
        pair<string, MyDB_AttTypePtr> attr = attributesOut[i];
        aggSchema->appendAtt(make_pair("MyDB_GroupAtt"+to_string(i), attr.second));
    }
    // 2. adding aggregation attributes
    for (int j = 0; j < aggsToCompute.size(); j++) {
        pair<string, MyDB_AttTypePtr> attr = attributesOut[numGroupClause+j];
        aggSchema->appendAtt (make_pair("MyDB_AggAtt"+to_string(j), attr.second));
    }
    // 3. adding count for AVG computation
    aggSchema->appendAtt(make_pair("MyDB_CntAtt", make_shared<MyDB_IntAttType>()));

    // create combined schama where its attributes contains inputSchema + aggSchema
    MyDB_SchemaPtr combinedSchema = make_shared<MyDB_Schema>();
    for (auto &a : input->getTable()->getSchema()->getAtts()) {
        combinedSchema->appendAtt(a);
    }
    for (auto &a : aggSchema->getAtts()) {
        combinedSchema->appendAtt(a);
    }

    // create records for input, agg, and combined
    MyDB_RecordPtr recIn = input->getEmptyRecord();
    MyDB_RecordPtr recAgg = make_shared<MyDB_Record>(aggSchema);
    MyDB_RecordPtr recCombined = make_shared<MyDB_Record>(combinedSchema);
    recCombined->buildFrom(recIn, recAgg);

    // agg update functions
    vector<func> aggComps;
    vector<func> finalAggregates;
    i = 0;
    for (auto &a : aggsToCompute) {
        if (a.first == MyDB_AggType :: sum) {
            finalAggregates.push_back (recCombined->compileComputation ("[MyDB_AggAtt" + to_string(i) + "]"));
            aggComps.push_back(recCombined->compileComputation("+ (" + a.second + ", [MyDB_AggAtt" + to_string(i) + "])"));
        }
        else if (a.first == MyDB_AggType :: avg) {
            finalAggregates.push_back (recCombined->compileComputation ("/ ([MyDB_AggAtt" + to_string(i) + "], [MyDB_CntAtt])"));
            aggComps.push_back(recCombined->compileComputation("+ (" + a.second + ", [MyDB_AggAtt" + to_string(i) + "])"));
        }
        else if (a.first == MyDB_AggType :: cnt) {
            finalAggregates.push_back (recCombined->compileComputation ("[MyDB_AggAtt" + to_string(i) + "]"));
            aggComps.push_back(recCombined->compileComputation("+ (int[1], [MyDB_AggAtt" + to_string(i) + "])"));
        }
        i++;
    }
    aggComps.push_back(recCombined->compileComputation ("+ (int[1], [MyDB_CntAtt])"));

    // group by clause computation
    vector<func> groupComps;
    for (auto &a : groupings) {
		groupComps.push_back (recIn->compileComputation(a));
	}

    // group by matching check
    string groupMatch = "bool[true]";
    i = 0;
    for (auto &a : groupings) {
        string curr = "== (" + a + ", [MyDB_GroupAtt" + to_string(i) + "])";
        if (i == 0) {
            groupMatch = curr;
        }
        else {
            groupMatch = "&& (" + groupMatch + ", " + curr + ")";
        }
        i++;
    }
    func isGroupMatch = recCombined->compileComputation(groupMatch);

    // WHERE predicate
    func selectionFunc = recIn->compileComputation(selectionPredicate);

    // initialize rec's hash map
	unordered_map <size_t, vector<void *>> recMap;

	MyDB_PageReaderWriter currPage(true, *(input->getBufferMgr()));
    vector<MyDB_PageReaderWriter> allPages;
    allPages.push_back(currPage);

    // start iterating input table records
    MyDB_RecordIteratorPtr inputIterator = input->getIterator(recIn);

    while (inputIterator->hasNext()) {

        inputIterator->getNext();

        // see if it is accepted by the WHERE predicate
        if (!selectionFunc()->toBool()) {
            continue;
        }

		// compute rec's hash
		size_t hashVal = 0;
		for (auto &f : groupComps) {
			hashVal ^= f()->hash();
		}

        void* matchedRec = nullptr;
        for (auto &r : recMap[hashVal]) {	

			recAgg->fromBinary(r);

            // if group by clauses are the same
			if (isGroupMatch()->toBool()) {
                matchedRec = r;
			    break;
			}
		}

        // if the first occurrence of the group by values, add it to map
        if (matchedRec == nullptr) {
			i = 0;
			for (auto &f : groupComps) {
				recAgg->getAtt(i)->set(f());
                i++;
			}
            int j;
            MyDB_AttValPtr zero = make_shared <MyDB_DoubleAttVal>();
            for (j = i; j <= i+aggsToCompute.size(); j++) {
                recAgg->getAtt(j)->set(zero);
            }
        }

        // update aggregate clauses
        i = 0;
        for (auto &f : aggComps) {
            recAgg->getAtt(numGroupClause+i)->set(f());
            i++;
        }

        recAgg->recordContentHasChanged ();
        if (matchedRec != nullptr) {
            // overwrite the old record
            recAgg->toBinary(matchedRec);
        }
        else {
            matchedRec = currPage.appendAndReturnLocation(recAgg);
            // currPage is full
            if (matchedRec == nullptr) {
                MyDB_PageReaderWriter nextPage(true, *(input->getBufferMgr()));
				currPage = nextPage;
				allPages.push_back(currPage);
				matchedRec = currPage.appendAndReturnLocation(recAgg);	
            }
            // recAgg->fromBinary(matchedRec);
			recMap[hashVal].push_back(matchedRec);
        }

    }

    // aggregate the aggregated records to form the final result
	MyDB_RecordIteratorAltPtr finalIterator = getIteratorAlt(allPages);	
	MyDB_RecordPtr outRec = output->getEmptyRecord();

	while (finalIterator->advance()) {
		finalIterator->getCurrent(recAgg);

        // update attributes
		for (i = 0; i < numGroupClause; i++) {
			outRec->getAtt(i)->set(recAgg->getAtt(i));
		}
		for (auto &f : finalAggregates) {
			outRec->getAtt(i)->set(f());
            i++;
		}

		outRec->recordContentHasChanged();
		output->append(outRec);
	}

}

#endif

