

CPP = g++
CC = gcc
OBJ = SmallObjBench.o SmallObj.o Singleton.o 
LINKOBJ = SmallObjBench.o SmallObj.o Singleton.o 
CXXINCS = -I./../../include -I./../../include/loki 
CXXFLAGS = $(CXXINCS) -DNDEBUG -fexpensive-optimizations -O3 
BIN = main-gcc.exe
RM = rm -f
CHK_DIR_EXISTS= if not exist
MKDIR = mkdir

.PHONY: all all-before all-after clean clean-custom

all: clean all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o main-gcc.exe $(LIBS) 
	
check_tmp: 
	@$(CHK_DIR_EXISTS) "" $(MKDIR) "tmp"

SmallObjBench.o: SmallObjBench.cpp
	$(CPP) -c SmallObjBench.cpp -o SmallObjBench.o $(CXXFLAGS)

SmallObj.o: ../../src/SmallObj.cpp
	$(CPP) -c ../../src/SmallObj.cpp -o SmallObj.o $(CXXFLAGS)

Singleton.o: ../../src/Singleton.cpp
	$(CPP) -c ../../src/Singleton.cpp -o Singleton.o $(CXXFLAGS)