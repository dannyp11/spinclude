# Unit test data
INCLUDES = .
MAIN_FILES = spinclude.cpp tarjan-util.cpp

# Compiler flags -------------------------------------------------
CXX ?=g++
DEBUG = -Os
CFLAGS = -Wall -Wextra -Werror $(DEBUG) -std=c++11
IFLAGS = $(foreach d, $(INCLUDES), -I$d)
LDFLAGS = -rdynamic
ARCHFLAGS = 
# Compiler flags ends ---------------------------------------------

# Config build structure ##########################################
BINS        := $(patsubst %.cpp,%,$(MAIN_FILES))
SRC         = $(filter-out $(MAIN_FILES), $(wildcard *.cpp))
BIN_OBJS    = $(MAIN_FILES:.cpp=.o)
OBJS        = $(SRC:.cpp=.o)
# Config build structure end ######################################

.PHONY: all

all: 
	$(shell xxd -i DefaultProjectDescription.cfg > _default_proj_cfg.h)
	@$(MAKE) $(BIN_OBJS)
	@$(MAKE) $(BINS)
	
clean:
	-rm -f $(OBJS) $(BINS) $(BIN_OBJS) _default_proj_cfg.h

# Build code #######################################
%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(IFLAGS) $(ARCHFLAGS) $< -o $@
	
%:%.o $(BIN_OBJS) $(OBJS)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJS) $< $(IFLAGS) $(ARCHFLAGS) -o $@

include gtest.mk
