#-----------------------------------------------------------------------#
#- GLOBAL DEFS ---------------------------------------------------------#
#-----------------------------------------------------------------------#

# Keep this as generic as possible.

NAME=cadical
VERSION=${library.version}
COMMIT=0f6674c67784d47dccf04dca10976636807702d7

#-----------------------------------------------------------------------#
# Solver signatures have to be both valid file names and C symbols.
# Since Picosat uses a dash '-' for the package name, we have to
# differentiate between with (directory / package name) and without
# dash (the signature).

SIG=$(NAME)$(VERSION)$(COMMIT)
DIR=$(NAME)-$(COMMIT)
TARGET	?= dlib

#-----------------------------------------------------------------------#

# Make sets default value for CXX, we want to override them
# but not if it set by the user
# see: https://stackoverflow.com/a/42958970
ifeq ($(origin CXX),default)
export CXX		= g++
endif
export AR		?= ar
export CXXFLAGS	?= -Wall -DNDEBUG -O3 -fPIC

#-----------------------------------------------------------------------#
#- REQUIRED TOP RULES --------------------------------------------------#
#-----------------------------------------------------------------------#

all: $(TARGET)

clean:
	rm -rf $(DIR)
	rm -f *.o *.a *.tar.gz *.dylib *.dll *.so

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
	CXX := $(CROSS_PREFIX)$(CXX)
	# prefer -posix version if there are any
	ifneq ($(shell command -v "$(CXX)-posix"),)
		CXX := $(CXX)-posix 
	endif	
	AR := $(CROSS_PREFIX)$(AR)
	export SKIP_EXEC=1
	UNAME := $(shell echo "$(CHOST)" | sed -e 's/^[^-]*-\([^-]*\)\s*$$/\1/' -e 's/^[^-]*-[^-]*-\([^-]*\)\s*$$/\1/' -e 's/^[^-]*-[^-]*-\([^-]*\)-.*$$/\1/')#						
endif

# determine operating system name
UNAME ?= $(shell uname -s)

# the build glue object file implementing the ipasir interface
IPASIRGLUEOBJ=$(DIR)/build/ipasir.o

ifeq ($(UNAME), Darwin)
 	DLIB = lib$(NAME).dylib
else ifeq ($(UNAME), Linux)
 	DLIB = lib$(NAME).so
else ifeq ($(UNAME), mingw32)
 	DLIB = $(NAME).dll
 	CXXFLAGS := $(CXXFLAGS) -D_POSIX -D__USE_MINGW_ALARM 
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
lib$(SIG).a: $(DIR)
	@#
	@# configure and build library
	@#
	cd $(DIR); ./configure && make cadical	
	cp $(DIR)/build/libcadical.a $@


$(DIR): $(COMMIT).tar.gz
	@#
	@# extract library
	@#
	$(VB) tar xf $<

$(COMMIT).tar.gz:
	@#
	@# download library
	@#
	wget "https://github.com/liveontologies/cadical/archive/$@"

#-----------------------------------------------------------------------#

.FORCE:
.PHONY: all clean
