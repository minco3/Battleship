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
    void displayAsEnemy();
    void placeShip(int index);
    void placeShipRandom(int index);
    void getValidShipInfo(Point &point, char &orientation, int index);
    bool spaceOccupied(Point point, int index, char orientation);
    bool isFiredAt(Point point);
    bool isDead(Point point);
    bool validPlacement(Point point, int index, char orientation);
    Point mostProbable();
};

#endif //BATTLESHIP_BATTLESHIP_H
