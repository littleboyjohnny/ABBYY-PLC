// PLC_N4_RTTI.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

class TypeID {
public:
	string name;
	int hash;

	TypeID() {}
	TypeID(string _name, int _hash) : name(_name), hash(_hash) {}
};

class TypeID_Holder {
public:
	TypeID typeinfo;
	void* forthis;
};

map<string, vector<string>> ClassGraph;
map<string, int> ClassSizes;
vector<string> da_way = {};

int insertInGraph(string child, int childSize, string parent1 = "", string parent2 = "") {
	vector<string> forParents;
	if (parent1 != "") {
		forParents.push_back(parent1);
		if (parent2 != "")
			forParents.push_back(parent2);
	}
	ClassGraph.insert(make_pair(child, forParents));
	ClassSizes.insert(make_pair(child, childSize - sizeof(TypeID_Holder)));
	return 0;
}

void* down_da_way(void* derPtr) {
	if (da_way.size() == 1)
		return derPtr;
	string curClass = da_way[da_way.size() - 1];
	da_way.pop_back();
	vector<string> forParents = ClassGraph[curClass];
	int shift = 0;
	for (int i = 0; i < forParents.size(); i++) {
		if (forParents[i] == da_way.back())
			break;
		shift += ClassSizes[forParents[i]];
	}
	derPtr = (void*)((int)derPtr + shift);
	return down_da_way(derPtr);
}

void* Shift(void* derPtr) {
	return down_da_way(derPtr);
}

bool my_search(string outName, string derName) {
	if (outName == derName) {
		da_way.push_back(outName);
		return true;
	}
	vector<string> loc_parents = ClassGraph[derName];
	if (loc_parents.size() == 0)
		return false;
	else {
		bool answer = false;
		for (int i = 0; i < loc_parents.size(); i++) {
			answer = my_search(outName, loc_parents[i]);
			if (answer) {
				da_way.push_back(derName);
				return true;
			}
		}
	}
}

bool Castable(string outName, string derName) {
	da_way = {};
	string newOut(outName, 0, outName.size() - 1);
	return my_search(newOut, derName);
}

#define DYNAMIC_CAST(outputType, inputPtr) \
((Castable(#outputType, (*inputPtr).typeinfo.name))? reinterpret_cast<outputType>(Shift((*inputPtr).forthis)) : nullptr)

#define TYPEID(inputPtr) \
(*inputPtr).typeinfo.name

#define BCLASS(Class) \
class Class : virtual public TypeID_Holder { \
int smth = [this]() { \
	this->typeinfo.name = #Class; \
	this->forthis = (void*)this; \
	return 0; \
}();

#define DCLASS(derClass, parentClass) \
class derClass : virtual public TypeID_Holder, public parentClass { \
int smth = [this]() { \
	this->typeinfo.name = #derClass; \
	this->forthis = (void*)this; \
	return 0; \
}();


#define DDCLASS(derClass, parentClass1, parentClass2) \
class derClass : virtual public TypeID_Holder, public parentClass1, public parentClass2 { \
int smth = [this]() { \
	this->typeinfo.name = #derClass; \
	this->forthis = (void*)this; \
	return 0; \
}();

#define USE_RTTI_B(Class) \
int __inserter_ ## Class = insertInGraph(#Class, sizeof(Class));

#define USE_RTTI_D(Class, parentClass) \
int __inserter_ ## Class = insertInGraph(#Class, sizeof(Class), #parentClass);

#define USE_RTTI_DD(Class, parentClass1, parentClass2) \
int __inserter_ ## Class = insertInGraph(#Class, sizeof(Class), #parentClass1, #parentClass2);

#define ENDCLASS \
};

BCLASS(A)
public:
	int a = 5;
	virtual void foo() {};
	void OnlyA() {
		cout << "OnlyA A a = " << a << endl;
	}
ENDCLASS

BCLASS(B)
public:
	int b = 6;
	void OnlyB() {
		cout << "OnlyB B b = " << b << endl;
	}
ENDCLASS

DDCLASS(C, A, B)
public:

ENDCLASS

USE_RTTI_B(A)
USE_RTTI_B(B)
USE_RTTI_DD(C, A, B)

int main()
{
	C* ptrC = new C();
	A* ptrA = new A();
	B* ptrB = new B();
	if (DYNAMIC_CAST(C*, ptrC) != nullptr)
		cout << "casted" << endl;
	else
		cout << "not" << endl;
	if (DYNAMIC_CAST(A*, ptrC) != nullptr) {
		cout << "casted" << endl;
		A* locptr = DYNAMIC_CAST(A*, ptrC);
		locptr->OnlyA();
	}
	else
		cout << "not" << endl;
	if (DYNAMIC_CAST(B*, ptrC) != nullptr)
		cout << "casted" << endl;
	else
		cout << "not" << endl;
	ptrA = DYNAMIC_CAST(A*, ptrC);
	if (DYNAMIC_CAST(B*, ptrA) != nullptr) {
		cout << "casted" << endl;
		B* locptr = DYNAMIC_CAST(B*, ptrA);
		locptr->OnlyB();
	}
	else
		cout << "not" << endl;
	if (DYNAMIC_CAST(C*, ptrB) != nullptr)
		cout << "casted" << endl;
	else
		cout << "not" << endl;
    return 0;
}

