
#include <future>
#include <thread>

using namespace std;

class interrupt_flag {
	atomic_bool f;
public:
	interrupt_flag() : f(false) {}

	void set() {
		f = true;
	}
	bool is_set() {
		return f;
	}
};

thread_local interrupt_flag this_thread_interrupt_flag;

class interrupt_thread {
	thread internal_thread;
	interrupt_flag* flag;

	promise<int> j;
	future<int> _join;
public:
	template<typename FunctionType>
	interrupt_thread(FunctionType f) {
		promise<interrupt_flag*> p;
		internal_thread = thread([f, &p, this] {
			p.set_value(&this_thread_interrupt_flag);
			try {
				f();
			}
			catch (std::runtime_error& e) {
				printf("caught %s\n", e.what());
			}
			this->j.set_value(0);
			});
		flag = p.get_future().get();
		_join = j.get_future();
	}

	void interrupt() {
		if (flag) {
			flag->set();
		}
	}

	void join() {
		_join.get();
		internal_thread.join();
	}


	static void interrupt_point() {
		if (this_thread_interrupt_flag.is_set()) {
			throw std::runtime_error("interrupted");
		}
	}
};



void func() {
	while (1) {
		interrupt_thread::interrupt_point();
	}
}

size_t fib(size_t n) {
	if (n <= 1) {
		return n;
	}
	interrupt_thread::interrupt_point();
	int r1 = fib(n - 1);
	interrupt_thread::interrupt_point();
	int r2 = fib(n - 2);
	return r1 + r2;
}

int main(int argc, char* argv[]) {

	vector<interrupt_thread> ts;

	vector<std::packaged_task<size_t()>> tasks;
	vector<future<size_t>> fut;
	for (int i = 0; i < 8; i++) {
		tasks.push_back(std::packaged_task<size_t()>(std::bind(fib, 15)));
		fut.push_back(tasks[i].get_future());
		ts.push_back(interrupt_thread(std::move(tasks[i])));
	}


	while (1) {
		for (int i = 0; i < 8; i++) {
			size_t res = fut[i].get();
		}
	}
	for (int i = 0; i < 8; i++) {
		ts[i].interrupt();
	}

	for (int i = 0; i < 8; i++) {
		ts[i].join();
	}

	return 0;
}

