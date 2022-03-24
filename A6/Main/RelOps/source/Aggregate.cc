
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

    MyDB_RecordPtr recIn = input->getEmptyRecord();
    MyDB_RecordPtr recOut = output->getEmptyRecord();

    func selectionFunc = recIn->compileComputation(selectionPredicate);

}

#endif

