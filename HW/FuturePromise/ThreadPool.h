#pragma once

#include <assert.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
	std::vector<std::thread> threads;
	std::vector<std::function<void()>> tasks;
	std::vector<bool> busy;
	std::mutex critsec;
	std::condition_variable cv;
	int size;
	bool stoped;

	void init();

public:
	ThreadPool(int size) :size(size), stoped(false) {}

	ThreadPool(ThreadPool&& tp) = delete;

	ThreadPool(const ThreadPool& tp) = delete;

	~ThreadPool();

	void addTask(std::function<void()> task);
	void stop();
};

