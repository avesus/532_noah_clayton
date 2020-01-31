1.

Clayton Knittel
Noah Goldstein


2.

{
	synchronized_list<int> l;

	l.push_front(1);
	l.push_front(3);
	l.push_front(4);
	l.push_front(6);

	cout << l.pop_back() << l.pop_back() << l.pop_back() << l.pop_back() << endl;
}

output:

6431


3.

Output:
  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  0  1  0  1  2  3  4  5  6  7  8  9  2  3  4  5
  6  7  8  9 10 11 12 13 14 15  0  0  1  2  3  4
  5  6  7  8  9 10 11 12 13 14 15  1  2  3  4  5
  6  7  8  9 10 11 12 13 14 15 10 11 12 13 14 15

Sometimes the numbers get interleaved, as threads execute
concurrently and may insert before the previously spawned
thread has finished inserting


4.

Output:
  0  6  9  3 11 12  17  8  0  1  2  3 13  5  6  7 10  9 10 11 12 13  2 15  4  1  2  3  4  5  6  7  8  9 10 11 12 13 148 15  0  4  2  3  4
  0  6  7  8  9 10 11 12 13 14 15  0  5  2  3  4 15  6  1  8
14  9 10 11 12 13  5 15  0  1
14 14  3  5  5  6  7  8  1  2 11 12 13 10 15  4  1  2  7  0  5  6
4  9  8  9 10 11 12 13 14 15  7  1  2  3  4  5  6  7  8  9 10
0 14 12 13 14
3 11
15

Now, not only are the numbers potentially interleaved, but also printing is interleaved, so the numbers may be jumbled
a second time and the newlines do not appear in the correct spots


5.


new class code (important parts)

template<typename T>
class synchronized_list {
	list<T> l;
	mutex mut;
	condition_variable cv;

public:
	void push_back(const T &t) {
		unique_lock<mutex> lock(mut);

		l.push_back(t);

		lock.unlock();
		cv.notify_all();
	}
	T &&pop_front() {
		unique_lock<mutex> lock(mut);

		cv.wait(lock, [&] { return l.size() > 0; });

		T ret = l.front();
		l.pop_front();

		lock.unlock();
		cv.notify_all();

		return std::move(ret);
	}
};


example output:
  0  4  5  6  1  8  3 10 11 12  7  2 15  0  1  2  3  4  5  6  7  8  9 10 14  9 13 14 15 13 11  2  3  4  5  6  7  8  9 10 11 1212 13 14 15  0  1  2  3  4  5
0

1  6  7  8  9 10 11 12 13 14 15


some differences in this program's behavior are that the ordering of the numbers and the interleaving of spaces are much more
varied compared to previous runs


6.


template<typename T>
class synchronized_list {
	list<T> l;
	mutex mut;
	condition_variable cv;

public:
	void push_back(const T &t) {
		unique_lock<mutex> lock(mut);

		cv.wait(lock, [&] { return (high == 0 || l.size() < high); });

		l.push_back(t);

		lock.unlock();
		cv.notify_all();
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
