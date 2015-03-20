# S.Chekanov

ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif

include ${PROMC}/etc/config.mk
OPT2          = -g
CXXFLAGS      = $(OPT2) $(CPP_FLAGS)
LD            = $(CXX)
LDFLAGS       = $(OPT2)
OutPutOpt     = -o

LIBS  = -L$(PROMC)/lib -lpronlo -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz
INCLUDE1= -I./inc -I./
INCLUDE2= -I./src
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src

Tasks:     clean read_nlo

SOURCE_FILES := $(shell ls -1 read_nlo.cc)
objects       = $(patsubst %.cc,%.o,$(SOURCE_FILES))

%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

read_nlo: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
clean:
	        @rm -f *.o *~ read_nlo; rm -f *.promc; echo "Clear.." 
