# Eltex_EmbeddedC

Tasks for Eltex course on Embedded C.
The most interesting ones are highlighted ***in bold italics***.

## Description of tasks

* [[lecture5]](./lecture5)

  - [[float_trouble]](./lecture5/float_trouble.c)
  A simple example of troubles with the float type.

* [[lecture8]](./lecture8)

  - [[byte_out]](./lecture8/byte_out.c)
  Implement the byte-by-byte output of the number (from low byte to high one),
  then change some byte.

* [[lecture9]](./lecture9)

  - [[square_matrix]](./lecture9/square_matrix.c)
  Output a square matrix filled with numbers from 1 to N^2 for a given N.
    
  - [[reverse_array]](./lecture9/reverse_array.c)
  Reverse array filled with numbers from 1 to N for a given N.
    
  - [[triangular_matrix]](./lecture9/triangular_matrix.c)
  Fill the upper triangle of the matrix of a given size N with zeros, and the
  lower one (including the side diagonal) with ones.
    
  - [[snail]](./lecture9/snail.c)
  Fill a matrix of a given size N with numbers from 1 to N^2 in a clockwise
  spiral.

* [[lecture10]](./lecture10)

  - [[byte_out_ptr]](./lecture10/byte_out_ptr.c)
  Implement the byte-by-byte output of the number (from low byte to high one)
  using pointers, then change some byte.

* [[lecture11]](./lecture11)

  - [[alignment]](./lecture11/alignment.c)
  Checking the alignment of structures in memory and forcing packed.

  - [[subscribers_directory]](./lecture11/subscribers_directory.c)
  A simple subscriber directory on a static array with the functions of
  adding, viewing, searching and deleting.

* [[lecture15]](./lecture15)

  - [[crackme]](./lecture15/crackme.c)
  Hacking a simple password check, where the insecure finction 'gets' is used
  (the -fno-stack-protector and -no-pie keys for compilation).

* [[lecture18]](./lecture18)

  - [[subscribers_dynamic]](./lecture18/subscribers_dynamic.c)
  The subscriber directory as in lecture11 but with dynamic arrays
  (realocation with one element)

* [[lecture24]](./lecture24)

  - [[simple_fork]](./lecture24/simple_fork.c)
  A very simple test program with fork(), exec(), wait(), atexit().

  - [[tree_fork]](./lecture24/tree_fork.c)
  Build a tree of 6 processes (the first process has two child, where one of
  them has one child, and the second has two child), output pid and ppid for
  verification.
    
  - [[shell]](./lecture24/shell.c)
  ***A simple command interpreter that runs the entered commands in a separate
  process.***

* [[lecture25]](./lecture25)

  - [[nice]](./lecture25/nice.c)
  A program for visualizing the effect of a nice change for one of two
  identical processes. Both processes run on the same core (using
  sched_setaffinity()).

* [[lecture26]](./lecture26)

  - [[threads]](./lecture26/threads.c)
  The program runs the specified number of threads that output their internal
  number.

  - [[sum_one]](./lecture26/sum_one.c),
  [[sum_multiple]](./lecture26/sum_multiple.c)
  The first program adds 10^8 units to the global variable. The second program
  divides this task into 1000 threads and demonstrates the problem of
  accessing this variable without synchronization primitives.

* [[lecture28]](./lecture28)

  - [[demand_supply]](./lecture28/demand_supply.c)
  ***The program creates producers and customers in threads that interact with
  several shops protected by mutexes.***

* [[lecture30]](./lecture30)

  - [[pipe]](./lecture30/pipe.c)
  Parent and child processes exchange messages over unnamed channels.

  - [[mkfifo1](./lecture30/mkfifo1.c), [mkfifo2](./lecture30/mkfifo2.c)]
  Two programs exchange messages over named channels.

  - [[shell2]](./lecture30/shell2.c)
  ***A simple command interpreter with PIPE (|) support that runs the entered
  commands in a separate process.***

* [[lecture31]](./lecture31)

  - [[file_operations]](./lecture31/file_operations.c)
  Creating file and unbuffered write/read operations with it.

  - [[editor]](./lecture31/editor.c)
  ***A simple editor that can open text files, replace some existing characters
  in them (without adding new ones), and save changes. Note: does not work
  correctly with non-printable characters and tabs.***

* [[lecture32]](./lecture32)

  - [[commander]](./lecture32/commander.c)
  ***A simple file manager that allows to navigate through the file system.
  It also has two displays and allows to switch between them.***

