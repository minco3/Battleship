#include "battleship.h"
#include <random>

int genRandom(int a, int b);

void PlayerBoard ::display() {
    string name = "Your Board";
    cout << setw((BOARD_WIDTH*4+4-name.length())/2) << ' ' << name
         << setw(((BOARD_WIDTH*4+4-name.length()))-((BOARD_WIDTH*4+4-name.length())/2)) << ' ' << '\n';
    for (int x = 0; x <= BOARD_WIDTH; x++) // first row
        if (x == 0)
            cout << ' ';
        else
            cout << setw(4) << x;
    cout << '\n';
    for (int y = 0; y <=BOARD_HEIGHT; y++) {

        cout << setw(2) << ' ';
        string line(BOARD_WIDTH * 4 + 1, '-');
        cout << line << '\n';

        if (y==BOARD_HEIGHT) break;
        cout << char(y + 'A') << " | ";
        for (int x = 0; x < BOARD_WIDTH; x++)
            cout << (isalpha(board[x][y]) ? board[x][y] : ' ') << " | ";
        cout << '\n';
    }
}
void PlayerBoard::displayAsEnemy() {
    string name = "Enemy Board";
    cout << setw((BOARD_WIDTH*4+4-name.length())/2) << ' ' << name
    << setw(((BOARD_WIDTH*4+4-name.length()))-((BOARD_WIDTH*4+4-name.length())/2)) << ' ' << '\n';

    for (int x = 0; x <= BOARD_WIDTH; x++) // first row
        if (x == 0)
            cout << ' ';
        else
            cout << setw(4) << x;
    cout << '\n';
    for (int y = 0; y <=BOARD_HEIGHT; y++) {

        cout << setw(2) << ' ';
        string line(BOARD_WIDTH * 4 + 1, '-');
        cout << line << '\n';

        if (y==BOARD_HEIGHT) break;
        cout << char(y + 'A') << " | ";
        for (int x = 0; x < BOARD_WIDTH; x++)
            cout << ((isalpha(board[x][y])&&board[x][y]!='S') ? board[x][y] : ' ') << " | ";
        cout << '\n';
    }
}
void PlayerBoard::placeShip(int index) {
    char orientation;
    getValidShipInfo(fleet[index].points[0], orientation, index);
}
void PlayerBoard::placeShipRandom(int index) {
    int x,y;
    char orientation;
    do {
        orientation=genRandom(0,1)?'h':'v';
        if (orientation=='h') { // 0 is vertical 1 is horizontal
            // horizontal
            x=genRandom(0,BOARD_WIDTH-fleet[index].size);
            y=genRandom(0, BOARD_HEIGHT-1);
        } else {
            // vertical
            x=genRandom(0,BOARD_WIDTH-1);
            y=genRandom(0, BOARD_HEIGHT-fleet[index].size);
        }
    } while (spaceOccupied(Point(x,y), index, orientation));
    for (int i=0; i<fleet[index].size; i++)
        if (orientation=='h')
            fleet[index].points.emplace_back(x+i,y);
        else
            fleet[index].points.emplace_back(x,y+i);
}
void PlayerBoard::getValidShipInfo(Point &point, char &orientation, int index) {

    int x;
    char y;
    bool occupied=false;
    cin.sync();
    do {
        while (true) {
            while (true) {
                cout << "Please input a vertical coordinate to place your " << fleet[index].name << " at (A-" << char(BOARD_HEIGHT+'A'-1) << ") :\n";
                cin >> y;
                if (islower(y)) y=toupper(y);
                y-='A';
                if (y>=0&&y<BOARD_HEIGHT)
                    break;
                cout << "ERROR: invalid coordinate\n";
                cin.sync();
            }
            while (true) {
                cout << "Please input a horizontal coordinate to place your " << fleet[index].name << " at (1-" << BOARD_WIDTH << ") :\n";
                cin >> x;
                x-=1;
                if (x>=0&&x<BOARD_WIDTH)
                    break;
                cout << "ERROR: invalid coordinate\n";
                cin.sync();
            }
            while (true) {
                cout << "Please input an orientation to place your " << fleet[index].name << " with (h/v)\n";
                cin >> orientation;
                if (orientation=='h'||orientation=='v')
                    break;
                cout << "ERROR: invalid orientation\n";
                cin.sync();
            }

            if (validPlacement(Point(x,y), index, orientation))
                break;
            cout << "ERROR: invalid placement\n";
        }

        occupied = spaceOccupied(Point(x,y), index, orientation);

        if (occupied)
            cout << "ERROR: space is occupied\n";

    } while (occupied);

    for (int i=0; i<fleet[index].size; i++)
        if (orientation=='h')
            fleet[index].points.emplace_back(x+i,y);
        else
            fleet[index].points.emplace_back(x,y+i);
}
bool PlayerBoard::spaceOccupied(Point point, int index, char orientation) {
    for (int i=0; i<fleet[index].size; i++)
        if (orientation=='h') {
            if (board[point.x+i][point.y]=='S') return true;
        } else {
            if (board[point.x][point.y+i]=='S') return true;
        }

    return false;
}
bool PlayerBoard::isFiredAt(Point point) {
    if (board[point.x][point.y]=='X'||board[point.x][point.y]=='O')
        return true;
    return false;
}
bool PlayerBoard::isDead(Point point) {
    for (int a=0; a<FLEET_SIZE; a++)
        if (!fleet[a].alive)
            for (int b=0; b<fleet[a].size; b++)
                if (fleet[a].points[b].x==point.x&&fleet[a].points[b].y==point.y)
                    return true;
    return false;
}
bool PlayerBoard::validPlacement(Point point, int index, char orientation) {
    if (orientation=='v') {
        if (point.y+fleet[index].size-1>=BOARD_HEIGHT) return false;
    } else {
        if (point.x+fleet[index].size-1>=BOARD_WIDTH) return false;
    }
    return true;
}
Point PlayerBoard::mostProbable() {

    int hitCount=0, killedHitCount=0, shipLengths[FLEET_SIZE]{}, ships[BOARD_WIDTH][BOARD_HEIGHT]{};;
    Point point(0,0);

    for (int x=0; x<BOARD_WIDTH; x++)
        for (int y=0; y<BOARD_HEIGHT; y++)
            if (board[x][y] == 'X')
                hitCount++;

    for (int i=0; i<FLEET_SIZE; i++)
        if (fleet[i].alive)
            shipLengths[i]=fleet[i].size;
        else
            killedHitCount+=fleet[i].size;

    if (hitCount>killedHitCount) {
        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (board[x][y] != 'O'&&!isDead(Point(x,y)))
                    for (int i=0; i<FLEET_SIZE; i++)
                        if (shipLengths[i] != 0) {
                            //vertical
                            bool valid=true, occupied=false, beenHit=false;
                            if (y + shipLengths[i]-1>=BOARD_HEIGHT) valid=false;
                            if (valid) {
                                for (int j= 1; j<shipLengths[i]; j++)
                                    if (board[x][y+j] == 'O'||isDead(Point(x,y+j))) occupied=true;
                                if (!occupied) {
                                    for (int g = 0; g < shipLengths[i]; g++)
                                        if (board[x][y + g] == 'X') beenHit = true;
                                    if (beenHit)
                                        for (int h = 0; h < shipLengths[i]; h++)
                                            ships[x][y + h]++;
                                }
                            }
                            //horizontal
                            valid=true, occupied=false, beenHit=false;
                            if (x + shipLengths[i]-1>=BOARD_WIDTH) valid = false;
                            if (valid) {
                                for (int j=1; j<shipLengths[i]; j++)
                                    if (board[x+j][y] == 'O'||isDead(Point(x+j,y))) occupied = true;
                                if (!occupied) {
                                    for (int g = 0; g < shipLengths[i]; g++)
                                        if (board[x + g][y] == 'X') beenHit = true;
                                    if (beenHit)
                                        for (int h = 0; h < shipLengths[i]; h++)
                                            ships[x + h][y]++;
                                }
                            }
                        }

        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (ships[x][y] > ships[point.x][point.y]&&board[x][y]!='X')
                    point = Point(x, y);
    } else {
        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (board[x][y] != 'O' && board[x][y] != 'X')
                    for (int i = 0; i < FLEET_SIZE; i++)
                        if (shipLengths[i] != 0) {
                            //vertical
                            bool valid = true, occupied = false;
                            if (y + shipLengths[i] - 1 >= BOARD_HEIGHT) valid = false;
                            if (valid) {
                                for (int j = 0; j < shipLengths[i]; j++)
                                    if (isFiredAt(Point(x, y + j))) occupied = true;
                                if (!occupied)
                                    for (int h = 0; h < shipLengths[i]; h++)
                                        ships[x][y + h]++;
                            }
                            //horizontal
                            valid = true, occupied = false;
                            if (x + shipLengths[i] - 1 >= BOARD_WIDTH) valid = false;
                            if (valid) {
                                for (int j = 0; j < shipLengths[i]; j++)
                                    if (isFiredAt(Point(x+j, y))) occupied = true;
                                if (!occupied)
                                    for (int h = 0; h < shipLengths[i]; h++)
                                        ships[x + h][y]++;
                            }
                        }

        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (ships[x][y] > ships[point.x][point.y])
                    point = Point(x, y);

    }

    return point;
}


int genRandom(int a, int b) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(a,b);
    return dist(mt);
}