#include <map>
#include <algorithm>
#include <iomanip>

#ifndef SYMBTAB_H
#define SYMBTAB_H
class SymbTab;	// forward declaration for symbtab pointer in the symbol table entry

class SymbTabEntry
{
public:
	std:: string varfun;
	std:: string  scope;
	datatype type;
	int size;
	int offset;
	SymbTab* symbtab;
	SymbTabEntry();
	SymbTabEntry(std::string vf, std::string sc, datatype ty, int sz, int off, SymbTab* st);
	void print();
	bool operator<(const SymbTabEntry& str) const
    {
        return (offset > str.offset);
    }
};

class SymbTab
{
public:
	map<std::string,SymbTabEntry> Entries; 
	SymbTab();
	void print();
	void printgst();
	int localSize();
	int paramSize();
	int push(std::string symbol, SymbTabEntry* entry);
	SymbTabEntry* search(std::string symbol);
	vector< pair <int,datatype> > getparameters();
};
#endif