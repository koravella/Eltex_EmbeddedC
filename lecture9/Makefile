BINS := $(patsubst %.c, %, $(wildcard *.c))

.PHONY: all

all: $(addsuffix .out, $(BINS))

%.out: %.c
	gcc $< -o $@ $(CFLAGS)

clean:
	rm $(addsuffix .out, $(BINS))
