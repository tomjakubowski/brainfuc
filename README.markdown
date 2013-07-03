brainfuc
========

basic brainfuck interpreter in C

building brainfuc
-----------------

you must have CMake >= 2.8 installed. change into the `build/` directory and run

    cmake .. && make

this should build `bin/brainfuc`. run it with an example program:

    echo "hello world" | bin/brainfuc ../examples/cat.tf

implementation notes
--------------------

* exactly 30000 bytes of memory are available to the program. moving
  the pointer beyond this limit on the left or right results in
  undefined behavior.
* when the `,` instruction reads EOF, the byte at the data pointer is
  unchanged
* only the first 1024 bytes or first line, whichever is shorter, of the
  program is read. this will be changed to allow for larger programs in
  the near future.
* there is an additional non-standard instruction `#` which prints the
  first 16 bytes of memory (from the start of the array), the current
  position of the data pointer, and the value at the data pointer.

language reference
------------------

* `>`	increment the data pointer (to point to the next cell to the right).
* `<`	decrement the data pointer (to point to the next cell to the left).
* `+`	increment (increase by one) the byte at the data pointer.
* `-`	decrement (decrease by one) the byte at the data pointer.
* `.`	output the byte at the data pointer.
* `,`	accept one byte of input, storing its value in the byte at the data pointer.
* `[`	if the byte at the data pointer is zero, then instead of moving the
    instruction pointer forward to the next command, jump it forward to the
    command after the matching ] command.
* `]`	if the byte at the data pointer is nonzero, then instead of moving the
    instruction pointer forward to the next command, jump it back to the
    command after the matching [ command.
