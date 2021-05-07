IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)
BINDIR=../bin
ODIR=obj
LIBS=-pthread

.PHONY: all
all: $(BINDIR)/main.out $(BINDIR)/signalManager.out

#header file
_DEPS = main.h signalManager.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

#object file
_OBJ = main.o signalManager.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
#executable file
$(BINDIR)/main.out: $(ODIR)/main.o
	$(CC) -o $@ $^ $(LIBS)
	
$(BINDIR)/signalManager.out: $(ODIR)/signalManager.o
	$(CC) -o $@ $^ $(LIBS)
	
#clean files
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(BINDIR)/*.out
