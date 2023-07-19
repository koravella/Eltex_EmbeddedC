# Eltex_EmbeddedC

Tasks for Eltex course on Embedded C.

## Description of tasks

* [[lecture1]](./lecture1)
    - [[hello]](./lecture1/hello.c) Write a simple HelloWorld program and
    perform all four stages of compilation (see all intermediate files).

* [lecture5]
    1. A simple example of troubles  with the float type.
* [lecture8]
    1. Implement the byte-by-byte output of the number (from low byte to high
    one), then change some byte.
* [lecture9]
    1. [square_matrix] Output a square matrix filled with numbers from 1 to N^2
    for a given N.
    2. [reverse_array] Reverse array filled with numbers from 1 to N for a
    given N.
    3. [triangular_matrix] Fill the upper triangle of the matrix of a
    given size N with zeros, and the lower one (including the side diagonal)
    with ones.
    4. [snail] Fill a matrix of a given size N with numbers from 1 to N^2
    in a clockwise spiral.
* [lecture10]
    1. Implement the byte-by-byte output of the number (from low byte to high
    one) using pointers, then change some byte.
* [lecture11]
    1. [alignment] Checking the alignment of structures in memory and forcing
    packed.
    2. [subscribers_directory] A simple subscriber directory on a static array
    with the functions of adding, viewing, searching and deleting.
* [lecture15]
    1. Hacking a simple password check, where the insecure finction 'gets' is
    used (the -fno-stack-protector and -no-pie keys for compilation).
* [lecture18]
    1. The subscriber directory as in lecture11 but with dynamic arrays
    (realocation with one element)
* [lecture24]
    1. [simple_fork] A very simple test program with fork(), exec(), wait(),
    atexit().
    2. [tree_fork] Build a tree of 6 processes (the first process has two child,
    where one of them has one child, and the second has two child), output pid
    and ppid for verification.
    3. [shell] A simple command interpreter that runs the entered commands in a
    separate process.
* [lecture25]
    1. A program for visualizing the effect of a nice change for one of two
    identical processes. Both processes run on the same core (using
    sched_setaffinity()).
* [lecture26]
    1. [threads] The program runs the specified number of threads that output
    their internal number.
    2. [sum_one, sum_multiple] The first program adds 10^8 units to the global
    variable. The second program divides this task into 1000 threads and
    demonstrates the problem of accessing this variable without synchronization
    primitives.
* [lecture28]
    1. The program creates producers and customers in threads that interact with
    several shops protected by mutexes.
