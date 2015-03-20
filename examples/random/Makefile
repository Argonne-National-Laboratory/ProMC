ifndef PROMC
$(error PROMC env variable is not set. Run setup.sh first)
endif

all:
	make -f Makefile_writer
	make -f Makefile_reader
clean: 
	make -f Makefile_writer clean;
	make -f Makefile_reader clean;
	rm -f out/*
