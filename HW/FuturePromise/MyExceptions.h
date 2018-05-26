#pragma once
#include <exception>


struct SecondSet : public std::exception
{
	const char* what() const throw ()
	{
		return "SecondSet";
	}
};

struct EFPD_exception : public std::exception  //empty future, promise is dead
{
	const char* what() const throw ()
	{
		return "empty future, promise is dead";
	}
};