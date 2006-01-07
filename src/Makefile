OBJ = Singleton.o SmallObj.o OrderedStatic.o
BIN = ../lib/libloki.a
CXXFLAGS = -I../include -Wall -O2 -DNDEBUG

$(BIN): $(OBJ)
	ar rs $(BIN) $(OBJ)

.PHONY: clean install
install: $(BIN)
	mkdir -p $(DESTDIR)/usr/lib
	install $(BIN) $(DESTDIR)/usr/lib

clean:
	rm -f $(OBJ) $(BIN)
