# S.Chekanov

# define here PYTHIA and HEPMC directories
PYTHIADIR=../pythia8
HEPMC=/share/sl6/HepMC/


ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif

include ${PROMC}/etc/config.mk
include ${ROOTSYS}/etc/Makefile.arch


# Root variables
ROOTCFLAGS    = $(shell root-config --nonew --cflags)
ROOTLIBS      = $(shell root-config --nonew --libs)
ROOTGTTLIBS   = $(shell root-config --nonew --glibs)
CXXFLAGS     += $(ROOTCFLAGS)

LIBDIRARCH=lib/archive
OutPutOpt     = -o  
LIBS         += -L${PROMC}/lib -lpromc -lprotoc -lprotobuf -lprotobuf-lite -lcbook
LIBS         += -L$(PYTHIADIR)/$(LIBDIRARCH) -lpythia8 -lhepmcinterface -llhapdfdummy
LIBS         += -L$(HEPMC)/lib -lHepMC

SOURCE_FILES1 := $(shell ls -1 writer_pythia.cc)

INCLUDE1=-I./src
INCLUDE2=-I.
INCLUDE3=-I${PROMC}/include -I$(PROMC)/src
INCLUDE4=-I$(HEPMC)/include
INCLUDE5=-I$(PYTHIADIR)/include


# build object files 
objects1       = $(patsubst %.cc,%.o,$(SOURCE_FILES1))


%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) $(INCLUDE4) $(INCLUDE5) -o $@ -c $<

Tasks:     clean writer_pythia


LIBOBJS = $(patsubst %.cc,%.o,$(SOURCE_FILES))

writer_pythia: $(objects1)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ writer_pythia src/*.o ;  echo "Clear.." 
