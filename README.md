# Diceware

A simple command line tool to generate [diceware](http://world.std.com/~reinhold/diceware.html)
passwords from diceware wordlists.

# Usage

The program needs a word list file (which you can download from the link above) to
work. The command to generate a password using a wordlist named `diceware.wordlist.asc` is:

    $ diceware -d diceware.wordlist.asc

If you need more or less words than the default, you can give the amount of words: 

    $ diceware -d diceware.wordlist.asc -n 12

On non-windows systems you can select which device is used for random data:

    $ diceware -d diceware.wordlist.asc -r /dev/random

The default source is `/dev/urandom`. On Windows the program uses the Windows cryptography
API (`CryptGenRandom` and friends).

# Building

To build on a unixy system:

    $ make

and to cross-compile for example on Linux to win32:

    $ make HOST=i686-w64-mingw32

Building on Windows with MinGW:

    > make
