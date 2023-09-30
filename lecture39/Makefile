CLIENTS := client client_tcp client_udp
SERVERS := server1 server2 server3 server_multiprotocol
LIBS1 := -pthread

.PHONY: all clean

all: $(addsuffix .out, $(CLIENTS) $(SERVERS))

%.out: %.c
	gcc $< -o $@ $(CFLAGS) $(LIBS1)

clean:
	rm $(addsuffix .out, $(CLIENTS) $(SERVERS))
