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

CXX=g++

CFLAGS=-std=c++11 -Wall -DNDEBUG -O3 -fPIC
export CFLAGS

#-----------------------------------------------------------------------#
#- REQUIRED TOP RULES --------------------------------------------------#
#-----------------------------------------------------------------------#

all: $(TARGET)

clean:
	rm -rf $(DIR)
	rm -f *.o *.a *.tgz *.dylib *.so

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
	$(CXX) $(CFLAGS) -g -I. -o $@ -dynamiclib -install_name $@ -o $@ $^

# shared library for Linux
lib$(NAME).so: ipasir$(NAME)glue.o libipasir$(SIG).a
	$(CXX) $(CFLAGS) -g -I. -o $@ -shared -Wl,-soname,$@ $^

# static library with the glue code
libipasir$(SIG).a: lib$(SIG).a ipasir$(NAME)glue.o
	@#
	@# merge library and glue code into target
	@#
	cp lib$(SIG).a $@
	ar r $@ ipasir$(NAME)glue.o

lib$(SIG).dylib: libipasir$(SIG).a ipasir$(NAME)glue.o
	$(CXX) $(CFLAGS) -g -I. -dynamiclib -install_name lib$(NAME).dylib -o $@ $^

lib$(SIG).a: $(DIR) $(DIR).patch
	@# apply patch for set_terminate and learn_callback
	@#
	patch -p0 < $(DIR).patch
	@#
	@# patch verbose output
	@#
	sed -i -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/core/Solver.cc
	sed -i -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/simp/SimpSolver.cc
	sed -i -e 's,"\([=|]\),"c [${SIG}] \1,' $(DIR)/simp/Main.cc
	@#
	@# configure and build library
	@#
	cd $(DIR)/simp; make -j4 VERBOSE=1 libr
	@#
	@# merge library and glue code into target
	@#
	cp $(DIR)/simp/lib_release.a $@

$(DIR): $(DIR).tgz
	@#
	@# extract library
	@#
	tar xvf $(DIR).tgz

$(DIR).tgz:
	@#
	@# download library
	@#
	wget "https://www.labri.fr/perso/lsimon/downloads/softwares/$(DIR).tgz"

#-----------------------------------------------------------------------#
#- LOCAL GLUE RULES ----------------------------------------------------#
#-----------------------------------------------------------------------#

ipasir$(NAME)glue.o: ipasir$(NAME)glue.cc ipasir.h makefile lib$(SIG).a
	$(CXX) -std=c++11 $(CFLAGS) \
	  -DVERSION=\"$(VERSION)\" \
	  -I$(DIR) -I$(DIR)/core -c ipasir$(NAME)glue.cc

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
