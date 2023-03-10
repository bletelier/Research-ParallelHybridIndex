SDSL_DIR=/home/bletelier
RMQ_DIR=/home/bletelier/rmq


CPP=g++
CPPFLAGS=-std=c++11 -O3 -DNDEBUG -march=native -fopenmp
INCLUDES=-I$(SDSL_DIR)/include/ -I$(RMQ_DIR)/includes/
LIB=$(SDSL_DIR)/lib/libsdsl.a $(SDSL_DIR)/lib/libdivsufsort.a $(SDSL_DIR)/lib/libdivsufsort64.a $(RMQ_DIR)/rmqrmmBP.a
OBJECTS=HybridSelfIndex.o
BINS=build_index

all: clean stats $(OBJECTS) $(BINS)

HybridSelfIndex.o: HybridSelfIndex.cpp
	@$(CPP) $(CPPFLAGS) $(INCLUDES) -c HybridSelfIndex.cpp
	@echo " [BLD] Building binary loadDL_hsi"
	@$(CPP) $(CPPFLAGS) $(INCLUDES) -o load_hsi load_hsi.cpp $(OBJECTS) $(LIB)

stats:
	@echo
	@echo " COMPILING hsi.a"
	@echo " ###################"

	@echo "  * Compiler flags: $(CPPFLAGS)"
	@echo "  * Include dirs: $(INCLUDES)"
	@echo "  * Lib dirs: $(LIB)"
	@echo

clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(BINS)

build_index: 
	@echo " [BLD] Building hsi.a"
	ar -rvcs hsi.a $(OBJECTS) $(LIB) 

build_binary:
	@echo " [BLD] Building binary build_hsi"
	@$(CPP) $(CPPFLAGS) $(INCLUDES) -o build_hsi build_hsi.cpp $(OBJECTS) $(LIB)

load_binary:
	@echo " [BLD] Building binary loadDL_hsi"
	@$(CPP) $(CPPFLAGS) $(INCLUDES) -o load_hsi load_hsi.cpp $(OBJECTS) $(LIB)


