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

    void display();
    void displayProbability();
    void placeShip(int index, Point point={-1,-1}, char orientation='a');
    void placeShipRandom(int index);
    void placeOnBoard(int index, Point point, char orientation);
    void getValidShipInfo(Point &point, char &orientation, int index);
    bool isOccupied(Point point, int index, char orientation);
    bool isFiredAt(Point point);
    bool isDead(Point point, int size=1, char orientation='h');
    bool beenHit(Point point, int size, char orientation);
    bool occupied(Point point, int size, char orientation);
    bool validPlacement(Point point, int index, char orientation);
    bool valid (Point point, int size, char orientation);
    bool isHuntable(Point point, int size, char orientation);
    bool isSearchable(Point point, int size, char orientation);
    Point mostProbable();
};

#endif //BATTLESHIP_BATTLESHIP_H
