#ifndef _SYNCHRONIZED_LIST_H
#define _SYNCHRONIZED_LIST_H

#include <list>
#include <mutex>

using namespace std;

template<typename T>
class synchronized_list {
	list<T> l;
	mutex mut;
	condition_variable cv;

	// watermarks for min and max allowed list sizes
	typename list<T>::size_type low, high;


public:
	synchronized_list(): low(0), high(0) {}

	synchronized_list(typename list<T>::size_type low, typename list<T>::size_type high):
		low(low), high(high) {}

	synchronized_list(const synchronized_list<T>& l) = delete;

	synchronized_list& operator=(const synchronized_list<T>& l) = delete;

	void push_back(const T &t) {
		unique_lock<mutex> lock(mut);

		cv.wait(lock, [&] { return (high == 0 || l.size() < high); });

		l.push_back(t);

		lock.unlock();
		cv.notify_all();
	}
	void push_front(const T &t) {
		unique_lock<mutex> lock(mut);

		cv.wait(lock, [&] { return (high == 0 || l.size() < high); });

		l.push_front(t);

		lock.unlock();
		cv.notify_all();
	}
	T &&pop_back() {
		unique_lock<mutex> lock(mut);

		cv.wait(lock, [&] { return l.size() > low; });

		T ret = l.back();
		l.pop_back();

		lock.unlock();
		cv.notify_all();

		return std::move(ret);
	}
	T &&pop_front() {
		unique_lock<mutex> lock(mut);

		cv.wait(lock, [&] { return l.size() > low; });

		T ret = l.front();
		l.pop_front();

		lock.unlock();
		cv.notify_all();

		return std::move(ret);
	}
};

#endif /* _SYNCHRONIZED_LIST_H */
