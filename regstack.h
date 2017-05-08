#include <iostream>
#include <string>
#include <sstream>
#include <stack>

#ifndef REGSTACK_H
#define REGSTACK_H

#define AVAILABLE_REGS 3
#define AVAILABLE_FREGS 3

#define FLOAT_TYPE_REGS 1
#define INT_TYPE_REGS 2

using namespace std;
class reg_stack
{
public:
	// class node{
	// public:
	// 	string elem;
	// 	node* next;
	// };
	// node* head;
	stack<string> q;
	string top()
	{
		return q.top();
	}

	string pop()
	{
		string temp=q.top();
		q.pop();
		return temp;
	}

	void push(string s)
	{
		q.push(s);
	}

	void swap()
	{
		// if(head == NULL){cerr<<"Error:head not defined, swap, reg-stack"<<endl;exit(1);}
		// if(head->next == NULL)
		// {
		// 	cerr<<"Error: only one element in reg_stack, swap"<<endl;exit(1);
		// }
		// node* third = head->next->next;
		// node* first = head;
		// head = head->next;
		// head->next = first;
		// first->next=third;
		// return;
		string elem1=q.top();
		q.pop();
		string elem2=q.top();
		q.pop();
		q.push(elem1);
		q.push(elem2);
	}

	reg_stack(int type)
	{
		//finally the registers are in order: t0,t1..t9,s0,s1..s7
		if(type== INT_TYPE_REGS){
			for(int i=7;i>=0;--i){
				stringstream ss;
				ss<<i;
				push("$s"+ss.str());
			}
			for(int i=9;i>=0;--i){
				stringstream ss;
				ss<<i;
				push("$t"+ss.str());
			}
		} else if(type==FLOAT_TYPE_REGS){
			// cout<<"mtc1($0,$f0)"<<endl;				//using f28-f31 for auxullary purposes
										//f12 has to be used for printing purposes
			for(int i=27;i>1;--i){					//using $f1-$f27, $f0 being set to 0
				if(i==12) continue;
				stringstream ss;
				ss<<i;
				push("$f"+ss.str());
			}
		}


	}

	int size()
	{
		return q.size();
	}
};

#endif
