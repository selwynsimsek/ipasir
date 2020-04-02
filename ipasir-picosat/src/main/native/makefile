#-----------------------------------------------------------------------#
#- GLOBAL DEFS ---------------------------------------------------------#
#-----------------------------------------------------------------------#

# Keep this as generic as possible.

NAME=picosat

# Avoid hardcoding the version number in this file.  This allows us to just
# drop a new distribution package into this directory without changing
# anything else. For other more complicated set-ups / solvers something like
# 'VERSION=961' should also work, if you have the appropriate tar file
# available, e.g., 'picosat-961.tar.gz', which extracts into 'picosat-961'.

# NOTE: requires GNU make extension '$(shell ...)'.

VERSION=$(shell ls $(NAME)-*.tar.gz | tail -1 | \
		sed -e 's,$(NAME)-,,' -e 's,.tar.gz,,')

#-----------------------------------------------------------------------#
# Solver signatures have to be both valid file names and C symbols.
# Since Picosat uses a dash '-' for the package name, we have to
# differentiate between with (directory / package name) and without
# dash (the signature).

SIG=$(NAME)$(VERSION)
DIR=$(NAME)-$(VERSION)
TARGET=dlib

#-----------------------------------------------------------------------#

CC=gcc
CFLAGS=-Wall -DNDEBUG -O3 -fPIC
export CFLAGS

#-----------------------------------------------------------------------#
#- REQUIRED TOP RULES --------------------------------------------------#
#-----------------------------------------------------------------------#

all: $(TARGET)

clean:
	rm -rf $(DIR)
	rm -f *.o *.a *.dylib *.so

#-----------------------------------------------------------------------#
#- INVISIBLE INTERNAL SUB RULES ----------------------------------------#
#-----------------------------------------------------------------------#

# determine operating system name
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
  DLIB = lib$(NAME).dylib
else ifeq ($(UNAME_S), Linux)
  DLIB = lib$(NAME).so
else
  $(error Platform not supported: $(UNAME_S))
endif 

dlib: $(DLIB)

# shared library for Mac OS
lib$(NAME).dylib: ipasir$(NAME)glue.o libipasir$(SIG).a 
	$(CXX) $(CXXFLAGS) -g -I. -o $@ -dynamiclib -install_name $@ -o $@ $^

# shared library for Linux
lib$(NAME).so: ipasir$(NAME)glue.o libipasir$(SIG).a
	$(CXX) $(CXXFLAGS) -g -I. -o $@ -shared -Wl,-soname,$@ $^

# static library with the glue code
libipasir$(SIG).a: lib$(SIG).a ipasir$(NAME)glue.o
	@#
	@# merge library and glue code into target
	@# 
	cp lib$(SIG).a $@
	ar r $@ ipasir$(NAME)glue.o

# static solver library
lib$(SIG).a:
	@#
	@# extract library
	@#
	rm -rf $(DIR)
	tar xvf $(DIR).tar.gz
	@#
	@# configure and build library
	@#
	cd $(DIR); ./configure -O --no-trace
	make -C $(DIR)
	cp $(DIR)/lib$(NAME).a $@

#-----------------------------------------------------------------------#
#- LOCAL GLUE RULES ----------------------------------------------------#
#-----------------------------------------------------------------------#

ipasir$(NAME)glue.o: ipasir$(NAME)glue.c ipasir.h makefile lib$(SIG).a
	$(CC) $(CFLAGS) \
	  -DVERSION=\"$(VERSION)\" \
	  -I$(DIR) -c ipasir$(NAME)glue.c

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
