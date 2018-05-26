#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <functional>
#include "MyExceptions.h"


template<typename type>
class Container {
	std::unique_ptr<type> value;
	std::exception_ptr exception;
	bool stopped;

	std::mutex critsec;
	std::condition_variable cv;
public:
	Container() : exception(nullptr), stopped(false) {}

	void wait();
	type* getValue();
	void setValue(const type& val);
	void setException();
	void tryThrow() const;

	void stop();
};

template<typename type>
void Container<type>::wait()
{
	std::unique_lock<std::mutex> lock(critsec);
	cv.wait(lock, [this]() {
		if (stopped && value.get() == nullptr && exception == nullptr) {
			throw EFPD_exception{};
		}
		if (value.get() != nullptr || exception != nullptr)
			return true;
		else
			return false;
	});
}

template<typename type>
type* Container<type>::getValue()
{
	std::lock_guard<std::mutex> lock(critsec);
	if (exception != nullptr) {
		std::rethrow_exception(exception);
	}
	return value.get();
}

template<typename type>
void Container<type>::tryThrow() const
{
	std::lock_guard<std::mutex> lock(critsec);
	if (exception != nullptr) {
		std::rethrow_exception(exception);
	}
}

template<typename type>
void Container<type>::setValue(const type& val)
{
	std::lock_guard<std::mutex> lock(critsec);
	if (value != nullptr || exception != nullptr) {
		throw SecondSet{};
	}
	value = std::move(std::make_unique<type>(val));
	cv.notify_all();
}

template<typename type>
void Container<type>::setException()
{
	std::lock_guard<std::mutex> lock(critsec);
	if (value != nullptr || exception != nullptr) {
		throw SecondSet{};
	}
	exception = std::current_exception();
	cv.notify_all();
}

template<typename type>
void Container<type>::stop()
{
	std::lock_guard<std::mutex> lock(critsec);
	stopped = true;
	cv.notify_all();
}