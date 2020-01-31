
#include <iomanip>
#include <iostream>
#include <thread>

using namespace std;

#include "synchronized_list.h"


#define N_INSERTS 16

void push_many(synchronized_list<int>& l) {
	for (int i = 0; i < N_INSERTS; i++) {
		l.push_back(i);
	}
}

void pop_many(synchronized_list<int>& l, int n_pops) {
	for (int i = 0; i < n_pops; i++) {
		cout << setw(3) << l.pop_front();
	}
	cout << endl;
}

#define N_THREADS 4

int main(int argc, char* argv[]) {

	synchronized_list<int> l;

	unique_ptr<thread> threads[N_THREADS];

	for (int i = 0; i < N_THREADS; i++) {
		threads[i] = make_unique<thread>(push_many, std::ref(l));
	}

	unique_ptr<thread> pop_threads[N_THREADS];

	for (int i = 0; i < N_THREADS; i++) {
		pop_threads[i] = make_unique<thread>(pop_many, std::ref(l), N_INSERTS);
	}

	for (int i = 0; i < N_THREADS; i++) {
		threads[i]->join();
	}

	for (int i = 0; i < N_THREADS; i++) {
		pop_threads[i]->join();
	}

	return 0;
}