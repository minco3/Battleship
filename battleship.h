#ifndef BATTLESHIP_BATTLESHIP_H
#define BATTLESHIP_BATTLESHIP_H
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;



static const int BOARD_WIDTH=10, BOARD_HEIGHT=10, FLEET_SIZE=5, PLAYER_COUNT=2, BOARD_OFFSET=7;\

struct Point {
    int x,y;
    Point(int a, int b) : x(a), y(b) {}

};

class Ship {
public:
    string name;
    int size, hitcount=0;
    vector<Point> points;
    bool alive=true;

    Ship(string str, int num) : name(str), size(num) {}
    Ship() {}

};

class PlayerBoard {
public:
    string name;
    char board[10][10]{};
    Ship fleet[FLEET_SIZE];
    bool isHuman=true;

    void display(); // displays all points on one board
    void placeShip(int index, Point point={-1,-1}, char orientation='a'); // places ship (point and orientation are optional if you want to specify where to place it instead of using the prompts)
    void placeShipRandom(int index); // tries to place a ship randomly until it will fit
    void placeOnBoard(int index, Point point, char orientation); // fills the board with 'S' at the ship position
    void getValidShipInfo(Point &point, char &orientation, int index); // used if a point and orientation isnt specified when calling placeShip() 
    bool isOccupied(Point point, int index, char orientation); // checks if a given spot will fit a ship
    bool isFiredAt(Point point); // checks if a point is an 'X' or an 'O'
    bool isDead(Point point, int size=1, char orientation='h'); // checks if any point or potential ship collides with another ship that has been killed already
    bool beenHit(Point point, int size, char orientation); // checks if a potential ship collides with a tile that has been hit already
    bool occupied(Point point, int size, char orientation); // checks if a potential ship will collide with a miss
    bool validPlacement(Point point, int index, char orientation); // checks if a given ship will fit at a point 
    bool valid (Point point, int size, char orientation); // checks if a potential ship will fit at a point
    bool isHuntable(Point point, int size, char orientation); // checks if a potential ship could be at a point in the hunt mode
    bool isSearchable(Point point, int size, char orientation); // checks if a potential ship could be at a point in the search mode
    Point mostProbable(); // used to generate a probability map of all points on the board based on information that is available to the player
};

#endif //BATTLESHIP_BATTLESHIP_H
