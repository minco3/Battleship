#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

static const int BOARD_WIDTH=10, BOARD_HEIGHT=10, FLEET_SIZE=5, PLAYER_COUNT=2;

struct Point {
    int x,y;
    Point(int a, int b) {
        x=a; y=b;
    }
};

struct Ship {
    string name;
    int size, hitcount;
    vector<Point> points;
};

struct PlayerBoard {
    string name;
    char board[10][10]{};
    Ship fleet[FLEET_SIZE];
};

void displayBoards(PlayerBoard players[]);
void initFleet(PlayerBoard &board);
void boardSetup(PlayerBoard &playerA, PlayerBoard &playerB);
void placeShip(PlayerBoard &board, int index);
void getValidShipInfo(Point &point, char &orientation, PlayerBoard &board, int index);
bool spaceOccupied(PlayerBoard board, Point point, char orientation, int index);

int main() {
    PlayerBoard players[2];
    players[0].name = "Your";
    players[1].name = "Enemy";
    players[0].board[0][0] = 'x';
    displayBoards(players);
    return 0;
}


void displayBoards(PlayerBoard players[2]) {
    for (int a=0; a<PLAYER_COUNT; a++) {
        string name = players[a].name+" Board";
        cout << setw((BOARD_WIDTH*4+4-name.length())/2) << ' ' << name << setw(((BOARD_WIDTH*4+4-name.length()))-((BOARD_WIDTH*4+4-name.length())/2)) << ' ';
        if (!a)
            cout << setw(5) << ' ';
        else cout << '\n';
    }
    for (int a=0; a<PLAYER_COUNT; a++) {
        for (int x = 0; x <= BOARD_WIDTH; x++) // first row
            if (x == 0)
                cout << ' ';
            else
                cout << setw(4) << x;

        if (!a)
            cout << setw(8) << ' ';
        else cout << '\n';
    }
    for (int y = 0; y <=BOARD_HEIGHT; y++) {
        for (int a=0; a<PLAYER_COUNT; a++) {
            cout << setw(2) << ' ';
            string line(BOARD_WIDTH * 4 + 1, '-');
            cout << line;

            if (!a)
                cout << setw(6) << ' ';
            else cout << '\n';
        }
        for (int a=0; a<PLAYER_COUNT; a++) {
            if (y==BOARD_HEIGHT) break;
            cout << char(y + 'A') << " | ";
            for (int x = 0; x < BOARD_WIDTH; x++)
                cout << players[a].board[x][y] << " | ";

            if (!a)
                cout << setw(5) << ' ';
            else cout << '\n';
        }
    }
}
void initFleet(PlayerBoard &board) {

}
void boardSetup(PlayerBoard &a, PlayerBoard &&b) {

}
void placeShip(PlayerBoard &board, int index) {

}
void getValidShipInfo(Point &point, char &orientation, PlayerBoard &board, int index) {

}
bool spaceOccupied(PlayerBoard board, Point point, char orientation, int index) {

}
