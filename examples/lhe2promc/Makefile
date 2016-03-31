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
LIBS          =  -L$(PROMC)/lib -lpromc -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz

INCLUDE1= -I./lhef
INCLUDE2= -I.
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src

Tasks:     prepare clean lhe2promc

prepare:
	rm -f proto; ln -s $(PROMC)/proto/promc proto

SOURCE_FILES1 := $(shell ls -1 lhe2promc.cc)

# build object files 
objects1       = $(patsubst %.cc,%.o,$(SOURCE_FILES1))


%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<
%.o: %.c
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

lhe2promc: $(objects1)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ lhe2promc; echo "Clear.." 
