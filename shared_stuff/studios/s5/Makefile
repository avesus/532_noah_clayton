CXX=g++
CXXFLAGS=-O0 -g -D_GNU_SOURCE -std=c++11
LDFLAGS=-lpthread -lm

H_OBJS=helpers/locks.o helpers/temp.o helpers/opt.o helpers/util.o helpers/arg.o helpers/bits.o helpers/timing.o active_object.o synchronized_list.o

all: s5

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

synchronized_list.o: synchronized_list.cpp synchronized_list.h
	$(CXX) $(CXXFLAGS) -c synchronized_list.cpp -o synchronized_list.o $(LDFLAGS)

active_object.o: active_object.cpp active_object.h
	$(CXX) $(CXXFLAGS) -c active_object.cpp -o active_object.o $(LDFLAGS)

s5.o: s5.cpp s5.h
	$(CXX) $(CXXFLAGS) -c s5.cpp -o s5.o $(LDFLAGS)

s5: s5.o $(H_OBJS)
	$(CXX) $(CXXFLAGS) s5.o $(H_OBJS) -o s5 $(LDFLAGS)

clean:
	rm -f s5 *~ *.o *#* helpers/*~ helpers/*#* + helpers/*.o

