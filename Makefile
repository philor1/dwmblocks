PREFIX ?= /usr/local
CC ?= cc
LDFLAGS = -lX11

output: dwmblocks.c config.def.h config.h
	${CC} dwmblocks.c $(LDFLAGS) -o dwmblocks
	${CC} -o internetav internetav.c
config.h:
	cp config.def.h $@


clean:
	rm -f *.o *.gch dwmblocks internetav
install: output
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 0755 dwmblocks $(DESTDIR)$(PREFIX)/bin/dwmblocks
	install -m 0755 internetav $(DESTDIR)$(PREFIX)/bin/internetav
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dwmblocks
	rm -f $(DESTDIR)$(PREFIX)/bin/internetav
