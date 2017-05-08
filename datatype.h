#ifndef DATATYPE_H
#define DATATYPE_H
class datatype {
public:
	int type;
	std::string data;
	int deref;
	bool is_array;
	std::vector<int> array;

	datatype();
	int size();
};
#endif