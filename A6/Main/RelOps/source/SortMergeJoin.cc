
#ifndef SORTMERGE_CC
#define SORTMERGE_CC

#include "Aggregate.h"
#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "SortMergeJoin.h"
#include "Sorting.h"

SortMergeJoin :: SortMergeJoin (MyDB_TableReaderWriterPtr leftInputIn, MyDB_TableReaderWriterPtr rightInputIn,
		MyDB_TableReaderWriterPtr outputIn, string finalSelectionPredicateIn, 
		vector <string> projectionsIn, pair <string, string> equalityCheckIn, 
        string leftSelectionPredicateIn, string rightSelectionPredicateIn) {
    
    output = outputIn;
    leftTable = leftInputIn;
    rightTable = rightInputIn;
    leftSelectionPredicate = leftSelectionPredicateIn;
    rightSelectionPredicate = rightSelectionPredicateIn;
    finalSelectionPredicate = finalSelectionPredicateIn;
    projections = projectionsIn;
    equalityCheck = equalityCheckIn;
    runSize = leftTable->getBufferMgr()->numPages / 2;
    
}

void SortMergeJoin :: run () {

    // create two empty rec for each table for inner table sorting
    MyDB_RecordPtr leftRecPtr1 = leftTable->getEmptyRecord();
    MyDB_RecordPtr leftRecPtr2 = leftTable->getEmptyRecord();
    MyDB_RecordPtr rightRecPtr1 = rightTable->getEmptyRecord();
    MyDB_RecordPtr rightRecPtr2 = rightTable->getEmptyRecord();

    // build result schema
    MyDB_SchemaPtr schemaOut = make_shared <MyDB_Schema> ();
    for (auto &p : leftTable->getTable()->getSchema()->getAtts()) {
        schemaOut->appendAtt(p);
    }
    for (auto &p : rightTable->getTable()->getSchema()->getAtts()) {
        schemaOut->appendAtt(p);
    }
    MyDB_RecordPtr recOut = make_shared <MyDB_Record> (schemaOut);
    recOut->buildFrom(leftRecPtr1, rightRecPtr1);

    func finalPredicate = recOut->compileComputation(finalSelectionPredicate);
    
    vector <func> finalComputations;
    for (string s : projections) {
        finalComputations.push_back(recOut->compileComputation(s));
    }

    // get feature to sort on
    string leftSortedBy = equalityCheck.first;
    string rightSortedBy = equalityCheck.second;

    // inner table comparator for sort phase
    function <bool ()> leftComp = buildRecordComparator(leftRecPtr1, leftRecPtr2, leftSortedBy);
	function <bool ()> rightComp = buildRecordComparator(rightRecPtr1, rightRecPtr2, rightSortedBy);

    // inner table comparator for equality checking
    function <bool ()> smallerThan = buildRecordComparator(leftRecPtr1, leftRecPtr2, leftSortedBy);
    function <bool ()> biggerThan = buildRecordComparator(leftRecPtr2, leftRecPtr1, leftSortedBy);

    // construct sorted iterator
    MyDB_RecordIteratorAltPtr leftSelectedIterator = buildItertorOverSortedRuns(runSize, *leftTable,
        leftComp, leftRecPtr1, leftRecPtr2, leftSelectionPredicate);
    MyDB_RecordIteratorAltPtr rightSelectedIterator = buildItertorOverSortedRuns(runSize, *rightTable,
        rightComp, rightRecPtr1, rightRecPtr2, rightSelectionPredicate);

    // this is output record
    MyDB_RecordPtr outputRec = output->getEmptyRecord();

    // compare records across two tables
	func leftSmaller = recOut->compileComputation (" < (" + leftSortedBy + ", " + rightSortedBy + ")");
	func rightSmaller = recOut->compileComputation (" > (" + leftSortedBy + ", " + rightSortedBy + ")");
    func areEqual = recOut->compileComputation (" == (" + leftSortedBy + ", " + rightSortedBy + ")");

    // merge phase
    bool hasLeft = leftSelectedIterator->advance();
    bool hasRight = rightSelectedIterator->advance();

    // it is time to run the merge!!
	MyDB_PageReaderWriter leftPage (true, *(leftTable->getBufferMgr()));
	vector <MyDB_PageReaderWriter> leftPages;

    while (hasLeft && hasRight) {

        // store the current record into leftRecPtr1 and rightRecPtr1
        leftSelectedIterator->getCurrent(leftRecPtr1);
        rightSelectedIterator->getCurrent(rightRecPtr1);

        // if left rec is smaller, advance to next left rec
        if (leftSmaller()->toBool()) {
            hasLeft = leftSelectedIterator->advance();
        }
        // if right rec is smaller, advance to next right rec
        else if (rightSmaller()->toBool()) {
            hasRight = rightSelectedIterator->advance();
        }
        // if left equals to right, find all left rec of same val and join with all right rec of same val
        else {

            leftPage.clear();
            leftPages.clear();
            leftPages.push_back(leftPage);
            leftPage.append(leftRecPtr1);

            // find all left rec of same values
            hasLeft = leftSelectedIterator->advance();
            while (hasLeft) {
                leftSelectedIterator->getCurrent(leftRecPtr2);
                // when next rec equals current rec
                if (!smallerThan() && !biggerThan()) {
                    if (!leftPage.append(leftRecPtr2)) {
                        MyDB_PageReaderWriter newLeftPage (true, *(leftTable->getBufferMgr()));
                        leftPage = newLeftPage;
                        leftPages.push_back(leftPage);
                        leftPage.append(leftRecPtr2);
                    }
                }
                else {
                    break;
                }
                hasLeft = leftSelectedIterator->advance();
            }

            // find all right recs that matches the current left recs
            while (true) {
                // curr right rec match curr val
                if (areEqual()->toBool()) {
                    MyDB_RecordIteratorAltPtr leftPagesIterator = getIteratorAlt(leftPages);
                    while (leftPagesIterator->advance()) {
                        leftPagesIterator->getCurrent(leftRecPtr1);
                        // if left rec and right rec satisfy final predicate
                        if (finalPredicate()->toBool()) {
							int i = 0;
							for (auto &f : finalComputations) {
								outputRec->getAtt(i++)->set(f());
							}
							outputRec->recordContentHasChanged ();
							output->append(outputRec);
                        }
                    }
                }
                else {
                    break;
                }

                hasRight = rightSelectedIterator->advance();
                if (!hasRight) {
                    break;
                }
                else {
                    rightSelectedIterator->getCurrent(rightRecPtr1);
                }
            }
        }

    }

}

#endif
