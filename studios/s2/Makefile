CXX=g++
CXXFLAGS=-O0 -g -D_GNU_SOURCE
LDFLAGS=-lpthread -lm

H_OBJS=helpers/locks.o helpers/util.o helpers/arg.o helpers/bits.o helpers/timing.o 

all: s2

locks.o: helpers/locks.cpp helpers/locks.h
	$(CXX) $(CXXFLAGS) -c helpers/locks.cpp -o helpers/locks.o $(LDFLAGS)

util.o: helpers/util.cpp helpers/util.h
	$(CXX) $(CXXFLAGS) -c helpers/util.cpp -o helpers/util.o $(LDFLAGS)

arg.o: helpers/arg.cpp helpers/arg.h
	$(CXX) $(CXXFLAGS) -c helpers/arg.cpp -o helpers/arg.o $(LDFLAGS)

bits.o: helpers/bits.cpp helpers/bits.h
	$(CXX) $(CXXFLAGS) -c helpers/bits.cpp -o helpers/bits.o $(LDFLAGS)

timing.o: helpers/timing.cpp helpers/timing.h
	$(CXX) $(CXXFLAGS) -c helpers/timing.cpp -o helpers/timing.o $(LDFLAGS)

s2.o: s2.cpp s2.h
	$(CXX) $(CXXFLAGS) -c s2.cpp -o s2.o $(LDFLAGS)

s2: s2.o $(H_OBJS)
	$(CXX) s2.o $(H_OBJS) -o s2 $(LDFLAGS)

clean:
	rm -f s2 *~ *.o *#* helpers/*~ helpers/*#* + helpers/*.o

