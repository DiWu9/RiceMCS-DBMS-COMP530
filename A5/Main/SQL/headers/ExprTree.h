
#ifndef SQL_EXPRESSIONS
#define SQL_EXPRESSIONS

#include "MyDB_AttType.h"
#include "MyDB_Schema.h"
#include "MyDB_Table.h"

#include <string>
#include <vector>
#include <map>

// create a smart pointer for database tables
using namespace std;
class ExprTree;
typedef shared_ptr <ExprTree> ExprTreePtr;

// this class encapsules a parsed SQL expression (such as "this.that > 34.5 AND 4 = 5")

// class ExprTree is a pure virtual class... the various classes that implement it are below
class ExprTree {

public:
	virtual string toString () = 0;
	virtual string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) = 0;
	virtual string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) = 0;
	virtual bool isValidAttribute (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) = 0;
	virtual bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) = 0;
	virtual ~ExprTree () {};
};

class BoolLiteral : public ExprTree {

private:
	bool myVal;
public:
	
	BoolLiteral (bool fromMe) {
		myVal = fromMe;
	}

	string toString () {
		if (myVal) {
			return "bool[true]";
		} else {
			return "bool[false]";
		}
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}
};

class DoubleLiteral : public ExprTree {

private:
	double myVal;
public:

	DoubleLiteral (double fromMe) {
		myVal = fromMe;
	}

	string toString () {
		return "double[" + to_string (myVal) + "]";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	~DoubleLiteral () {}
};

// this implement class ExprTree
class IntLiteral : public ExprTree {

private:
	int myVal;
public:

	IntLiteral (int fromMe) {
		myVal = fromMe;
	}

	string toString () {
		return "int[" + to_string (myVal) + "]";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "int";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "int";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	~IntLiteral () {}
};

class StringLiteral : public ExprTree {

private:
	string myVal;
public:

	StringLiteral (char *fromMe) {
		fromMe[strlen (fromMe) - 1] = 0;
		myVal = string (fromMe + 1);
	}

	string toString () {
		return "string[" + myVal + "]";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "string";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "string";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	~StringLiteral () {}
};

class Identifier : public ExprTree {

private:
	string tableName;
	string attName;
public:

	Identifier (char *tableNameIn, char *attNameIn) {
		tableName = string (tableNameIn);
		attName = string (attNameIn);
	}

	string toString () {
		return "[" + tableName + "_" + attName + "]";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return this->getType(tables, tableAlias);
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string realTableName = tableAlias.find(tableName)->second;
		pair <int, MyDB_AttTypePtr> attributeInfo = tables.find(realTableName)->second->getSchema()->getAttByName(attName);
		return attributeInfo.second->toString();
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		// get complete table name by its alias
		map<string, string>::iterator it = tableAlias.find(tableName);
		if (it == tableAlias.end()) {
			cout << "Error: table alias " + tableName + " before the attribute does not exist." << endl;
			return false;
		}
		// search attribute name in real table name
		string realTableName = it->second;
		pair <int, MyDB_AttTypePtr> attributeInfo = tables.find(realTableName)->second->getSchema()->getAttByName(attName);
		if (attributeInfo.first == -1) {
			cout << "Error: no such attribute exists in table" + realTableName + "." << endl;
			return false;
		}
		return true;
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return true;
	}

	~Identifier () {}
};

class MinusOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	MinusOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "- (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if (isLeftNumeric && isRightNumeric) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in minus operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = isLeftNumeric && isRightNumeric;
			if (!match) {
				cout << "Error: mismatch expression in minus operation." << endl;
			}
			return match;
		}
	}

	~MinusOp () {}
};

class PlusOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	PlusOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "+ (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->evaluatesTo(tables, tableAlias);
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if ((isLeftNumeric && isRightNumeric) || (l_eval == "string" && r_eval == "string")) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in plus(concat) operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = (isLeftNumeric && isRightNumeric) || (ltype == "string" && rtype == "string");
			if (!match) {
				cout << "Error: mismatch expression in plus(concat) operation." << endl;
			}
			return match;
		}
	}

	~PlusOp () {}
};

class TimesOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	TimesOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "* (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if (isLeftNumeric && isRightNumeric) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in times operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = isLeftNumeric && isRightNumeric;
			if (!match) {
				cout << "Error: mismatch expression in times operation." << endl;
			}
			return match;
		}
	}

	~TimesOp () {}
};

class DivideOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	DivideOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "/ (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if (isLeftNumeric && isRightNumeric) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in divide operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = isLeftNumeric && isRightNumeric;
			if (!match) {
				cout << "Error: mismatch expression in divide operation." << endl;
			}
			return match;
		}
	}

	~DivideOp () {}
};

class GtOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	GtOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "> (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if ((isLeftNumeric && isRightNumeric) || (l_eval == "string" && r_eval == "string")) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in greater than operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = (isLeftNumeric && isRightNumeric) || (ltype == "string" && rtype == "string");
			if (!match) {
				cout << "Error: mismatch expression in greater than operation." << endl;
			}
			return match;
		}
	}

	~GtOp () {}
};

class LtOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	LtOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "< (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if ((isLeftNumeric && isRightNumeric) || (l_eval == "string" && r_eval == "string")) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in less than operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = (isLeftNumeric && isRightNumeric) || (ltype == "string" && rtype == "string");
			if (!match) {
				cout << "Error: mismatch expression in less than operation." << endl;
			}
			return match;
		}
	}

	~LtOp () {}
};

class NeqOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	NeqOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "!= (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if ((isLeftNumeric && isRightNumeric) || (l_eval == r_eval)) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in neq operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = (isLeftNumeric && isRightNumeric) || (ltype == rtype);
			if (!match) {
				cout << "Error: mismatch expression in neq operation." << endl;
			}
			return match;
		}
	}

	~NeqOp () {}
};

class OrOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	OrOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "|| (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			if (l_eval == "bool" && r_eval == "bool") {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in or operation." << endl;
				return false;
			}
		}
		else {
			bool match = ltype == "bool" && rtype == "bool";
			if (!match) {
				cout << "Error: mismatch expression in or operation." << endl;
			}
			return match;
		}
	}

	~OrOp () {}
};

class EqOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	EqOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "== (" + lhs->toString () + ", " + rhs->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return lhs->isValidAttribute(tables, tableAlias) && rhs->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ltype = lhs->getType(tables, tableAlias);
		string rtype = rhs->getType(tables, tableAlias);
		if (ltype == "operation" || rtype == "operation") {
			string l_eval = lhs->evaluatesTo(tables, tableAlias);
			string r_eval = rhs->evaluatesTo(tables, tableAlias);
			bool isLeftNumeric = l_eval == "int" || l_eval == "double";
			bool isRightNumeric = r_eval == "int" || r_eval == "double";
			if ((isLeftNumeric && isRightNumeric) || (l_eval == r_eval)) {
				return lhs->noMismatchExpressions(tables, tableAlias) && rhs->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in equal operation." << endl;
				return false;
			}
		}
		else {
			bool isLeftNumeric = ltype == "int" || ltype == "double";
			bool isRightNumeric = rtype == "int" || rtype == "double";
			bool match = (isLeftNumeric && isRightNumeric) || (ltype == rtype);
			if (!match) {
				cout << "Error: mismatch expression in equal operation." << endl;
			}
			return match;
		}
	}

	~EqOp () {}
};

class NotOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	NotOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "!(" + child->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "bool";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return child->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ctype = child->getType(tables, tableAlias);
		if (ctype == "operation") {
			if (child->evaluatesTo(tables, tableAlias) == "bool") {
				return child->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in not operation." << endl;
				return false;
			}
		}
		else {
			bool match = ctype == "bool";
			if (!match) {
				cout << "Error: mismatch expression in not operation." << endl;
			}
			return match;
		}
	}

	~NotOp () {}
};

class SumOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	SumOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "sum(" + child->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return child->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ctype = child->getType(tables, tableAlias);
		if (ctype == "operation") {
			if (child->evaluatesTo(tables, tableAlias) == "double" || child->evaluatesTo(tables, tableAlias) == "int") {
				return child->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in sum()." << endl;
				return false;
			}
		}
		else {
			bool match = ctype == "int" || ctype == "double";
			if (!match) {
				cout << "Error: mismatch expression in sum()." << endl;
			}
			return match;
		}
	}

	~SumOp () {}
};

class AvgOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	AvgOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "avg(" + child->toString () + ")";
	}

	string evaluatesTo (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "double";
	}

	string getType(map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return "operation";
	}

	bool isValidAttribute (map <string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		return child->isValidAttribute(tables, tableAlias);
	}

	bool noMismatchExpressions (map<string, MyDB_TablePtr> tables, map<string, string> tableAlias) {
		string ctype = child->getType(tables, tableAlias);
		if (ctype == "operation") {
			if (child->evaluatesTo(tables, tableAlias) == "double" || child->evaluatesTo(tables, tableAlias) == "int") {
				return child->noMismatchExpressions(tables, tableAlias);
			}
			else {
				cout << "Error: mismatch expression in avg()." << endl;
				return false;
			}
		}
		else {
			bool match = ctype == "int" || ctype == "double";
			if (!match) {
				cout << "Error: mismatch expression in avg()." << endl;
			}
			return match;
		}
	}

	~AvgOp () {}
};

#endif
