

CPP = g++
CC = gcc
OBJ = Factory.o 
LINKOBJ = Factory.o
CXXINCS = -I./../../include
LIBS = -L../../lib -lloki
CXXFLAGS = $(CXXINCS) -O2 -DNDEBUG
BIN = main-gcc.exe
RM = rm -f
CHK_DIR_EXISTS= if not exist
MKDIR = mkdir

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o main-gcc.exe $(LIBS)

check_tmp: 
	@$(CHK_DIR_EXISTS) "" $(MKDIR) "tmp"
	
Factory.o: Factory.cpp
	$(CPP) -c Factory.cpp -o Factory.o $(CXXFLAGS)

