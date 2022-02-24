
#ifndef SORT_C
#define SORT_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"
#include "RecordIteratorComparator.h"
#include "Sorting.h"

using namespace std;

/**
 * @brief merge phase: merge sorted page vecotrs into sorted pages and append them to the table sortIntoMe
 *
 * @param sortIntoMe
 * @param mergeUs
 * @param comparator
 * @param lhs
 * @param rhs
 */
void mergeIntoFile(MyDB_TableReaderWriter &sortIntoMe, vector<MyDB_RecordIteratorAltPtr> &mergeUs, function<bool()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs)
{
        RecordIteratorComparator recIterComp(comparator, lhs, rhs);
        priority_queue<MyDB_RecordIteratorAltPtr, vector<MyDB_RecordIteratorAltPtr>, RecordIteratorComparator> pq {recIterComp};
        for (int i = 0; i < mergeUs.size(); i++)
        {
                MyDB_RecordIteratorAltPtr ptr = mergeUs[i];
                if (ptr->advance()) {
                        pq.push(ptr);
                }
        }
        while (!pq.empty())
        {
                MyDB_RecordIteratorAltPtr itPtr = pq.top();
                pq.pop();
                MyDB_RecordPtr emptyRec = sortIntoMe.getEmptyRecord();
                itPtr->getCurrent(emptyRec);
                sortIntoMe.append(emptyRec);
                if (itPtr->advance())
                {
                        pq.push(itPtr);
                }
        }
}

/**
 * @brief sort phase: sort a single page vector of size runSize using merge sort
 *
 * @param parent
 * @param leftIter
 * @param rightIter
 * @param comparator
 * @param lhs
 * @param rhs
 * @return vector<MyDB_PageReaderWriter>
 */
vector<MyDB_PageReaderWriter> mergeIntoList(MyDB_BufferManagerPtr parent, MyDB_RecordIteratorAltPtr leftIter, MyDB_RecordIteratorAltPtr rightIter, function<bool()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs)
{
        vector<MyDB_PageReaderWriter> mergedList;
        MyDB_PageReaderWriter currPage(*parent);
        bool hasLeft = leftIter->advance();
        bool hasRight = rightIter->advance();
        while (hasLeft || hasRight)
        {
                if (hasLeft && hasRight)
                {
                        leftIter->getCurrent(lhs);
                        rightIter->getCurrent(rhs);
                        // if lhs < rhs
                        if (comparator())
                        {
                                if (!currPage.append(lhs))
                                {
                                        mergedList.push_back(currPage);
                                        MyDB_PageReaderWriter newPage(*parent);
                                        currPage = newPage;
                                        currPage.append(lhs);
                                }
                                hasLeft = leftIter->advance();
                        }
                        else
                        {
                                if (!currPage.append(rhs))
                                {
                                        mergedList.push_back(currPage);
                                        MyDB_PageReaderWriter newPage(*parent);
                                        currPage = newPage;
                                        currPage.append(rhs);
                                }
                                hasRight = rightIter->advance();
                        }
                }

                else if (hasLeft)
                {
                        leftIter->getCurrent(lhs);
                        if (!currPage.append(lhs))
                        {
                                mergedList.push_back(currPage);
                                MyDB_PageReaderWriter newPage(*parent);
                                currPage = newPage;
                                currPage.append(lhs);
                        }
                        hasLeft = leftIter->advance();
                }

                else
                {
                        rightIter->getCurrent(rhs);
                        if (!currPage.append(rhs))
                        {
                                mergedList.push_back(currPage);
                                MyDB_PageReaderWriter newPage(*parent);
                                currPage = newPage;
                                currPage.append(rhs);
                        }
                        hasRight = rightIter->advance();
                }
        }

        mergedList.push_back(currPage);
        return mergedList;
}

/*
int runSize: the size, in number of pages, of each run that is written as the result of the sort phase of the TPMMS

MyDB_TableReaderWriter &sortMe: The MyDB_TableReaderWriter whose contents are to be sorted.

MyDB_TableReaderWriter &sortIntoMe: The MyDB_TableReaderWriter that the results of the sort are to be written into. The result of the sort should be appended to the end of this file.

function <bool ()> comparator: The comparator function (a boolean lambda) that is to be used to power the sorting.

MyDB_RecordPtr lhs, rhs: The two MyDB_Record objects that the comparator function operates over. The idea is that to compare two records, one loads the records into the two provided
MyDB_Record objects, and then invokes the lambda to see if the contents of the first record are “less than” the contents second.
*/
void sort(int runSize, MyDB_TableReaderWriter &sortMe, MyDB_TableReaderWriter &sortIntoMe, function<bool()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs)
{
        int numPages = sortMe.getNumPages();
        MyDB_BufferManagerPtr mgr = sortMe.getBufferMgr();
        vector<MyDB_RecordIteratorAltPtr> runs;

        for (int i = 0; i < numPages; i += runSize)
        {
                vector<vector<MyDB_PageReaderWriter>> sortedPageVecs;
                for (int j = i; j < i + runSize; j++)
                {
                        vector<MyDB_PageReaderWriter> pageVec;
                        MyDB_PageReaderWriterPtr page = sortMe[j].sort(comparator, lhs, rhs);
                        pageVec.push_back(*page);
                        sortedPageVecs.push_back(pageVec);
                }

                int l, r, numVecs;
                while (sortedPageVecs.size() != 1)
                {
                        l = 0;
                        r = 1;
                        numVecs = sortedPageVecs.size();

                        vector<vector<MyDB_PageReaderWriter>> mergedSortedPageVecs;
                        while (l < numVecs && r < numVecs)
                        {
                                MyDB_RecordIteratorAltPtr lptr = getIteratorAlt(sortedPageVecs[l]);
                                MyDB_RecordIteratorAltPtr rptr = getIteratorAlt(sortedPageVecs[r]);
                                mergedSortedPageVecs.push_back(mergeIntoList(mgr, lptr, rptr, comparator, lhs, rhs));
                                l = r + 1;
                                r = l + 1;
                        }
                        // when left is not paired with a right, just append the entire left vec to the end of merged vecs
                        if (l < numVecs)
                        {
                                mergedSortedPageVecs.push_back(sortedPageVecs[l]);
                        }
                        sortedPageVecs = mergedSortedPageVecs;
                }

                runs.push_back(::getIteratorAlt(sortedPageVecs[0]));
        }

        mergeIntoFile(sortIntoMe, runs, comparator, lhs, rhs);
}

#endif
