
#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include <iostream>
#include <stdlib.h>
#include "ExprTree.h"
#include "MyDB_Catalog.h"
#include "MyDB_Schema.h"
#include "MyDB_Table.h"
#include <string>
#include <utility>
#include <map>
#include <set>

using namespace std;

/*************************************************/
/** HERE WE DEFINE ALL OF THE STRUCTS THAT ARE **/
/** PASSED AROUND BY THE PARSER                **/
/*************************************************/

// structure that encapsulates a parsed computation that returns a value
struct Value {

private:

        // this points to the expression tree that computes this value
        ExprTreePtr myVal;

public:
        ~Value () {}

        Value (ExprTreePtr useMe) {
                myVal = useMe;
        }

        Value () {
                myVal = nullptr;
        }
	
	friend struct CNF;
	friend struct ValueList;
	friend struct SFWQuery;
	#include "FriendDecls.h"
};

// structure that encapsulates a parsed CNF computation
struct CNF {

private:

        // this points to the expression tree that computes this value
        vector <ExprTreePtr> disjunctions;

public:
        ~CNF () {}

        CNF (struct Value *useMe) {
              	disjunctions.push_back (useMe->myVal); 
        }

        CNF () {}

	friend struct SFWQuery;
	#include "FriendDecls.h"
};

// structure that encapsulates a parsed list of value computations
struct ValueList {

private:

        // this points to the expression tree that computes this value
        vector <ExprTreePtr> valuesToCompute;

public:
        ~ValueList () {}

        ValueList (struct Value *useMe) {
              	valuesToCompute.push_back (useMe->myVal); 
        }

        ValueList () {}

	friend struct SFWQuery;
	#include "FriendDecls.h"
};


// structure to encapsulate a create table
struct CreateTable {

private:

	// the name of the table to create
	string tableName;

	// the list of atts to create... the string is the att name
	vector <pair <string, MyDB_AttTypePtr>> attsToCreate;

	// true if we create a B+-Tree
	bool isBPlusTree;

	// the attribute to organize the B+-Tree on
	string sortAtt;

public:
	string addToCatalog (string storageDir, MyDB_CatalogPtr addToMe) {

		// make the schema
		MyDB_SchemaPtr mySchema = make_shared <MyDB_Schema>();
		for (auto a : attsToCreate) {
			mySchema->appendAtt (a);
		}

		// now, make the table
		MyDB_TablePtr myTable;

		// just a regular file
		if (!isBPlusTree) {
			myTable =  make_shared <MyDB_Table> (tableName, 
				storageDir + "/" + tableName + ".bin", mySchema);	

		// creating a B+-Tree
		} else {
			
			// make sure that we have the attribute
			if (mySchema->getAttByName (sortAtt).first == -1) {
				cout << "B+-Tree not created.\n";
				return "nothing";
			}
			myTable =  make_shared <MyDB_Table> (tableName, 
				storageDir + "/" + tableName + ".bin", mySchema, "bplustree", sortAtt);	
		}

		// and add to the catalog
		myTable->putInCatalog (addToMe);

		return tableName;
	}

	CreateTable () {}

	CreateTable (string tableNameIn, vector <pair <string, MyDB_AttTypePtr>> atts) {
		tableName = tableNameIn;
		attsToCreate = atts;
		isBPlusTree = false;
	}

	CreateTable (string tableNameIn, vector <pair <string, MyDB_AttTypePtr>> atts, string sortAttIn) {
		tableName = tableNameIn;
		attsToCreate = atts;
		isBPlusTree = true;
		sortAtt = sortAttIn;
	}
	
	~CreateTable () {}

	#include "FriendDecls.h"
};

// structure that stores a list of attributes
struct AttList {

private:

	// the list of attributes
	vector <pair <string, MyDB_AttTypePtr>> atts;

public:
	AttList (string attName, MyDB_AttTypePtr whichType) {
		atts.push_back (make_pair (attName, whichType));
	}

	~AttList () {}

	friend struct SFWQuery;
	#include "FriendDecls.h"
};

struct FromList {

private:

	// the list of tables and aliases
	vector <pair <string, string>> aliases;

public:
	FromList (string tableName, string aliasName) {
		aliases.push_back (make_pair (tableName, aliasName));
	}

	FromList () {}

	~FromList () {}
	
	friend struct SFWQuery;
	#include "FriendDecls.h"
};


// structure that stores an entire SFW query
struct SFWQuery {

private:

	// the various parts of the SQL query
	vector <ExprTreePtr> valuesToSelect; // select
	vector <pair <string, string>> tablesToProcess; // from
	vector <ExprTreePtr> allDisjunctions; // where
	vector <ExprTreePtr> groupingClauses; // group by

public:
	SFWQuery () {}

	SFWQuery (struct ValueList *selectClause, struct FromList *fromClause, 
		struct CNF *cnf, struct ValueList *grouping) {
		valuesToSelect = selectClause->valuesToCompute;
		tablesToProcess = fromClause->aliases;
		allDisjunctions = cnf->disjunctions;
		groupingClauses = grouping->valuesToCompute;
	}

	SFWQuery (struct ValueList *selectClause, struct FromList *fromClause, 
		struct CNF *cnf) {
		valuesToSelect = selectClause->valuesToCompute;
		tablesToProcess = fromClause->aliases;
		allDisjunctions = cnf->disjunctions;
	}

