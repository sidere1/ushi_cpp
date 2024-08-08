# preparing different actions depending of the OS 
UNAME := $(shell uname)


SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

ifeq ($(UNAME), Darwin)
CXXFLAGS := -std=c++17 -g -O3 -march=native 
LIBS =  
LINKS = 
endif
# ifeq ($(UNAME), Linux)
# CXXFLAGS := -std=c++11 -g -O3 -march=native
# LIBS = -I/usr/include -L/usr/lib
# LINKS = 
# endif

WARNING := -Wall -Wextra -pedantic

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean


all: prog
ifeq ($(UNAME), Darwin)
	caffeinate time ./prog config_uchi.json
	# caffeinate gtime -v ./prog config_uchi.json
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
