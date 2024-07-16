# preparing different actions depending of the OS 
UNAME := $(shell uname)


SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

ifeq ($(UNAME), Darwin)
CXXFLAGS := -std=c++17 -g -O3 -march=native 
LIBS = -I/opt/homebrew/Cellar/eigen/3.4.0_1/lib/  #-I/opt/homebrew/Cellar/boost/1.82.0_1/include/ -I/Users/silouane/Documents/code/libs/boost/1.72.0 
LINKS = #-I/opt/homebrew/include -L/opt/homebrew/lib #-lboost_filesystem
endif
ifeq ($(UNAME), Linux)
CXXFLAGS := -std=c++11 -g -O3 -march=native
LIBS = -I/usr/include -L/usr/lib
# LINKS = -lboost_system -L/home/silouane/Documents/logiciels/SALOME-9.2.0-UB18.04-SRC/BINARIES-UB18.04/boost/lib/ -lboost_filesystem
LINKS = 
endif

# ADD MORE WARNINGS!
WARNING := -Wall -Wextra -pedantic

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean


all: prog
ifeq ($(UNAME), Darwin)
	caffeinate gtime -v ./prog config_uchi.json
	# time ./prog
endif
ifeq ($(UNAME), Linux)
	/usr/bin/time -v ./prog 
endif

clean:
	$(RM) $(OBJECTS) $(DEPENDS) prog

# Linking the executable from the object files
prog: $(OBJECTS)
	$(CXX) $(WARNING) $(CXXFLAGS) $(LIBS) $(LINKS) $^ -o $@

-include $(DEPENDS)

%.o: %.cpp Makefile
	$(CXX) $(WARNING) $(CXXFLAGS) $(LIBS) -MMD -MP -c $< -o $@
