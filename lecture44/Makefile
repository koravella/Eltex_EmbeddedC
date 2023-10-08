BINS := server client
BINWIN := server_windows
LIBWIN := -lwsock32

.PHONY: all for_win

all: $(addsuffix .out, $(BINS))

for_win: $(addsuffix .exe, $(BINWIN))

%.out: %.c
	gcc $< -o $@ $(CFLAGS)

%.exe: %.c
	gcc $< -o $@ $(CFLAGS) $(LIBWIN)

clean:
	rm $(addsuffix .out, $(BINS))
