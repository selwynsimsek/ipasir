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

SIG		= $(NAME)$(VERSION)
DIR		= $(NAME)-$(VERSION)
TARGET	?= dlib

#-----------------------------------------------------------------------#

# Make sets default value for CC, we want to override them
# but not if it set by the user
# see: https://stackoverflow.com/a/42958970
ifeq ($(origin CC),default)
export CC 		= gcc
endif

export CXX 		?= g++
export AR		?= ar
export RANLIB	?= ranlib
export MAKE		?= make
export CFLAGS	?= -Wall -DNDEBUG -O3 -fPIC

#-----------------------------------------------------------------------#
#- REQUIRED TOP RULES --------------------------------------------------#
#-----------------------------------------------------------------------#

all: $(TARGET)

clean:
	rm -rf $(DIR)
	rm -f *.o *.a *.dylib *.so *.dll

#-----------------------------------------------------------------------#
#- INVISIBLE INTERNAL SUB RULES ----------------------------------------#
#-----------------------------------------------------------------------#

# enable verbatim output
ifeq ($(VERB),)
VB=@
else
VB=
endif

# cross-compilation
ifeq ($(CHOST),)
else
	CROSS_PREFIX = $(CHOST)-
	CXX := $(CROSS_PREFIX)$(CXX)
	CC := $(CROSS_PREFIX)$(CC)	
	AR := $(CROSS_PREFIX)$(AR)
	RANLIB := $(CROSS_PREFIX)$(RANLIB)
	UNAME = $(shell echo "$(CHOST)" | sed -e 's/^[^-]*-\([^-]*\)\s*$$/\1/' -e 's/^[^-]*-[^-]*-\([^-]*\)\s*$$/\1/' -e 's/^[^-]*-[^-]*-\([^-]*\)-.*$$/\1/')
endif

# determine operating system name
UNAME ?= $(shell uname -s)

# the location of this makefile (independent of the current directory)
MROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ifeq ($(UNAME), Darwin)
  DLIB = lib$(NAME).dylib
else ifeq ($(UNAME), Linux)
  DLIB = lib$(NAME).so
else ifeq ($(UNAME), mingw32)
  DLIB = $(NAME).dll  
  CFLAGS += -DNGETRUSAGE -DNALLSIGNALS -Wextra
else
  $(error Platform not supported: $(UNAME))
endif 

dlib: $(DLIB)

# shared library for Mac OS
lib$(NAME).dylib: ipasir$(NAME)glue.o libipasir$(SIG).a
	$(VB) $(CXX) $(CFLAGS) -g -I. -o $@ -dynamiclib -install_name $@ -o $@ $^

# shared library for Linux
lib$(NAME).so: ipasir$(NAME)glue.o libipasir$(SIG).a
	$(VB) $(CXX) $(CFLAGS) -g -I. -o $@ -shared -Wl,-soname,$@ $^

# shared library for MinGW
$(NAME).dll: ipasir$(NAME)glue.o libipasir$(SIG).a	
	$(VB) $(CXX) $(CFLAGS) -static-libgcc -static-libstdc++ -g -I. -o $@ -shared -Wl,--subsystem,windows $^

# static library with the glue code
libipasir$(SIG).a: lib$(SIG).a ipasir$(NAME)glue.o
	@#
	@# merge library and glue code into target
	@#
	$(VB) cp lib$(SIG).a $@
	$(VB) $(AR) r $@ ipasir$(NAME)glue.o

# static solver library
lib$(SIG).a: $(DIR) $(LIBS)	
	@#
	@# configure and build library
	@#
	$(VB) cd $(DIR); ./configure -O --no-trace
	$(VB) make -C $(DIR)
	$(VB) cp $(DIR)/lib$(NAME).a $@

$(DIR): $(DIR).tar.gz
	@#
	@# extract library
	@#
	$(VB) tar xf $(DIR).tar.gz
	@#
	@# apply patch for mingw
	@#
ifeq ($(UNAME), mingw32)
	$(VB) patch -p0 < $(DIR).mingw.patch
endif

#-----------------------------------------------------------------------#
#- LOCAL GLUE RULES ----------------------------------------------------#
#-----------------------------------------------------------------------#

ipasir$(NAME)glue.o: ipasir$(NAME)glue.c ipasir.h makefile lib$(SIG).a
	$(VB) $(CC) $(CFLAGS) \
	  -DVERSION=\"$(VERSION)\" \
	  -I$(DIR) -c ipasir$(NAME)glue.c

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
