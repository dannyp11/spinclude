# TEST_SOURCES: source files to be tested
# TEST_HEADERS: header file for test cases in gtest framework, must be test/*gtest.h

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIRNAME := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
TOP :=$(shell dirname $(MKFILE_PATH))/..
TOP :=$(shell readlink -f $(TOP))

# compiler vars
TEST_AR ?=ar

# gtest library definition
GTEST_DIR = $(TOP)/submodules/googletest/googletest
GTEST_SRC = $(wildcard $(GTEST_DIR)/src/*.cc)
GTEST_LIB = $(GTEST_DIR)/gtest.a
TEST_CFLAGS += -isystem $(GTEST_DIR)/include $(CFLAGS)
GTEST_INCLUDES += $(GTEST_DIR)/include/gtest/ $(GTEST_DIR)/include/gtest/internal/  $(GTEST_DIR)/

GTEST_OBJS = $(patsubst %.cc, %.ogtest, $(GTEST_SRC))
GTEST_OBJS := $(filter-out %.cc, $(GTEST_OBJS))

# test objects definition
MAIN_FILE       ?= main.cpp
TEST_HEADERS    := $(wildcard test/*gtest.cpp) 
TEST_EXTRA_SRCS := $(filter-out $(TEST_HEADERS), $(wildcard test/*.cpp))
MAIN_OBJS	    = $(patsubst %.cpp, %.o, $(MAIN_FILE)) # exclude main() files
TEST_OBJECTS    := $(filter-out $(MAIN_OBJS),$(OBJS)) $(patsubst %.cpp, %.o, $(TEST_EXTRA_SRCS))
TEST_BIN_OBJS	=  $(patsubst %.cpp, %.ogtest, $(TEST_HEADERS))

TEST_BINARIES   = $(patsubst %.cpp, %.gtest, $(TEST_HEADERS))
TEST_IFLAGS	    = $(foreach d, $(GTEST_INCLUDES), -I$d) $(IFLAGS)

# main rules to run
##########################################################################################
TEST_GEN: $(GTEST_LIB)
	$(MAKE) test_compile -j4

check: TEST_GEN
	@printf "Running test binaries \n\n"
	for bin in $(TEST_BINARIES); do ./$$bin --gtest_print_time=0 || exit $? ; done # abort when fail
	@printf "\nDone running tests\n"
	$(MAKE) cleantest > /dev/null
##########################################################################################

test_compile: $(TEST_OBJECTS) $(TEST_BIN_OBJS) $(TEST_BINARIES)

%.gtest: %.ogtest
	$(CXX) $(TEST_CFLAGS) $(LDFLAGS) $(TEST_IFLAGS) $(TEST_OBJECTS) $< $(GTEST_LIB) -o $@

%.ogtest: %.cpp
	$(CXX) -c $(TEST_CFLAGS) $(TEST_IFLAGS) -o $@ $<

%.ogtest: %.cc
	$(CXX) -c $(TEST_CFLAGS) $(TEST_IFLAGS) -o $@ $<

$(GTEST_LIB): $(GTEST_OBJS)
	$(TEST_AR) r $(GTEST_LIB) $(GTEST_OBJS)
	
cleantest:
	rm -f $(TEST_OBJECTS) $(TEST_BIN_OBJS) $(TEST_BINARIES)
    # no need to remove GTEST_SRC and GTEST_LIB since we don't edit gtest src

