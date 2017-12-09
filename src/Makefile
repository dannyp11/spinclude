# Unit test data
INCLUDES = .
MAIN_FILE = spinclude.cpp

# Compiler flags -------------------------------------------------
CXX ?=g++
DEBUG = -Os
CFLAGS = -Wall -Wextra -Werror $(DEBUG) -std=c++11
IFLAGS = $(foreach d, $(INCLUDES), -I$d)
LDFLAGS = -pthread -rdynamic
ARCHFLAGS = 
# Compiler flags ends ---------------------------------------------

# Config build structure ##########################################
BIN ?= spinclude
SRC = $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)
# Config build structure end ######################################

.PHONY: all

all: $(BIN)
	
clean:
	-rm -f $(OBJS) $(BIN)

# Build code #######################################
%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(IFLAGS) $(ARCHFLAGS) $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJS) $(IFLAGS) $(ARCHFLAGS) -o $@

include gtest.mk
