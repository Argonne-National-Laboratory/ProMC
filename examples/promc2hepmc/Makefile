# ProMC to HepMC converter
# S.Chekanov (ANL). June 2013 

ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif

ifndef HEPMC
$(error HEPMC env variable is not set. Run setup.sh first)
endif


# set correct location of HEPMC library!
HEPMC_PREFIX =$(HEPMC)


include ${PROMC}/etc/config.mk
OPT2          = -g
CXXFLAGS      = $(OPT2) $(CPP_FLAGS)
LD            = $(CXX) 
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
OutPutOpt     = -o  
LIBS         += -L$(PROMC)/lib -lpromc -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz
LIBS         += -L$(HEPMC_PREFIX)/lib -lHepMC

INCLUDE1= -I./src
INCLUDE2= -I. -I$(HEPMC_PREFIX)/include
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src

Tasks:     clean promc2hepmc

SOURCE_FILES1 := $(shell ls -1f promc2hepmc.cc)
# build object files 
objects1       = $(patsubst %.cc,%.o,$(SOURCE_FILES1))

%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

LIBOBJS = $(patsubst %.cc,%.o,$(SOURCE_FILES))

promc2hepmc: $(objects1)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ promc2hepmc; rm -rf src;  echo "Clear.." 
