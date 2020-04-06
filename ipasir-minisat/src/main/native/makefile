#-----------------------------------------------------------------------#
#- GLOBAL DEFS ---------------------------------------------------------#
#-----------------------------------------------------------------------#

# Keep this as generic as possible.

NAME=minisat

# Avoid hardcoding the version number in this file.  This allows us to just
# drop a new distribution package into this directory without changing
# anything else. For other more complicated set-ups / solvers something like
# 'VERSION=960' should also work, if you have the appropriate tar file
# available, e.g., 'picosat-960.tar.gz', which extracts into 'picosat-960'.

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

CXX=g++
CXXFLAGS=-Wall -DNDEBUG -O3 -fPIC
export CXXFLAGS

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

# enable verbatim output
ifeq ($(VERB),)
VB=@
else
VB=
endif

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
	$(VB) $(CXX) $(CXXFLAGS) -g -I. -o $@ -dynamiclib -install_name $@ -o $@ $^

# shared library for Linux
lib$(NAME).so: ipasir$(NAME)glue.o libipasir$(SIG).a
	$(VB) $(CXX) $(CXXFLAGS) -g -I. -o $@ -shared -Wl,-soname,$@ $^

# static library with the glue code
libipasir$(SIG).a: lib$(SIG).a ipasir$(NAME)glue.o
	@#
	@# merge library and glue code into target
	@# 
	$(VB) cp lib$(SIG).a $@
	$(VB) ar r $@ ipasir$(NAME)glue.o

# static solver library
lib$(SIG).a:
	@#
	@# extract library
	@#
	$(VB) rm -rf $(DIR)
	$(VB) tar xvf $(DIR).tar.gz
	@#
	@# apply patch for set_terminate and learn_callback
	@#
	$(VB) patch -p0 < $(DIR).patch
	@#
	$(VB) @# patch verbose output
	@#
	$(VB) sed -i -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/minisat/core/Main.cc
	$(VB) sed -i -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/minisat/core/Solver.cc
	@#
	@# configure and build library
	@#
	$(VB) make -C $(DIR) lr
	$(VB) cp $(DIR)/build/release/lib/lib$(NAME).a $@

#-----------------------------------------------------------------------#
#- LOCAL GLUE RULES ----------------------------------------------------#
#-----------------------------------------------------------------------#

ipasir$(NAME)glue.o: ipasir$(NAME)glue.cc ipasir.h makefile lib$(SIG).a
	$(VB) $(CXX) $(CXXFLAGS) \
	  -DVERSION=\"$(VERSION)\" \
	  -I$(DIR) -I$(DIR)/minisat/core -c ipasir$(NAME)glue.cc

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
