# S.Chekanov (ANL)

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


INCLUDE1= -I./inc -I./
INCLUDE2= -I./src
INCLUDE3= -I$(PROMC)/include -I$(PROMC)/src

Tasks:     clean promc2hepevt

SOURCE_FILES := $(shell ls -1 promc2hepevt.cc)
objects       = $(patsubst %.cc,%.o,$(SOURCE_FILES))

%.o: %.cc
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

promc2hepevt: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
clean:
	        @rm -f *.o *~ promc2hepevt; echo "Clear.." 
