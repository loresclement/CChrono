# Nom du programme
PROG = chrono

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

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(PROG) $(OBJS)
