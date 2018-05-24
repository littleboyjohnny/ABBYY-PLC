#include "FuturePromise.h"
#include "Async.h"
#include "ThreadPool.h"
#include <iostream>

int main() {
	ThreadPool pool(4);
	int size = 4;
	bool* done = new bool[size];
	for (int i = 0; i < size; ++i) {
		done[i] = false;
	}

	for (int i = 0; i < size; ++i) {
		pool.addTask([&done, i]() {
			done[i] = true;
		});
	}

	pool.stop();

	for (int i = 0; i < size; ++i) {
		if (!done[i])
			std::cout << "Fail\n";
	}
	return 0;
}