* [[lecture33]](./lecture33)

  - [[server_systemv](./lecture33/server_systemv.c),
     [client_systemv](./lecture33/client_systemv.c)]
  Server and client programs that exchange messages using SystemV message
  queues.

  - [[server_posix](./lecture33/server_posix.c),
     [client_posix](./lecture33/client_posix.c)]
  Server and client programs that exchange messages using POSIX message
  queues.
    
  - [[server](./lecture33/server.c), [client](./lecture33/client.c)]
  ***Chat with a shared room for multiple users based on SystemV message
  queues.***

* [[lecture34]](./lecture34)

  - [[server_systemv](./lecture34/server_systemv.c),
     [client_systemv](./lecture34/client_systemv.c)]
  Server and client programs that exchange messages using SystemV shared
  memory and SystemV semaphores.

  - [[server_posix](./lecture34/server_posix.c),
     [client_posix](./lecture34/client_posix.c)]
  Server and client programs that exchange messages using POSIX shared
  memory and POSIX named semaphores.
     
  - [[server](./lecture34/server.c), [client](./lecture34/client.c)]
  ***Chat with a shared room for multiple users based on POSIX shared
  memory.***

* [[lecture35]](./lecture35)

  - [[disposition]](./lecture35/disposition.c)
  Setting signal's disposition on signal SIGUSR1.

  - [[eventloop]](./lecture35/eventloop.c)
  Creating an Eventloop by waiting for the SIGUSR1 signal.

* [[lecture36]](./lecture36)

  - [[server_local_stream](./lecture36/server_local_stream.c),
     [client_local_stream](./lecture36/client_local_stream.c)]
  Server and client exchange messages via sockets with AF_LOCAL and
  SOCK_STREAM.

  - [[server_local_dgram](./lecture36/server_local_dgram.c),
     [client_local_dgram](./lecture36/client_local_dgram.c)]
  Server and client exchange messages via sockets with AF_LOCAL and
  SOCK_DGRAM.

  - [[server_inet_stream](./lecture36/server_inet_stream.c),
     [client_inet_stream](./lecture36/client_inet_stream.c)]
  Server and client exchange messages via sockets with AF_INET and
  SOCK_STREAM.

  - [[server_inet_dgram](./lecture36/server_inet_dgram.c),
     [client_inet_dgram](./lecture36/client_inet_dgram.c)]
  Server and client exchange messages via sockets with AF_INET and
  SOCK_DGRAM.

* [[lecture39]](./lecture39)

  - [[server1](./lecture39/server1.c), [client](./lecture39/client.c)]
  The server that creates threads to process each new client. The client that
  creates multiple threads accessing the server at the same time.

  - [[server2](./lecture39/server2.c), [client](./lecture39/client.c)]
  The server that creates a thread pool in advance for processing clients and
  assigns each new client to a specific thread. The client that creates
  multiple threads accessing the server at the same time.
    
  - [[server3](./lecture39/server3.c), [client](./lecture39/client.c)]
  The server that creates request queue (with client descriptors) and a thread
  pool in advance for processing clients. Each thread takes the next client
  from  queue. The client that creates multiple threads accessing the server
  at the same time.

  - [[server_multiprotocol](./lecture39/server_multiprotocol.c),
     [client_tcp](./lecture39/client_tcp.c),
     [client_udp](./lecture39/client_udp.c)] 
  The server that can procces TCP and UDP connection simultaneously using
  poll. Simple TCP and UDP clients that connect to server and receive time.

* [[lecture42]](./lecture42)

  - [[server](./lecture42/server.c), [client](./lecture42/client.c)]
  The server and the client exchange UDP messages, and the client uses a RAW
  socket and fills in the UDP header.

* [[lecture43]](./lecture43)

  - [[server](./lecture43/server.c), [client](./lecture43/client.c)]
  The server and the client exchange UDP messages, and the client uses a RAW
  socket and fills in the UDP header and IP header.

* [[lecture44]](./lecture44)

  - [[server](./lecture44/server.c), [client](./lecture44/client.c),
     [server_windows.c](./lecture44/server_windows.c)]
  The server and the client exchange UDP messages, and the client uses a RAW
  socket and fills in the UDP, IP and Ethernet headers.

* [[lecture48]](./lecture48)

  - Screenshots of the kernel build for x86 architecture.

* [[lecture49-50]](./lecture49-40)

  - Screenshots of the kernel build for arm architecture and launch in QEMU.



