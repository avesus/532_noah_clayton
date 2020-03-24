all:
	g++ -L$(ACE_ROOT)/ace -Wall -std=c++11 -pthread -DTEMPLATE_HEADERS_INCLUDE_SOURCE -o aserver aserver.cpp -I$(ACE_ROOT) -lACE

clean:
	rm -f aserver *~

