# Author: S.Chekanov 

###### do not modify this file
include ./config.mk

# name 
MODULE = cbook
LIBDIR = ${DIR_CBOOK}/share/lib
#
#
# do EMC 
SOURCE_FILES := $(shell ls -1 ${DIR_CBOOK}/src/HCL/*.cpp)
SOURCE_FILES += $(shell ls -1 ${DIR_CBOOK}/src/zipios/*.cpp)
SOURCE_FILES += $(shell ls -1 ${DIR_CBOOK}/src/CBook/*.cpp)
LIBOBJS = $(patsubst %.cpp,%.o,$(SOURCE_FILES))

# CC files
SOURCE_CC = $(shell ls -1 ${DIR_CBOOK}/src/HRecord/*.cc)
LIBOBJS += $(patsubst %.cc,%.o,$(SOURCE_CC))
SOURCE_FILES += $(SOURCE_CC)





SUBDIRLOOK = $(sort $(dir $(SOURCE_FILES)))

# where to put the library, will need install-target later
EXECDIR = bin


# Fortran compile options
CPP_INCLUDEDIRS1 = -I${DIR_CURRENT}/share/include
CPP_INCLUDEDIRS2 = -I${DIR_CURRENT}/cbook/inc
CPP_INCLUDEDIRS3 = -I${DIR_CURRENT}/cbook/src
CPP_INCLUDEDIRS4 = -I${DIR_CURRENT}/cbook/inc/CBook


CPP_INCLUDEDIRS := $(CPP_INCLUDEDIRS1) $(CPP_INCLUDEDIRS2) $(CPP_INCLUDEDIRS3) $(CPP_INCLUDEDIRS4) 

# We need to compile directly against the following files
CPP_TIME := $(shell date +%H%M)
CPP_DATE := $(shell date +%d%m%y)
CPP_VERS := $(shell if [ -f CVS/Tag ] ; \
                    then cat CVS/Tag | sed -e "s/[a-zA-Z_]//g"; fi)
CPP_TITLE := "cbook_hcl"

ext = $(CPP_VERS)

#DEBUG = -g

export CPP_COMP CPP_INCLUDEDIRS CPP_TIME CPP_DATE CPP_VERS CPP_TITLE \
       DEFINES DEBUG CPP_FLAGS 


all: lib 

install: all
	( [ -d lib ] || mkdir lib ) && mv libbases$(ext).a lib 

objects: version makefiles 
	@for subdir in $(SUBDIRLOOK);\
	  do $(MAKE) -C $$subdir all;\
	done
clean: makefiles
	@rm -f  src/*/*.o
	@rm -rf src/zipios/.libs

clobber: makefiles
	for subdir in $(SUBDIRLOOK);\
	  do $(MAKE) -C $$subdir clobber;\
	done
	-@rm version.h qftitle.inc *~ *.a 2>/dev/null
version: version.h qftitle.inc
version.h::
	@if [ -f version.h ]; then rm version.h ; fi
	@echo >version.h "#define CPP_TIME $(CPP_TIME)"
	@echo >>version.h "#define CPP_DATE $(CPP_DATE)"
	@echo >>version.h "#define CPP_VERS $(CPP_VERS)"
	@echo >>version.h "#define CPP_TITLE $(CPP_TITLE)"
	@echo >>version.h "#define CPP_VERSS '$(CPP_VERS)'"
qftitle.inc::
	@if [ -f qftitle.inc ]; then rm qftitle.inc ; fi
	@echo >qftitle.inc "     *' $(CPP_TITLE)           CVS version: $(CPP_VERS) '"
makefiles: Makefile Makefile_sub
	for subdir in $(SUBDIRLOOK);\
	  do [ -f $$subdir/Makefile ] && rm $$subdir/Makefile;\
	  echo >$$subdir/Makefile "F_INCLUDEDIRS=$(CPP_INCLUDEDIRS)";\
	  echo >>$$subdir/Makefile "DEBUG=$(DEBUG)";\
	  cat Makefile_sub >>$$subdir/Makefile;\
	done


lib: lib$(MODULE)$(ext).a

lib$(MODULE)$(ext).a: objects
	ar -rv $(LIBDIR)/lib$(MODULE)$(ext).a $(LIBOBJS)
	ranlib $(LIBDIR)/lib$(MODULE)$(ext).a
doc:
	$(MAKE) -C manual all







