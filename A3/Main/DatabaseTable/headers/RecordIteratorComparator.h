#ifndef REC_ITER_COMPARATOR_H
#define REC_ITER_COMPARATOR_H

#include "RecordComparator.h"
#include "MyDB_RecordIteratorAlt.h"

using namespace std;

class RecordIteratorComparator {

public:

	RecordIteratorComparator (function <bool ()> comparatorIn, MyDB_RecordPtr lhsIn,  MyDB_RecordPtr rhsIn) {
        comparator = comparatorIn;
		lhs = lhsIn;
		rhs = rhsIn;
	}

	bool operator () (MyDB_RecordIteratorAltPtr p1, MyDB_RecordIteratorAltPtr p2) {
        p1->getCurrent(lhs);
        p2->getCurrent(rhs);
        return !comparator();
	}

private:

    function <bool ()> comparator;
	MyDB_RecordPtr lhs;
	MyDB_RecordPtr rhs;

};

#endif