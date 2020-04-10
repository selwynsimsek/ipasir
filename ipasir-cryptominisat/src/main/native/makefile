#-----------------------------------------------------------------------#
#- GLOBAL DEFS ---------------------------------------------------------#
#-----------------------------------------------------------------------#

# Keep this as generic as possible.

NAME=cryptominisat
VERSION=5.6.8

#-----------------------------------------------------------------------#
# Solver signatures have to be both valid file names and C symbols.
# Since Picosat uses a dash '-' for the package name, we have to
# differentiate between with (directory / package name) and without
# dash (the signature).

SIG=$(NAME)$(VERSION)
DIR=$(NAME)-$(VERSION)
TARGET	?= dlib

#-----------------------------------------------------------------------#

# Make sets default value for CC, we want to override them
# but not if it set by the user
# see: https://stackoverflow.com/a/42958970
ifeq ($(origin CC),default)
export CC 		= gcc
endif
ifeq ($(origin CXX),default)
export CXX		= g++
endif
export AR		?= ar
export CFLAGS	?= -Wall -DNDEBUG -O3 -fPIC

#-----------------------------------------------------------------------#
#- REQUIRED TOP RULES --------------------------------------------------#
#-----------------------------------------------------------------------#

all: $(TARGET)

clean:
	rm -rf $(DIR)
	rm -f *.o *.a *.tar.gz *.dylib

#-----------------------------------------------------------------------#
#- INVISIBLE INTERNAL SUB RULES ----------------------------------------#
#-----------------------------------------------------------------------#

# enable verbatim output
ifeq ($(VERB),)
VB=@
else
VB=
endif

# the location of this makefile (independent of the current directory)
MROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# cross-compilation
ifneq ($(CHOST),)
	CROSS_PREFIX = $(CHOST)-
	CC := $(CROSS_PREFIX)$(CC)
	CXX := $(CROSS_PREFIX)$(CXX)
	# prefer -posix version if there are any
	ifneq ($(shell command -v "$(CC)-posix"),)
		CC := $(CC)-posix
		CXX := $(CXX)-posix 
	endif	
	AR := $(CROSS_PREFIX)$(AR)
	UNAME := $(shell echo "$(CHOST)" | sed -e 's/^[^-]*-\([^-]*\)\s*$$/\1/' -e 's/^[^-]*-[^-]*-\([^-]*\)\s*$$/\1/' -e 's/^[^-]*-[^-]*-\([^-]*\)-.*$$/\1/')#						
endif

# determine operating system name
UNAME ?= $(shell uname -s)

# the build static library
CMSLIB = $(DIR)/build/lib/libcryptominisat5.a
# the build glue object file implementing the ipasir interface
IPASIRGLUEOBJ=$(DIR)/build/cmsat5-src/CMakeFiles/ipasircryptominisat5.dir/ipasir.cpp.o

ifeq ($(UNAME), Darwin)
 	DLIB = lib$(NAME).dylib
else ifeq ($(UNAME), Linux)
 	DLIB = lib$(NAME).so
else ifeq ($(UNAME), mingw32)
 	DLIB = $(NAME).dll  
 	CMAKE_OPTIONS=-D CMAKE_SYSTEM_NAME=Windows
 	# different file names for windows
 	CMSLIB = $(DIR)/build/lib/libcryptominisat5win.a  
 	IPASIRGLUEOBJ=$(DIR)/build/cmsat5-src/CMakeFiles/ipasircryptominisat5.dir/ipasir.cpp.obj  
else
 	$(error Platform not supported: $(UNAME))
endif 

dlib: $(DLIB)


# shared library for Mac OS
lib$(NAME).dylib: libipasir$(SIG).a
	$(VB) $(CXX) $(CFLAGS) -g -I. -o $@ $(IPASIRGLUEOBJ) $^ -dynamiclib -install_name $@ 

# shared library for Linux
lib$(NAME).so: libipasir$(SIG).a
	$(VB) $(CXX) -o $@ $(IPASIRGLUEOBJ) $^ $(CFLAGS) -g -I. -shared -Wl,-soname,$@ 

# shared library for MinGW
$(NAME).dll: libipasir$(SIG).a	 	
	$(VB) $(CXX) -o $@ $(IPASIRGLUEOBJ) $^ $(CFLAGS) -static-libgcc -static-libstdc++ -g -I. -shared -Wl,--subsystem,windows -Wl,-Bstatic -lstdc++ -lpthread

# static library with the glue code
libipasir$(SIG).a: lib$(SIG).a
	@#
	@# merge library and glue code into target
	@#
	$(VB) cp lib$(SIG).a $@
	$(VB) $(AR) r $@ $(IPASIRGLUEOBJ)

# static solver library
lib$(SIG).a: $(DIR) $(LIBS)
	@#
	@# configure and build library
	@#
	cd $(DIR); sh ../build.sh $(CMAKE_OPTIONS)	
	cp $(CMSLIB) $@

$(DIR): $(VERSION).tar.gz
	@#
	@# extract library
	@#
	$(VB) tar xf $<
	@#
	@# apply patch for set_terminate and learn_callback
	@#
	patch $(DIR)/src/ipasir.cpp < ipasir.cpp.patch

$(VERSION).tar.gz:
	@#
	@# download library
	@#
	wget "https://github.com/msoos/cryptominisat/archive/$@"

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
