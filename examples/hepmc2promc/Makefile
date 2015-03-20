# S.Chekanov


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
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src/

Tasks:     clean hepmc2promc

SOURCE_FILES := $(shell ls -1 hepmc2promc.cc)

# build object files 
objects       = $(patsubst %.cc,%.o,$(SOURCE_FILES))

%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

hepmc2promc: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ hepmc2promc; echo "Clear.." 
