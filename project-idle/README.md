# README.md

## Prerequisites
`C++` -  20 version

`SFML` - https://www.sfml-dev.org/tutorials/2.6/start-linux.php

## How to run project
1. Open terminal in the project root.
2. `mkdir build && cd build`
3. `cmake ..`
4. `make`
5. `./idle-game`

## Save/Load
Game will automatically create savefile with name `idle-save.txt` in a directory with the game.
If such file exists at the opening of program, game will open it and load save from it. 
Game automatically saves every 10 seconds in file `idle-save.txt`. 
If data in file is edited manually, correctness is not guaranteed.
Some precision will be lost on load/save cycle.