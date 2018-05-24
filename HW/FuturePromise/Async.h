#pragma once

#include "FuturePromise.h"
#include "ThreadPool.h"

namespace Sync {

	enum launch {
		async,
		sync
	};

}

template<class Func, class...Args>
class Async {
public:
	Async(ThreadPool& pool, Sync::launch type, Func&& f, Args&&... args) { f(args...); }
};