	SFWQuery (struct ValueList *selectClause, struct FromList *fromClause) {
		valuesToSelect = selectClause->valuesToCompute;
		tablesToProcess = fromClause->aliases;
		allDisjunctions.push_back (make_shared <BoolLiteral> (true));
	}
	
	~SFWQuery () {}

	void print () {
		cout << "Selecting the following:\n";
		for (auto a : valuesToSelect) {
			cout << "\t" << a->toString () << "\n";
		}
		cout << "From the following:\n";
		for (auto a : tablesToProcess) {
			cout << "\t" << a.first << " AS " << a.second << "\n";
		}
		cout << "Where the following are true:\n";
		for (auto a : allDisjunctions) {
			cout << "\t" << a->toString () << "\n";
		}
		cout << "Group using:\n";
		for (auto a : groupingClauses) {
			cout << "\t" << a->toString () << "\n";
		}
	}

	// checkers

	/**
	 * @brief check if query tables exists in DB
	 */
	bool isInTables (map <string, MyDB_TablePtr> tables) {
		map<string, MyDB_TablePtr>::iterator it;
		for (auto a : tablesToProcess) {
			it = tables.find(a.first);
			if (it == tables.end()) {
				return false;
			}
		}
		return true;
	}

	/**
	 * @brief check all attributes exists in table
	 */
	bool validAttributes (map <string, MyDB_TablePtr> tables) {
		map<string, string> tableAlias;
		for (auto a : tablesToProcess) {
			tableAlias.insert(pair<string, string>(a.second, a.first)); // alias -> tablename
		}
		for (ExprTreePtr v : valuesToSelect) {
			if (!v->isValidAttribute(tables, tableAlias)) {
				return false;
			}
		}
		for (ExprTreePtr d : allDisjunctions) {
			if (!d->isValidAttribute(tables, tableAlias)) {
				return false;
			}
		}
		for (ExprTreePtr g : groupingClauses) {
			if (!g->isValidAttribute(tables, tableAlias)) {
				return false;
			}
		}
		return true;
	}

	/**
	 * @brief check all arithmetic has valid data types
	 */
	bool noMismatchExpressions (map <string, MyDB_TablePtr> tables) {
		map<string, string> tableAlias;
		for (auto a : tablesToProcess) {
			tableAlias.insert(pair<string, string>(a.second, a.first)); // alias -> tablename
		}
		// check arithmetic in select
		for (ExprTreePtr v : valuesToSelect) {
			if (!v->noMismatchExpressions (tables, tableAlias)) {
				return false;
			}
		}
		// check compare in where
		for (ExprTreePtr d : allDisjunctions) {
			if (!d->noMismatchExpressions (tables, tableAlias)) {
				return false;
			}
		}
		return true;
	}

	/**
	 * @brief check the non-aggregate selected attributes are in GROUP BY
	 */
	bool validAggregation (map <string, MyDB_TablePtr> tables) {
		if (groupingClauses.empty()) { // no aggregation term
			return true;
		}
		else {
			set<string> groupBys;
			for (ExprTreePtr g : groupingClauses) {
				string clause = g->toString();
				if (clause[0] != '[') {
					cout << "Error: invalid clause to group by: " + clause + "." << endl;
					return false; // not grouping by identifiers
				}
				else {
					groupBys.insert(clause);
				}
			}
			for (ExprTreePtr v : valuesToSelect) {
				string clause = v->toString();
				if (clause[0] == '[') {
					if (groupBys.find(clause) == groupBys.end()) {
						cout << "Error: attribute " + clause + " should be aggregated in group by." << endl;
						return false;
					}
				}
			}
			return true;
		}
	}

	#include "FriendDecls.h"
};

// structure that sores an entire SQL statement
struct SQLStatement {

private:

	// in case we are a SFW query
	SFWQuery myQuery;
	bool isQuery;

	// in case we a re a create table
	CreateTable myTableToCreate;
	bool isCreate;

public:
	SQLStatement (struct SFWQuery* useMe) {
		myQuery = *useMe;
		isQuery = true;
		isCreate = false;
	}

	SQLStatement (struct CreateTable *useMe) {
		myTableToCreate = *useMe;
		isQuery = false;
		isCreate = true;
	}

	bool isCreateTable () {
		return isCreate;
	}

	bool isSFWQuery () {
		return isQuery;
	}

	string addToCatalog (string storageDir, MyDB_CatalogPtr addToMe) {
		return myTableToCreate.addToCatalog (storageDir, addToMe);
	}		
	
	void printSFWQuery () {
		myQuery.print ();
	}

	// checkers

	/**
	 * @brief check if query tables exists in DB
	 * 
	 * @param tables existing tables in DB
	 */
	bool isInTables (map <string, MyDB_TablePtr> tables) {
		return myQuery.isInTables(tables);
	}

	/**
	 * @brief check all attributes exists in table
	 */
	bool validAttributes (map <string, MyDB_TablePtr> tables) {
		return myQuery.validAttributes(tables);
	}

	/**
	 * @brief check all arithmetic has valid data types
	 */
	bool noMismatchExpressions (map <string, MyDB_TablePtr> tables) {
		return myQuery.noMismatchExpressions(tables);
	}

	/**
	 * @brief check the non-aggregate selected attributes are in GROUP BY
	 */
	bool validAggregation (map <string, MyDB_TablePtr> tables) {
		return myQuery.validAggregation(tables);
	}

	#include "FriendDecls.h"
};

#endif
