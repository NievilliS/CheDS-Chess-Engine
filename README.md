# Chess_Engine
A dumb chess engine and AI made by Nievillis (https://www.youtube.com/watch?v=q5XfvlTfjPQ)

# FYI: THIS IS INCOMPLETED!!!!

___

## About
This is a console program for a chess board thingy. I made this for the fun of it and because I am really bored.

## Pieces
You will come across fields that have capital P and lower-case p, which is always the first initial of the piece:

- P - white pawn
- b - black bishop
- K - white king
- k - black king

## Console syntax
In order to be able to do anything, you need the console syntax:

- x - exit application
- p - printout board
- y - reset board

## Chess move notation
But that's not all. You will have to use Chess Algebraic Notation, which is more deeply described in https://en.wikipedia.org/wiki/Algebraic_notation_(chess)#Castling .

The following examples might help you out a bit :)

- e4 - Move pawn at E-file to e4
- AxB - Use pawn at A-file to capture the piece at B-file. The x is required as it is a pawn.
- NC3 - Knight to C3
- qxe4 - Capture piece at e4 with Queen. Note that the 'x' is not necessary for any pieces besides the pawns. When using this, however, the program checks if the resulting move was actually a capture.
- 0-0 - Castle King-side
- 0-0-0 - Castle Queen-side
- f7xe=Q - White pawn at F7 captures at E8 and gets promoted to a Queen
- RA4xb4 - Took at A4 captures piece at B4. Again, the 'x' is not necessary.

White always starts and thus has a prefix such as "1.", "2.", ...

All you need to know is that you can not use any special characters to indicate a check (via suffix +) or anything alike.

## Invalid moves return "errors"
For invalid moves the program will spit out very undescriptive error codes such as 0, 3 and 4, because this isn't Java. Cry about it. (I did)

- 0 = Movement check error or general error
- 3 = Piece error
- 4 = Syntax or wrong syntax error

___

## Compilation

Depending on your compiler, you might have to change the CC, CXX variables in makefile.mak.
I used the MinGW32 compiler from Code::Blocks (without actually using Code::Blocks lol).

Compiler should follow this syntax:

``gcc -c file.c -o file.o``

You should not have to change any further settings other than OUTPUT_PATH, which currently outputs as "o.exe" and SRCS incase of additions, etc.

___

## Have a great day :)
