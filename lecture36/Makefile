BINS1 := server_local_stream client_local_stream
BINS2 := server_local_dgram client_local_dgram
BINS3 := server_inet_stream client_inet_stream
BINS4 := server_inet_dgram client_inet_dgram

.PHONY: all

all: $(addsuffix .out, $(BINS1) $(BINS2) $(BINS3) $(BINS4))

%.out: %.c
	gcc $< -o $@ $(CFLAGS) $(LIBS3)

clean:
	rm $(addsuffix .out, $(BINS1) $(BINS2) $(BINS3) $(BINS4))
