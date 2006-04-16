
CXX       = g++
CXXFLAGS  = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2 -I../../include -DNDEBUG
LINK      = g++
LFLAGS    = -L../../lib
LIBS      = -lloki
DEL_FILE  = rm -f 

SOURCES = \
		main.cpp \
		strong.cpp
		
OBJECTS = \
		main.o \
		strong.o 
		
TARGET        = main.exe

####### Implicit rules

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

####### Build rules

first: all
all: $(TARGET)

$(TARGET):  $(OBJECTS) 
	$(LINK) $(LFLAGS) -o "$(TARGET)" $(OBJECTS)  $(LIBS)


clean: compiler_clean
	-$(DEL_FILE) main.o strong.o
	-$(DEL_FILE) "$(TARGET)"

compiler_clean: 



####### Compile

main.o: main.cpp 
	$(CXX) -c $(CXXFLAGS) -o main.o main.cpp

strong.o: strong.cpp 
	$(CXX) -c $(CXXFLAGS) -o strong.o strong.cpp

FORCE:
