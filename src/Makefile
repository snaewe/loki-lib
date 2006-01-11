OBJ = Singleton.o SmallObj.o OrderedStatic.o SafeFormat.o
BIN = ../lib/libloki.a
CXXFLAGS = -Wall -O2
CPPFLAGS = -I../include -DNDEBUG

$(BIN): $(OBJ)
	ar rs $(BIN) $(OBJ)

.PHONY: install clean-tmp clean
install: $(BIN)
	mkdir -p $(DESTDIR)/usr/lib
	install $(BIN) $(DESTDIR)/usr/lib

clean-tmp:
	rm -f $(OBJ)

clean: clean-tmp
	rm -f $(BIN)
