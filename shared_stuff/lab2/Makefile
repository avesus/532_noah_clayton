CFLAGS=-c -g -O0 -std=c++11 -Wall -Werror
LDFLAGS=-lpthread

# name of c file containing main method
MAIN=main
SDIR=.
ODIR=.obj
TEST_FOLDER=test

SRC=$(shell find $(SDIR) -type f -name '*.cpp')
OBJ=$(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(SRC))
DEP=$(wildcard $(SDIR)/*.h)


CC=g++ -MMD -MP
EXE=lab2

DIRS=$(shell find $(SDIR) -type d)
OBJDIRS=$(patsubst $(SDIR)/%,$(ODIR)/%,$(DIRS))

$(shell mkdir -p $(ODIR))
$(shell mkdir -p $(OBJDIRS))

.PHONY: all

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS) $(LDFLAGS)
	cp play_files/*.txt .


$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@ 

-include $(wildcard .obj/*.d)

.PHONY: clean
clean:
	rm -f *~
	rm -f *.txt
	find $(ODIR) -type f -name '*.[od]' -delete
	rm -f $(EXE) $(TEXES)

