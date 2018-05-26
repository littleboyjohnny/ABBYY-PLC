#include "FuturePromise.h"
#include "Async.h"
#include "ThreadPool.h"
#include <iostream>

Promise<int> promise;
Promise<int> promise1;

void th_f1() {
	promise.Set(10);
}

void th_f2() {
	std::cout << promise.get_future().Get() << std::endl;
}

void th_f3() {
	promise1.SetException();
}

void th_f4() {
	int val = 0;
	bool answ = promise1.get_future().TryGet(val);
	std::cout << ((answ) ? "OK" : "FAIL") << std::endl;
}

bool is_prime(int x) {
	std::cout << "Calculating. Please, wait...\n";
	for (int i = 2; i<x; ++i) if (x%i == 0) return false;
	return true;
}

int main() {
	std::thread th1(th_f1);
	std::thread th2(th_f2);
	th2.join();
	th1.join();
	// 10
	
	Promise<int> prom;
	prom.Set(3);
	try {
		prom.Set(4);
		std::cout << prom.get_future().Get() << std::endl;
	}
	catch(...){
		std::cout << "Got ya!" << std::endl;
	}
	

	std::thread th3(th_f3);
	std::thread th4(th_f4);
	th3.join();
	th4.join();

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
	system("pause");
	return 0;
}
