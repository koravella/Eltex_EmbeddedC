BINS := server client

.PHONY: all

all: $(addsuffix .out, $(BINS))

%.out: %.c
	gcc $< -o $@ $(CFLAGS)

clean:
	rm $(addsuffix .out, $(BINS))
