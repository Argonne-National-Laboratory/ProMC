# S.Chekanov


ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif


# set correct location of HEPMC library!
HEPMC_PREFIX =$(HEPMC)


include ${PROMC}/etc/config.mk
OPT2          = -g
CXXFLAGS      = $(OPT2) $(CPP_FLAGS)
LD            = $(CXX) 
LDFLAGS       = $(OPT2)
OutPutOpt     = -o  
LIBS         += -L$(PROMC)/lib -lpromc -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz
LIBS         += -L../stdhep2promc/stdhep/lib -lstdhepC -lFmcfio

INCLUDE1= -I./src
INCLUDE2= -I../stdhep2promc/stdhep/mcfio -I../stdhep2promc/stdhep/src/inc
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src

Tasks:     clean promc2stdhep

SOURCE_FILES1 := $(shell ls -1 promc2stdhep.cc)

# build object files 
objects1       = $(patsubst %.cc,%.o,$(SOURCE_FILES1))


%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<
%.o: %.c
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

promc2stdhep: $(objects1)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ promc2stdhep;  echo "Clear.." 
