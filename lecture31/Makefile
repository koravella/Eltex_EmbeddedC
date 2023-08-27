target := editor
addit := buffer display
libs := -lncurses

.PHONY: all clean

all: $(target).out

%.out: $(addsuffix .o, % $(addit))
	gcc -o $@ $^ $(libs)

%.o: %.c
	gcc -c $<

clean:
	rm $(addsuffix .o, $(target) $(addit))
