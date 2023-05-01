# Nom du programme
PROG = cchrono
VERSION = 1.0

# Options de compilation
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0` -lm

# Fichiers sources
SRCS = chrono.c

# Fichiers objets
OBJS = $(SRCS:.c=.o)

# Règles de compilation
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LIBS)
	mv $(PROG) ./usr/bin/$(PROG)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(PROG) $(OBJS)

# Règles d'installation
install:
	mkdir -p $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share/applications
	cp $(PROG) $(DESTDIR)/usr/bin/$(PROG)
	cp ./usr/share/applications/cchrono.desktop $(DESTDIR)/usr/share/applications/

# Règle de construction du paquet .deb
package:
	mkdir -p cchrono_$(VERSION)/DEBIAN
	cp DEBIAN/control cchrono_$(VERSION)/DEBIAN/
	dpkg-deb --build cchrono_$(VERSION)

copy:
	mv $(PROG) ./usr/bin/$(PROG)


# Règle de désinstallation
uninstall:
	rm -f $(DESTDIR)/usr/bin/$(PROG)
	rm -f $(DESTDIR)/usr/share/applications/cchrono.desktop
	rmdir --ignore-fail-on-non-empty $(DESTDIR)/usr/bin
	rmdir --ignore-fail-on-non-empty $(DESTDIR)/usr/share/applications
