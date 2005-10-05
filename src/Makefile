

CPP = g++ 
CC = gcc 
OBJ = Singleton.o SmallObj.o OrderedStatic.o
LINKOBJ = Singleton.o SmallObj.o OrderedStatic.o
LIBS = 
CXXINCS = -I..\include 
BIN = ../lib/libloki.a
CXXFLAGS = $(CXXINCS) -O2 -DNDEBUG
CFLAGS = $(INCS) -g3
RM = rm -f
CHK_DIR_EXISTS= if not exist
MKDIR = mkdir

.PHONY: all all-before all-after clean clean-custom

all: all-before ../lib/libloki.a all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(LINKOBJ)
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)

check_tmp: 
	@$(CHK_DIR_EXISTS) $(MKDIR) tmp

check_lib: 
	@$(CHK_DIR_EXISTS) ..\lib\ $(MKDIR) ..\lib\
	
Singleton.o: Singleton.cpp
	$(CPP) -c Singleton.cpp -o Singleton.o $(CXXFLAGS)

SmallObj.o: SmallObj.cpp
	$(CPP) -c SmallObj.cpp -o SmallObj.o $(CXXFLAGS)
	
OrderedStatic.o: OrderedStatic.cpp
	$(CPP) -c OrderedStatic.cpp -o OrderedStatic.o $(CXXFLAGS)
	
