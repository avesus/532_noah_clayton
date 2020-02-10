
#include <cassert>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

mutex m;

struct arr {
	int* a;
	int w, h;

	arr(int **a, int w, int h) :
		a((int*) a), w(w), h(h) {}

	int* operator[](int idx) {
		return &a[idx * w];
	}
};

struct rect {
	// x & y are starting row & col, w & h are width and height of the region
	int x, y, w, h;

	rect(int x, int y, int w, int h) :
		x(x), y(y), w(w), h(h) {}
};


static void count(int val, arr a, rect region) {
	int c = 0;
	for (int j = region.y; j < region.y + region.h; j++) {
		for (int i = region.x; i < region.x + region.w; i++) {
			if (a[j][i] == val) {
				c++;
			}
		}
	}
	lock_guard<mutex> g(m);
	cout << "found " << c << " occurrences of " << val << endl;
}

static void matrix_add(arr a, arr b, arr c, rect region) {
	for (int j = region.y; j < region.y + region.h; j++) {
		for (int i = region.x; i < region.x + region.w; i++) {
			c[j][i] = a[j][i] + b[j][i];
		}
	}
}

static void matrix_mult(arr a, rect arect, arr b, rect brect, arr c, rect crect) {
	assert(arect.w == brect.h);
	assert(arect.h == crect.h);
	assert(brect.w == crect.w);
	for (int j = 0; j < crect.h; j++) {
		for (int i = 0; i < crect.w; i++) {
			c[crect.y + j][crect.x + i] = 0;
			for (int k = 0; k < arect.w; k++) {
				c[crect.y + j][crect.x + i] += a[arect.y + j][arect.x + k] + b[brect.y + k][brect.x + i];
			}
		}
	}
}


int main(int argc, char* argv[]) {
	int a[4][16];
	int b[8][8];
	int c[16][8];

	int y[4][16];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 16; j++) {
			a[i][j] = i + j;
			y[i][j] = i - j;
		}
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			b[i][j] = i * j + 5;
		}
	}
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++) {
			c[i][j] = i - j;
		}
	}

	/*cout << "a:" << endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 16; j++) {
			cout << (void*)& a[i][j] << ": (" << i << ", " << j << ")" << endl;
		}
	}

	cout << endl << "b:" << endl;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cout << (void*)& b[i][j] << ": (" << i << ", " << j << ")" << endl;
		}
	}

	cout << endl << "c:" << endl;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 4; j++) {
			cout << (void*)& c[i][j] << ": (" << i << ", " << j << ")" << endl;
		}
	}*/

	int nthreads = std::thread::hardware_concurrency();
	if (nthreads < 4) {
		nthreads = 4;
	}

	cout << "spawining " << nthreads << " threads" << endl;

	vector<thread> threads;

	/*for (int i = 0; i < nthreads; i++) {
		arr ar((int**) a, 16, 4);
		rect r(0, 0, 16, 4);
		threads.push_back(thread(count, i, ar, r));
	}*/

	int res[4][8];

	arr ar((int**)a, 16, 4);
	arr cr((int**)c, 8, 16);
	arr rr((int**)res, 8, 4);

	for (int i = 0; i < nthreads; i++) {
		rect are(0, 2 * (i / 4), 16, 2);
		rect bre(2 * (i % 4), 0, 2, 16);
		rect cre(2 * (i % 4), 2 * (i / 4), 2, 2);
		threads.push_back(thread(matrix_mult, ar, are, cr, bre, rr, cre));
	}

	for (int i = 0; i < nthreads; i++) {
		threads[i].join();
	}


	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 16; j++) {
			cout << setw(4) << a[i][j];
		}
		cout << endl;
	}

	cout << "*" << endl;

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++) {
			cout << setw(4) << c[i][j];
		}
		cout << endl;
	}

	cout << "=" << endl;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 8; j++) {
			cout << setw(4) << res[i][j];
		}
		cout << endl;
	}

	/*threads.resize(0);

	for (int i = 0; i < nthreads; i++) {
		arr ar((int**)a, 16, 4);
		rect r(4 * (i % 4), 2 * (i / 4), 4, 2);
		threads.push_back(thread(count, 10, ar, r));
	}

	for (int i = 0; i < nthreads; i++) {
		threads[i].join();
	}*/

	

	return 0;
}
