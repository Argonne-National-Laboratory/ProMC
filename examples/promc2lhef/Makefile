# ProMC to HepMC converter
# S.Chekanov (ANL). June 2013 

ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif



include ${PROMC}/etc/config.mk
OPT2          = -g
CXXFLAGS      = $(OPT2) $(CPP_FLAGS)
LD            = $(CXX) 
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
OutPutOpt     = -o  
LIBS         += -L$(PROMC)/lib -lpromc -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz


INCLUDE1= -I./src
INCLUDE2= -I. -I$(HEPMC_PREFIX)/include
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src/

Tasks:     clean promc2lhef 

SOURCE_FILES := $(shell ls -1f promc2lhef.cc)
objects       = $(patsubst %.cc,%.o,$(SOURCE_FILES))

%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<


promc2lhef: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ promc2lhef; echo "Clear.." 
