BINS := $(patsubst %.c, %, $(wildcard *.c))
CFLAGS := ${CFLAGS} -fno-stack-protector -no-pie

.PHONY: all

all: $(addsuffix .out, $(BINS))

%.out: %.c
	gcc $< -o $@ $(CFLAGS)

clean:
	rm $(addsuffix .out, $(BINS))
