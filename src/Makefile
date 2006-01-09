OBJ = Singleton.o SmallObj.o OrderedStatic.o
BIN = ../lib/libloki.a
CXXFLAGS = -Wall -O2
CPPFLAGS = -I../include -DNDEBUG

$(BIN): $(OBJ)
	ar rs $(BIN) $(OBJ)

.PHONY: clean install
install: $(BIN)
	mkdir -p $(DESTDIR)/usr/lib
	install $(BIN) $(DESTDIR)/usr/lib

clean:
	rm -f $(OBJ) $(BIN)
