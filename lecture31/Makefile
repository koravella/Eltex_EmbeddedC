TARGET := editor
ADDIT := buffer display
LIBS := -lncurses

.PHONY: all clean

all: $(TARGET).out

%.out: $(addsuffix .o, % $(ADDIT))
	gcc -o $@ $^ $(LIBS)

%.o: %.c
	gcc -c $<

clean:
	rm $(TARGET).out
