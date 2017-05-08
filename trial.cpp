#include <bits/stdc++.h>
using namespace std;

int main () {
	std::vector<int> v(5);
	for(int i=0; i<5; i++)
		v[i]=i;
	v.resize(3);
	for(int i=0; i<3; i++)
		cout<<v[i]<<" ";
}