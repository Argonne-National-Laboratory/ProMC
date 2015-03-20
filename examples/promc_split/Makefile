# Random access example
# S.Chekanov (ANL). June 2013 

ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif


include ${PROMC}/etc/config.mk
OPT2          = -g
CXXFLAGS      = $(OPT2) $(CPP_FLAGS)
LD            = $(CXX) 
LDFLAGS       = $(OPT2)
OutPutOpt     = -o  
LIBS          = -L$(PROMC)/lib -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz

INCLUDE1= -I./src
INCLUDE2= -I$(PROMC)/include

%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) -o $@ -c $<

Tasks:     clean prepare promc_split

prepare:
	@cp -rf $(PROMC)/src .
	echo "Prepare the ProMC default layout in src/";

COPY := $(shell cp -rf ${PROMC}/src .)

SOURCE_FILES1 := $(shell ls -1 promc_split.cc src/*.cc)

# build object files 
objects1       = $(patsubst %.cc,%.o,$(SOURCE_FILES1))

LIBOBJS = $(patsubst %.cc,%.o,$(SOURCE_FILES))


promc_split: $(objects1)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@

clean:
	        @rm -f *.o *~ promc_split; rm -fr src; rm -f src/*.o;  echo "Clear.." 
