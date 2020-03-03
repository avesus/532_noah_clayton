CXX=g++
CXXFLAGS=-O0 -g -D_GNU_SOURCE -std=c++11
LDFLAGS=-lpthread -lm

H_OBJS=helpers/locks.o helpers/temp.o helpers/opt.o helpers/util.o helpers/arg.o helpers/bits.o helpers/timing.o 

all: s11

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

s11.o: s11.cpp s11.h
	$(CXX) $(CXXFLAGS) -c s11.cpp -o s11.o $(LDFLAGS)

s11: s11.o $(H_OBJS)
	$(CXX) $(CXXFLAGS) s11.o $(H_OBJS) -o s11 $(LDFLAGS)

clean:
	rm -f s11 *~ *.o *#* helpers/*~ helpers/*#* + helpers/*.o

