TARGET := editor
ADDIT := buffer display
LIBS := -lncurses
TARGET2 := file_operations

.PHONY: all clean

all: $(TARGET).out $(TARGET2).out

$(TARGET2).out: $(TARGET2).c
	gcc $^ -o $@

%.out: $(addsuffix .o, % $(ADDIT))
	gcc -o $@ $^ $(LIBS)

%.o: %.c
	gcc -c $<

clean:
	rm $(TARGET).out $(TARGET2).out
