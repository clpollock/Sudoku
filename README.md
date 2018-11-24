# Sudoku
The world probably doesn't need another program for solving Sudoku puzzles, but it's an interesting little programming challenge so I wrote one anyway. It takes about 4 milliseconds to solve a puzzle on my ageing Intel Xeon E5-1620 machine.

This program should build on most Linux or UNIX systems that have a C++14 compatible version of the Clang compiler. You could also use another compiler such as gcc by changing the value of CXX in the Makefile. To do a build, simply run make in the Sudoku directory. The resulting binary is Release/sudoku. You can do a debug build by running 'make Mode=Debug', in which case the binary will be Debug/sudoku.

### Puzzle Files

Puzzle files are ASCII text files, with 9 lines, and up to 9 characters per line. Empty cells should be represented by spaces. It is not necessary to have spaces for empty cells at the end of a line - if a line contains fewer than 9 characters, the missing characters are assumed to be empty cells. It is however necessary for the file to have 9 lines, even if the bottom line(s) consist entirely of empty cells.

An example puzzle file appears below. You can find some more examples in the Puzzles directory.

<pre>
5 92  8 6
1  394  5
  75   3 
  41   7 
7 3   5 9
 2   36  
 6   14  
8  476  3
4 1  52 8
</pre>

### Solving a Puzzle

To solve a puzzle, simply run sudoku with the name of the puzzle file as a parameter. The solution is printed to standard output. If the puzzle file is invalid or unsolvable, an error message is printed to standard error:

<pre>
$ Release/sudoku Puzzles/Puzzle01.txt 
539217846
186394725
247568931
654189372
713642589
928753614
365821497
892476153
471935268
</pre>

Verifying a Puzzle

If you run sudoku with the -v (verify) parameter, it will verify that the specified file represents a valid solution, i.e. it contains no empty cells, and does not contain any duplicate numbers in a row, column, or sub-grid:

<pre>
$ Release/sudoku Puzzles/Puzzle01.txt > Solution1.txt
$ Release/sudoku -v Solution1.txt
Solution1.txt is a valid solution.
</pre>
