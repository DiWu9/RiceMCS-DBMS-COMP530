
#ifndef REG_SELECTION_C                                        
#define REG_SELECTION_C

#include "RegularSelection.h"

RegularSelection :: RegularSelection (MyDB_TableReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
		string selectionPredicateIn, vector <string> projectionsIn) {
    
    input = inputIn;
    output = outputIn;
    selectionPredicate = selectionPredicateIn;
    projections = projectionsIn;

}

void RegularSelection :: run () {

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
    MyDB_RecordIteratorAltPtr recordIterator = input->getIteratorAlt();

    while (recordIterator->advance()) {

        recordIterator->getCurrent(recIn);

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
