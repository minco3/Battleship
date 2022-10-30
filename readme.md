# Battleship
1-2 player battleship game played in a terminal window

## Description

- My final project for my introductory C++ class
- 1 player and simulation mode's AI uses a probability map to determine the best move
- has a seek / hunt mode
- the player is able to place their ships on the board with colision detection to make sure the ships are being placed correctly

## What I Learned

- deeper understanding of 2D arrays
- calling values by reference
- how to efficiently test code
- game loop

## Screenshots

![placing a ship](/images/ship_placement.png)
![gameplay](/images/gameplay.png)
![simulation mode results](/images/simulation_mode.png)

## Getting Started

### Dependencies
- GCC
- CMake

### Setup

- if you want to modify the ship lengths or names ships.txt is modifiable and is checked at runtime
- the first row is ignored as it is used for the column names

#### simple setup with CMake and GCC

```
mkdir build
cd build
cmake .. -G "Unix Makefiles"
cmake --build .
main
```