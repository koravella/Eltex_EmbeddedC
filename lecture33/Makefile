BINS1 := server_systemv client_systemv
BINS2 := server_posix client_posix
BINS3 := server client
HEADERS1 := systemv.h
HEADERS2 := posix.h
HEADERS3 := declarations.h
LIBS2 := -lrt
LIBS3 := -pthread -lncurses

.PHONY: all

all: $(addsuffix .out, $(BINS1) $(BINS2) $(BINS3))

%_systemv.out: %_systemv.c $(HEADERS1)
	gcc $< -o $@ $(CFLAGS)

%_posix.out: %_posix.c $(HEADERS2)
	gcc $< -o $@ $(CFLAGS) $(LIBS2)

%.out: %.c $(HEADERS3)
	gcc $< -o $@ $(CFLAGS) $(LIBS3)

clean:
	rm $(addsuffix .out, $(BINS1) $(BINS2) $(BINS3))
