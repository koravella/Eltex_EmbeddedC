# Eltex_EmbeddedC

Tasks for Eltex course on Embedded C.

## Description of tasks

* [[lecture1]](./lecture1)

    - [[hello]](./lecture1/hello.c)
    Write a simple HelloWorld program and perform all four stages of compilation
    (see all intermediate files).

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
    A simple command interpreter that runs the entered commands in a separate
    process.

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
    The program creates producers and customers in threads that interact with
    several shops protected by mutexes.
