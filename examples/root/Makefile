# S.Chekanov


ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif

ifndef ROOTSYS 
$(error ROOTSYS env variable is not set. Install ROOT first)
endif


include ${PROMC}/etc/config.mk

include ${ROOTSYS}/test/Makefile.arch

# Root variables
ROOTCFLAGS    = $(shell root-config --nonew --cflags)
ROOTLIBS      = $(shell root-config --nonew --libs)
ROOTGTTLIBS   = $(shell root-config --nonew --glibs)
# Assign or add variables
CXXFLAGS     += $(ROOTCFLAGS)
LIBS         += $(ROOTLIBS)
LIBS         += -L${PROMC}/lib -lpromc -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz

INCLUDE1= -I./inc -I./
INCLUDE2= -I./src
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src

Tasks:     clean mydict promc2root


SOURCE_FILES := $(shell ls -1 promc2root.cc)
SOURCE_FILES += $(shell ls -1 src/*.cc)


# build object files 
objects       = $(patsubst %.cc,%.o,$(SOURCE_FILES))


%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<


mydict: inc/LParticle.h
	@rm -f src/LParticle_dict*
	@rm -f inc/LParticle_dict*
	@echo "Generating dictionary for LParticle"
	@rootcint src/LParticle_dict.cc -c inc/LParticle.h


LIBOBJS = $(patsubst %.cc,%.o,$(SOURCE_FILES))

lib:
	ar cr libDensitySearch.a $(LIBOBJS)


promc2root: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
clean:
	        @rm -f *.o  promc2root *~ src/*.o src/ProMC* src/H* inc/*~; echo "Clear.." 
