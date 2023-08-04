bins := mkfifo1.out mkfifo2.out
pipes := .pipe_1_to_2 .pipe_2_to_1

.PHONY: all clean

all: $(bins)

%.out: %.c
	gcc $< -o $@

clean:
	rm $(pipes)
