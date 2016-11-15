F_INCLUDEDIRS=-I/users/chekanov/work/ProMC/ProMC/share/include -I/users/chekanov/work/ProMC/ProMC/cbook/inc -I/users/chekanov/work/ProMC/ProMC/cbook/src -I/users/chekanov/work/ProMC/ProMC/cbook/inc/CBook 
DEBUG=
include ../../config.mk

SOURCE_FILES_CPP = $(shell ls -1 *.cpp)
SOURCE_FILES_CC = $(shell ls -1 *.cc)

OBJS1 = $(patsubst %.cpp,%.o,$(SOURCE_FILES_CPP))
%.o: %.cpp
	$(CPP_COMP) $(CPP_FLAGS) $(DEBUG) $(DEFINES) $(CPP_INCLUDEDIRS) -o $(@F) $<

OBJS2 = $(patsubst %.cc,%.o,$(SOURCE_FILES_CC))
%.o: %.cc
	$(CPP_COMP) $(CPP_FLAGS) $(DEBUG) $(DEFINES) $(CPP_INCLUDEDIRS) -o $(@F) $<

all: $(OBJS1) $(OBJS2) 

clean:
	-rm  *.o 2>/dev/null
clobber:
	-rm *.o *~ 2>/dev/null
