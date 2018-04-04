// CLP_N2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "setjmp.h"
#include <iostream>

enum errors {Bad_Work = 1, Bad_answer, Bad_student} err;

struct base_type {
	base_type();
	virtual ~base_type();
	base_type* prev;
	void delFromStack();
};

struct try_section {
	try_section();
	~try_section();
	jmp_buf buf;
	try_section* prev;
	base_type* ptr_to_top_objects_stack;
};

try_section* PtrTopTrySection = NULL;

base_type::base_type() {
	if (NULL != PtrTopTrySection)
	{
		prev = PtrTopTrySection->ptr_to_top_objects_stack;
		PtrTopTrySection->ptr_to_top_objects_stack = this;
	}
}

base_type::~base_type() {
	delFromStack();
}

void base_type::delFromStack() {
	if (NULL != PtrTopTrySection && (prev != NULL)) {
		PtrTopTrySection->ptr_to_top_objects_stack = prev;
		prev = NULL;
	}
}

try_section::try_section() {
	ptr_to_top_objects_stack = NULL;
	prev = PtrTopTrySection;
	PtrTopTrySection = this;
}

try_section::~try_section() {
	PtrTopTrySection = prev;
}

int throw_slice(int err);

#define TRY_SECTION { \
try_section __sl; \
int __exc = setjmp(__sl.buf); \
if (__exc == 0) 

#define CATCH_SECTION(exc) \
 else if (__exc == exc)

#define END_SECTION \
 else { \
throw_slice(__exc); \
} \
}

#define THROW_SECTION(exc) \
throw_slice (exc);

#define RETHROW \
throw_slice(__exc);

int pop_slice() {
	try_section* sl = PtrTopTrySection;
	if (NULL == sl) {
		std::abort();
	}
	PtrTopTrySection = sl->prev;
	return 0;
}

int throw_slice(int err) {
	try_section* sl = PtrTopTrySection;
	base_type* obj = PtrTopTrySection->ptr_to_top_objects_stack;
	while (NULL != obj) {
		base_type* tmp = obj;
		obj = obj->prev;
		tmp->~base_type();
	}
	pop_slice();
	longjmp(sl->buf, err);
	return 0;
}

class my_int : public base_type {
	int a;
public:
	my_int(int _a) { a = _a; };
	~my_int() { std::cout << "~my_int() " << a << std::endl; };
};

void foo2() {
	my_int a(10);
	my_int b(100);
}

void foo() {
	my_int a(11);
	my_int b(12);
	TRY_SECTION{
		my_int c(13);
		THROW_SECTION(Bad_Work)
	}
	CATCH_SECTION(Bad_answer){
		std::cout << "foo catched, resending... " << Bad_answer << std::endl;
		RETHROW
	}
	END_SECTION
	my_int d(14);
}

int main()
{
	my_int k(1);
	TRY_SECTION{
		my_int a(2);
		my_int b(3);
		foo();
	}
	CATCH_SECTION(Bad_Work){
		std::cout << "Catched Bad Work" << std::endl;
	}
	END_SECTION
	/*
	my_int kk(4);
	TRY_SECTION{
		my_int g(5);
		THROW_SECTION(2)
	}
	CATCH_SECTION{
		std::cout << "Catched" << std::endl;
	}}
	*/
    return 0;
}

