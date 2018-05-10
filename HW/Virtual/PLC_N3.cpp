// PLC_N3.cpp: определ€ет точку входа дл€ консольного приложени€.

//

#include "stdafx.h"
#include <iostream>
#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

/*
#define VIRTUAL_CLASS( name ) \
class name { \
public: \
	vector<string> methodTable; \
	string curClassName; \
	name() { \
		curClassName = #name; \
	}

#define END( name ) \
};

#define VIRTUAL_CLASS_DERIVED( nameDerived, nameBase ) \
class nameDerived : public nameBase { \
public: \
	nameDerived() { \
		curClassName = #nameDerived; \
	}
//static vector<string> methodTable; \
	//static vector<string> virtualTable;
//static type_info typ = typeid(nameBase);


#define END_DERIVED( nameDerived, nameBase ) \
};

#define DECLARE_METHOD( nameClass, nameMethod ) \
void nameMethod(nameClass obj) { \
	string str = typeid(nameClass).name(); \
	str.erase(0,str.find(' ') + 1); \
	std::cout << str << "::" << __FUNCTION__ << std::endl; \
}

#define VIRTUAL_CALL( ptr, method ) { \
string tp = typeid(ptr).name(); \
tp.erase(tp.rfind(' '), tp.length() - tp.rfind(' ')); \
tp.erase(0, tp.find(' ') + 1); \
string to = typeid(*ptr).name(); \
to.erase(0, to.find(' ') + 1); \
cout << tp << " " << to << endl; \
int i = 0; \
if (tp.compare(to) == 0) { \
if (tp == (*ptr).curClassName) { \
	cout << "Equal" << endl; \
	method(*ptr); \
} \
else { \
	method(*ptr); \
	cout << "Not" << endl; \
	method(*ptr); \
} \
}

VIRTUAL_CLASS(Base)
	int a;
END(Base)

DECLARE_METHOD(Base, Both)
DECLARE_METHOD(Base, OnlyBase)

VIRTUAL_CLASS_DERIVED(Derived, Base)
	int b;
END_DERIVED(Derived, Base)

DECLARE_METHOD(Derived, Both)
DECLARE_METHOD(Derived, OnlyDerived)



int main()
{
	//vector<int> a;
	Base base;
	Derived derived;
	base.a = 5;
	Base* reallyDerived = reinterpret_cast<Base *>(&derived);
	//Base* bptr = &b;
	VIRTUAL_CALL(&base, Both); // печатает УBase::Both a = 0Ф
	//VIRTUAL_CALL(reallyDerived, Both); // печатает УDerived::Both b = 1Ф
	VIRTUAL_CALL(reallyDerived, OnlyBase);  // печатает УBase::OnlyBaseФ
	//VIRTUAL_CALL(reallyDerived, OnlyDerived);
	//cout << typeid(ptr).name() << endl;
	//cout << typeid(*ptr).name() << endl;
	//Both(b);
	//VIRTUAL_CALL ( ptr, Both )
	system("pause");
	return 0;
}
*/



#define VIRTUAL_CLASS( name ) \
class name { \
public: \
	static set<string> methodTable; \
	static set<string> der_methodTable; \
	static map<string, string> vtable; \
	static string nbase; \
	bool is_inh = false;

#define END( name ) \
}; \
set<string> name::methodTable = {}; \
string name::nbase = (string)#name; \
map<string, string> name::vtable = {}; 

#define VIRTUAL_CLASS_DERIVED( nameDerived, nameBase ) \
class nameDerived : public nameBase { \
public: \
	nameDerived() { \
		is_inh = true; \
	}

#define END_DERIVED( nameDerived, nameBase ) \
}; \
set<string> nameDerived::der_methodTable = {}; 

#define DECLARE_METHOD( nameClass, nameMethod ) { \
nameClass::vtable[(string)#nameMethod] = (string)#nameClass; \
if (nameClass::nbase == (string)#nameClass) { \
	nameClass::methodTable.insert((string)#nameMethod); \
} \
else { \
	nameClass::der_methodTable.insert((string)#nameMethod); \
} \
}

#define VIRTUAL_CALL( ptr, method ) { \
string to = typeid(ptr).name(); \
to.erase(to.rfind(' '), to.length() - to.rfind(' ')); \
to.erase(0, to.find(' ') + 1); \
if (to == (*ptr).nbase) { \
	auto imet = (*ptr).methodTable.find(#method); \
	if (imet == (*ptr).methodTable.end()) {\
		cout << "Error" << endl; \
	} \
	else { \
		if ((*ptr).is_inh) { \
			auto ivmet = (*ptr).vtable.find(#method); \
			cout << ivmet->second << "::" << #method << endl; \
		} \
		else \
			cout << to << "::" << #method << endl; \
	} \
} \
else { \
	auto imet = (*ptr).der_methodTable.find(#method); \
	if (imet == (*ptr).der_methodTable.end()) { \
		imet = (*ptr).methodTable.find(#method); \
		if (imet == (*ptr).methodTable.end()) \
			cout << "Error" << endl; \
		else { \
			cout << to << "::" << #method << endl; \
		} \
	} \
	else { \
		cout << to << "::" << #method << endl; \
	} \
}\
}
/*auto imet = (*ptr).methodTable.find(#method); \
if (imet == (*ptr).methodTable.end()) { \
	cout << "Error" << endl; \
} \
else { \
	auto imet = (*ptr).vtable.find(#method); \
	cout << imet->second << "::" << #method; \
} \
}*/

VIRTUAL_CLASS(Base)
int a;
END(Base)

VIRTUAL_CLASS_DERIVED(Derived, Base)
int b;
END_DERIVED(Derived, Base)

int main()
{
	DECLARE_METHOD(Base, Both)
	DECLARE_METHOD(Base, OnlyBase)
	DECLARE_METHOD(Derived, Both)
	DECLARE_METHOD(Derived, OnlyDerived)
	//vector<int> a;
	Base base;
	Derived derived;
	base.a = 5;
	Base* reallyDerived = reinterpret_cast<Base *>(&derived);
	//Base* bptr = &b;
	VIRTUAL_CALL(&base, Both); // печатает УBase::Both a = 0Ф
	VIRTUAL_CALL(reallyDerived, Both); // печатает УDerived::Both b = 1Ф
	VIRTUAL_CALL(reallyDerived, OnlyBase);  // печатает УBase::OnlyBaseФ
	VIRTUAL_CALL(reallyDerived, OnlyDerived);
											//cout << typeid(ptr).name() << endl;
											//cout << typeid(*ptr).name() << endl;
											//Both(b);
											//VIRTUAL_CALL ( ptr, Both )
	system("pause");
	return 0;
}

