
#ifndef BPLUS_SELECTION_C                                        
#define BPLUS_SELECTION_C

#include "BPlusSelection.h"

BPlusSelection :: BPlusSelection (MyDB_BPlusTreeReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
		MyDB_AttValPtr lowIn, MyDB_AttValPtr highIn,
		string selectionPredicateIn, vector <string> projectionsIn) {
    
    input = inputIn;
	output = outputIn;
	low = lowIn;
	high = highIn;
	selectionPredicate = selectionPredicateIn;
	projections = projectionsIn;

}

void BPlusSelection :: run () {

    // construct two empty records for read table and write table
    MyDB_RecordPtr recIn = input->getEmptyRecord();
    MyDB_RecordPtr recOut = output->getEmptyRecord();

    // select predicate function
    func selectFunc = recIn->compileComputation(selectionPredicate);

    // projection functions
    vector <func> finalComputations;
    for (string s : projections) {
        finalComputations.push_back(recIn->compileComputation(s));
    }

    // iterate within the range from low to high
    MyDB_RecordIteratorAltPtr rangeIterator = input->getSortedRangeIteratorAlt(low, high);

    while (rangeIterator->advance()) {

        rangeIterator->getCurrent(recIn);

        // if accepted by selection predicate
        if (selectFunc()->toBool()) {
		    int i = 0;
		    for (auto &f : finalComputations) {
			    recOut->getAtt(i)->set(f());
                i++;
		    }

            recOut->recordContentHasChanged();
            output->append(recOut);
        }
    }
    
}

#endif
