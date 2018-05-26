#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <functional>
#include "MyExceptions.h"
#include "Container.h"

template <typename type>
class Future {
	std::shared_ptr<Container<type>> container;
public:
	Future(const std::shared_ptr<Container<type>>& container) : container(container) {}
	Future(const Future& future) : container(future.container) {}
	Future(Future&& future) : container(future.container) {}

	const type& Get();
	bool TryGet(type& value);
};

template<typename type>
const type& Future<type>::Get()
{
	container->wait();
	return *container->getValue();
}

template<typename type>
bool Future<type>::TryGet(type& value)
{
	type* curr = container->getValue();
	if (curr == nullptr) {
		return false;
	}
	else {
		value = *curr;
		return true;
	}
}

//==================================================================================================================

template <typename type>
class Promise {
	std::shared_ptr<Container<type>> container;
public:
	Promise() : container(new Container<type>) {}
	Promise(Promise&& promise) : container(promise.container) {}
	Promise(const Promise& promise) = delete;
	~Promise();

	void Set(const type& value);
	void SetException();

	Future<type> get_future();
};

template <typename type>
Promise<type>::~Promise() {
	container->stop();
}

template <typename type>
void Promise<type>::Set(const type& value) { 
	container->setValue(value); 
}

template <typename type>
void Promise<type>::SetException() { 
	container->setException(); 
}

template <typename type>
Future<type> Promise<type>::get_future() { 
	return std::move(Future<type>(container)); 
}