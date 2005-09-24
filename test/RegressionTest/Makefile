

CPP = g++
CC = gcc
OBJ = Test.o 
LINKOBJ = Test.o
CXXINCS = -I./../../include
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

Test.o: Test.cpp
	$(CPP) -c Test.cpp -o Test.o $(CXXFLAGS)

