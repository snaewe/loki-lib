DESTDIR = /usr
.PHONY: install
install:
	mkdir -p $(DESTDIR)/include/loki
	mkdir -p $(DESTDIR)/include/loki/flex
	mkdir -p $(DESTDIR)/include/loki/yasli
	
	install -m 644 loki/*.h $(DESTDIR)/include/loki
	install -m 644 loki/flex/*.h $(DESTDIR)/include/loki/flex
	install -m 644 loki/yasli/*.h $(DESTDIR)/include/loki/yasli
