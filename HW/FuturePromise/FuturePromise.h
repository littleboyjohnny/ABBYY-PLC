#pragma once

#include <memory>
#include <condition_variable>
#include <mutex>
#include <functional>

#define interface struct

struct MultipleSetPromiseException : public std::exception
{
	const char* what() const throw ()
	{
		return "Trying to set promise value multiple times";
	}
};

struct DanglingFutureException : public std::exception
{
	const char* what() const throw ()
	{
		return "The future is dangling";
	}
};

template<typename T>
class SharedState {
public:
	SharedState() : exception(nullptr), terminated(false) {}

	virtual void Wait();
	virtual T* GetValue();
	T* GetOrSetValue(const T&v);
	void SetValue(const T& v);
	void SetException();
	void TryThrow() const;
	void Terminate();

private:
	std::unique_ptr<T> value;
	std::exception_ptr exception;
	std::mutex critical;
	std::condition_variable cv;
	bool terminated;
};

template <typename T>
class Future {
public:
	explicit Future(const std::shared_ptr<SharedState<T>>& state) : state(state) {}
	Future(const Future& other) : state(other.state) {}
	Future(Future&& other) : state(other.state) {}

	const T& Get();
	bool TryGet(T& value);
	template<typename R>
	Future<R> Then(std::function<R(const T&)> process);

private:
	std::shared_ptr<SharedState<T>> state;
};

template<typename T, typename R>
class WrappedState : public SharedState<R> {
public:
	WrappedState(Future<T> future, std::function<R(const T&)> worker) :
		future(future),
		processor(worker)
	{}

	virtual void Wait() override { return; }
	virtual R* GetValue() override;

private:
	Future<T> future;
	std::function<R(const T&)> processor;
};

template <typename T>
class Promise {
public:
	Promise();
	Promise(Promise&& other);
	Promise(const Promise& other) = delete;
	~Promise() { state->Terminate(); }

	void Set(const T& value) { state->SetValue(value); }
	void SetException() { state->SetException(); }

	Future<T> Result() { return Future<T>(state); }

private:
	std::shared_ptr<SharedState<T>> state;
};

template<typename T>
void SharedState<T>::Wait()
{
	std::unique_lock<std::mutex> lk(critical);
	cv.wait(lk, [this]() {
		if (value.get() == nullptr && exception == nullptr && terminated) {
			throw DanglingFutureException{};
		}
		return value.get() != nullptr || exception != nullptr;
	});
}

template<typename T>
T* SharedState<T>::GetValue()
{
	std::lock_guard<std::mutex> guard(critical);
	if (exception != nullptr) {
		std::rethrow_exception(exception);
	}
	return value.get();
}

template<typename T>
T* SharedState<T>::GetOrSetValue(const T& v)
{
	std::lock_guard<std::mutex> guard(critical);
	if (exception != nullptr) {
		std::rethrow_exception(exception);
	}
	if (value != nullptr) {
		return value.get();
	}
	value = std::move(std::make_unique<T>(v));
	cv.notify_all();
	return value.get();
}

template<typename T>
void SharedState<T>::TryThrow() const
{
	std::lock_guard<std::mutex> guard(critical);
	if (exception != nullptr) {
		std::rethrow_exception(exception);
	}
}

template<typename T>
void SharedState<T>::SetValue(const T& v)
{
	std::lock_guard<std::mutex> lock(critical);
	if (value != nullptr || exception != nullptr) {
		throw MultipleSetPromiseException{};
	}
	value = std::move(std::make_unique<T>(v));
	cv.notify_all();
}

template<typename T>
void SharedState<T>::SetException()
{
	std::lock_guard<std::mutex> lock(critical);
	if (value != nullptr || exception != nullptr) {
		throw MultipleSetPromiseException{};
	}
	exception = std::current_exception();
	cv.notify_all();
}

template<typename T>
void SharedState<T>::Terminate()
{
	std::lock_guard<std::mutex> lock(critical);
	terminated = true;
	cv.notify_all();
}

template<typename T>
const T& Future<T>::Get()
{
	state->Wait();
	return *state->GetValue();
}

template<typename T>
bool Future<T>::TryGet(T& value)
{
	T* current = state->GetValue();
	if (current == nullptr) {
		return false;
	}
	value = *current;
	return true;
}

template<typename T, typename R>
R* WrappedState<T, R>::GetValue()
{
	return this->GetOrSetValue(processor(future.Get()));
}

template<typename T>
template<typename R>
Future<R> Future<T>::Then(std::function<R(const T&)> process)
{
	return Future<R>(std::make_shared<WrappedState<T, R>>(*this, process));
}

template<typename T>
Promise<T>::Promise() : state(new SharedState<T>)
{
}

template<typename T>
Promise<T>::Promise(Promise<T>&& other) : state(other.state)
{
}