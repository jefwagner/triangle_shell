######################################################################
# General Makefile
#
# This is a general makefile for a c project. You need to fill in the
# program name, the source files, and the dependency (header or
# resource) files. It should work with very minor changes in the
# pattern substitution for c++ and fortran files.
######################################################################
# Name of the program
PROGRAM = shell

# Directory names for sources, objects, ect.
SDIR = src
ODIR = obj
TDIR = test
DDIR = doc

# Use wild cards to get a list of sources
SRCS = $(wildcard $(SDIR)/*.c)
TEST_SRCS = $(wildcard $(TDIR)/*_test.c)
# Use pattern substitution to get list of objects and dependencies
OBJS = $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(SRCS))
DEPS = $(patsubst $(SDIR)/%.c, $(ODIR)/%.d, $(SRCS))
TESTS = $(patsubst %.c, %, $(TEST_SRCS)) 
TEST_OBJS = $(patsubst %.c, %.o, $(TEST_SRCS))
TEST_DEPS = $(patsubst %.c, %.d, $(TEST_SRCS))

# Choose the C, C++, and Fortran compiler
CC=gcc
CXX=g++
FC=gfortran

# Options for finding external libraries
LDFLAGS += -L/opt/local/lib
CFLAGS += -I/opt/local/include
#LDFLAGS += -L/usr/local/lib
#LDFLAGS += -L/usr/lib
#CFLAGS += -I/usr/local/include
#CFLAGS += -I/usr/include
CFLAGS += -I.

# Additional options
CFLAGS += -Wall
CFLAGS += -ggdb
CFLAGS += -O0
#CFLAGS += -O3

# Libraries
#LIBS += -lgsl
#LIBS += -lgslcblas
#LIBS += -llapack -lptcblas -latlas
#LIBS += -lnlopt
LIBS += -lm

# Default rule compiles the program from all object files
$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

# Include the dependencies
include $(DEPS)
include $(TEST_DEPS)

# Implicit rule for automatic dependency generation
$(ODIR)/%.d: $(SDIR)/%.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,$(ODIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

# Implicit rule for object generation
$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Implicit rule for test dependency generation
$(TDIR)/%_test.d: $(TDIR)/%_test.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)_test\.o[ :]*,$(TDIR)/\1_test.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

# Implicit rule for test generation and running
$(TDIR)/%_test: $(TDIR)/%_test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $< -o $@
#	@./$@

# Implicit rule for test object generation
$(TDIR)/%_test.o: $(TDIR)/%_test.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: all clean test docs

# Run the test, build the program, make the docs
all: test $(PROGRAM) docs

# 
clean:
	rm -f $(ODIR)/*
	rm -f $(TDIR)/*_test 
	rm -f $(TDIR)/*.o 
	rm -f $(TDIR)/*.d 
	rm -f $(PROGRAM)

test: $(TESTS)

#docs: 
#    doxygen
