CXX=g++
CXXFLAGS=-O0 -g -D_GNU_SOURCE -std=c++11
LDFLAGS=-lpthread -lm

H_OBJS=lib/sp_arr.o lib/play.o lib/line.o lib/worker_thread.o helpers/locks.o helpers/temp.o helpers/opt.o helpers/util.o helpers/arg.o helpers/bits.o helpers/timing.o 

all: lab0


sp_arr.o: lib/sp_arr.cpp lib/sp_arr.h
	$(CXX) $(CXXFLAGS) -c lib/sp_arr.cpp -o lib/sp_arr.o $(LDFLAGS)

play.o: lib/play.cpp lib/play.h
	$(CXX) $(CXXFLAGS) -c lib/play.cpp -o lib/play.o $(LDFLAGS)

line.o: lib/line.cpp lib/line.h
	$(CXX) $(CXXFLAGS) -c lib/line.cpp -o lib/line.o $(LDFLAGS)

worker_thread.o: lib/worker_thread.cpp lib/worker_thread.h
	$(CXX) $(CXXFLAGS) -c lib/worker_thread.cpp -o lib/worker_thread.o $(LDFLAGS)

locks.o: helpers/locks.cpp helpers/locks.h
	$(CXX) $(CXXFLAGS) -c helpers/locks.cpp -o helpers/locks.o $(LDFLAGS)

temp.o: helpers/temp.cpp helpers/temp.h
	$(CXX) $(CXXFLAGS) -c helpers/temp.cpp -o helpers/temp.o $(LDFLAGS)

opt.o: helpers/opt.cpp helpers/opt.h
	$(CXX) $(CXXFLAGS) -c helpers/opt.cpp -o helpers/opt.o $(LDFLAGS)

util.o: helpers/util.cpp helpers/util.h
	$(CXX) $(CXXFLAGS) -c helpers/util.cpp -o helpers/util.o $(LDFLAGS)

arg.o: helpers/arg.cpp helpers/arg.h
	$(CXX) $(CXXFLAGS) -c helpers/arg.cpp -o helpers/arg.o $(LDFLAGS)

bits.o: helpers/bits.cpp helpers/bits.h
	$(CXX) $(CXXFLAGS) -c helpers/bits.cpp -o helpers/bits.o $(LDFLAGS)

timing.o: helpers/timing.cpp helpers/timing.h
	$(CXX) $(CXXFLAGS) -c helpers/timing.cpp -o helpers/timing.o $(LDFLAGS)


lab0.o: lab0.cpp lab0.h
	$(CXX) $(CXXFLAGS) -c lab0.cpp -o lab0.o $(LDFLAGS)

lab0: lab0.o $(H_OBJS)
	$(CXX) $(CXXFLAGS) lab0.o $(H_OBJS) -o lab0 $(LDFLAGS)

clean:
	rm -f lab0 *~ *.o *#* lib/*~ lib/*#* + lib/*.o helpers/*~ helpers/*#* + helpers/*.o

