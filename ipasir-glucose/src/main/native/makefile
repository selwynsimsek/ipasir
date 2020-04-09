#-----------------------------------------------------------------------#
#- GLOBAL DEFS ---------------------------------------------------------#
#-----------------------------------------------------------------------#

# Keep this as generic as possible.

NAME=glucose-syrup
VERSION=4.1

#-----------------------------------------------------------------------#
# Solver signatures have to be both valid file names and C symbols.
# Since Picosat uses a dash '-' for the package name, we have to
# differentiate between with (directory / package name) and without
# dash (the signature).

SIG=$(NAME)$(VERSION)
DIR=$(NAME)-$(VERSION)
TARGET=dlib

#-----------------------------------------------------------------------#

export CXX 		?= g++
export AR		?= ar
export MAKE		?= make
export CFLAGS	?= -std=c++11 -Wall -DNDEBUG -O3 -fPIC
# zlib is not provided for mingw32
ZLIB_VERSION	?= 1.2.11

#-----------------------------------------------------------------------#
#- REQUIRED TOP RULES --------------------------------------------------#
#-----------------------------------------------------------------------#

all: $(TARGET)

clean:
	rm -rf $(DIR) zlib-$(ZLIB_VERSION)
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
	AR := $(CROSS_PREFIX)$(AR)
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
  CFLAGS += -I$(MROOT)/zlib-$(ZLIB_VERSION) -L$(MROOT)/zlib-$(ZLIB_VERSION)
  # MinGW does not ship with zlib
  LIBS = zlib-$(ZLIB_VERSION)  
else
  $(error Platform not supported: $(UNAME))
endif 

dlib: $(DLIB)

# shared library for Mac OS
lib$(NAME).dylib: ipasir$(NAME)glue.o libipasir$(SIG).a
	$(VB) $(CXX) $(CFLAGS) -g -I. -o $@ -dynamiclib -install_name $@ $^

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
	$(VB) cd $(DIR)/simp; $(MAKE) VERBOSE=1 libr
	$(VB) cp $(DIR)/simp/lib_release.a $@

$(DIR): $(DIR).tgz $(DIR).patch
	@#
	@# extract library
	@#
	tar xf $<
	@# apply patch for set_terminate and learn_callback
	@#
	$(VB) patch -p0 < $(DIR).patch
	@#
	@# apply patch for mingw
	@#
ifeq ($(UNAME), mingw32)
	$(VB) patch -p0 < $(DIR).mingw.patch
endif
	@#
	@# patch verbose output
	@#
	$(VB) sed -i.bak -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/core/Solver.cc
	$(VB) sed -i.bak -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/simp/SimpSolver.cc
	$(VB) sed -i.bak -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/simp/Main.cc

$(DIR).tgz:
	@#
	@# download library
	@#
	wget "https://www.labri.fr/perso/lsimon/downloads/softwares/$@"

# zlib
zlib-$(ZLIB_VERSION): zlib-$(ZLIB_VERSION).tar.gz
	tar xzf $<
	@#
	@# patch makefile for mingw
	@#
	$(VB) sed -i.bak -e s/"PREFIX ="/"PREFIX = $(CROSS_PREFIX)"/ zlib-$(ZLIB_VERSION)/win32/Makefile.gcc
	$(VB) cd zlib-$(ZLIB_VERSION); $(MAKE) -f win32/Makefile.gcc
		

zlib-$(ZLIB_VERSION).tar.gz:
	$(VB) wget http://zlib.net/zlib-$(ZLIB_VERSION).tar.gz
	

#-----------------------------------------------------------------------#
#- LOCAL GLUE RULES ----------------------------------------------------#
#-----------------------------------------------------------------------#

ipasir$(NAME)glue.o: ipasir$(NAME)glue.cc ipasir.h makefile lib$(SIG).a
	$(VB) $(CXX) -std=c++11 $(CFLAGS) \
	  -DVERSION=\"$(VERSION)\" \
	  -I$(DIR) -I$(DIR)/core -c ipasir$(NAME)glue.cc

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
