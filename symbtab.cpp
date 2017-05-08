SymbTabEntry::SymbTabEntry() {

}

SymbTabEntry::SymbTabEntry(std::string vf, std::string sc,
 datatype ty,int sz, int off,SymbTab* st)
{
	// std::cout<<"Symtab entry"<<std::endl;
	varfun = vf;
	scope = sc;
	type = ty;
	size = sz;
	offset = off;
	symbtab = st;
}

void SymbTabEntry::print()
{
	const char separator    = ' ';
    const int nameWidth     = 20;
    const int numWidth      = 8;
	std::cout<< left<<setw(nameWidth)<<setfill(separator)<<varfun<<
				left<<setw(nameWidth)<<setfill(separator)<<scope;
	std::cout<< left<<setw(nameWidth)<<setfill(separator)<<size<<
				left<<setw(nameWidth)<<setfill(separator)<<offset;
				type.print();
}

int SymbTab::push (std::string symbol, SymbTabEntry* entry)
{
	// std::cout<<"Hello"<<std::endl;
	if(Entries.find(symbol) != Entries.end())
	{
		return -1;	//Variable already exists
	}
	Entries[symbol] = *entry;			// entry inserted in symbol table
	return 0;
}

vector< pair <int,datatype> > SymbTab::getparameters() {
	vector< pair <int,datatype> > v;
	for(map<std::string,SymbTabEntry>::iterator it = Entries.begin();it!=Entries.end();++it)
	{
		if(it->second.offset>0)
			v.push_back({it->second.offset,it->second.type});
	}
	sort(v.begin(), v.end());
	reverse(v.begin(), v.end());
	return v;
}

SymbTab::SymbTab() {
	// std::cout<<"gst constructed"<<std::endl;	
}

void SymbTab::print() {
	vector<pair<SymbTabEntry,std::string> > temp;
	const char separator    = ' ';
    const int nameWidth     =20;
    const int numWidth      = 8;
	for(map<std::string,SymbTabEntry>::iterator it = Entries.begin();it!=Entries.end();++it)
	{
		temp.push_back(make_pair(it->second,it->first));
	}
	sort(temp.begin(),temp.end());

	for(vector<pair<SymbTabEntry,std::string> >::iterator it = temp.begin();it!=temp.end();++it)
	{
		std::cout<<left<<setw(nameWidth)<<setfill(separator)<<it->second;
		it->first.print();
		std::cout<<std::endl;
	}
}

void SymbTab::printgst() {
	std::vector<pair <string,SymbTab*> > v;
	cout<<"Global Symbol Table : "<<endl;
	const char separator    = ' ';
    const int nameWidth     = 20;
    const int numWidth      = 8;
	for(map<std::string,SymbTabEntry>::iterator it = Entries.begin();it!=Entries.end();++it)
	{
		std::cout<<left<<setw(nameWidth)<<setfill(separator)<<it->first;
		it->second.print();
		std::cout<<std::endl;
		v.push_back(make_pair(it->first,it->second.symbtab));
	}
	cout<<endl;
	cout<<endl;
	for(int i=0; i<v.size(); i++) {
		cout<<"Symbol Table for "<<v[i].first<<endl;
		v[i].second->print();
		cout<<endl;
		cout<<endl;
		cout<<endl;
	}
}


SymbTabEntry* SymbTab::search (std::string symbol)
{
	if(Entries.find(symbol)!= Entries.end())
	{
		return &Entries[symbol];
	}
	else 			// symbol not found in this symbol table
	{
		return NULL;
	}
}

int SymbTab::localSize() {
	int totalsize=0;
	for(map<std::string,SymbTabEntry>::iterator it = Entries.begin();it!=Entries.end();++it) {
		if(it->second.scope=="local")
			totalsize+=it->second.size;
	}
	return totalsize;
}

int SymbTab::paramSize() {
	int totalsize=0;
	for(map<std::string,SymbTabEntry>::iterator it = Entries.begin();it!=Entries.end();++it) {
		if(it->second.scope=="param")
			totalsize+=it->second.size;
	}
	return totalsize;
}

