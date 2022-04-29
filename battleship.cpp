#include "battleship.h"
#include <random>

int genRandom(int a, int b);

void PlayerBoard::display() {
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
void PlayerBoard::placeShip(int index, Point point, char orientation) {
    if ((orientation!='h'&&orientation!='v')||!validPlacement(point,index,orientation)||isOccupied(point,index,orientation))
        getValidShipInfo(point, orientation, index);

    placeOnBoard(index, point, orientation);
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
    } while (isOccupied(Point(x, y), index, orientation));

    placeOnBoard(index, {x,y}, orientation);
}
void PlayerBoard::placeOnBoard(int index, Point point, char orientation) {

    for (int i=0; i<fleet[index].size; i++)
        if (orientation=='h')
            fleet[index].points.emplace_back(point.x+i,point.y);
        else
            fleet[index].points.emplace_back(point.x,point.y+i);

    for (int i=0; i<fleet[index].size; i++) 
        board[fleet[index].points[i].x][fleet[index].points[i].y] = 'S';
}
void PlayerBoard::getValidShipInfo(Point &point, char &orientation, int index) {
    bool occupied=false;
    char y;
    int x;
    cin.sync();
    do {
        while (true) {
            while (point.y<0||point.y>BOARD_HEIGHT) {
                cout << "Please input a vertical coordinate to place your " << fleet[index].name << " at (A-" << char(BOARD_HEIGHT+'A'-1) << ") :\n";
                cin >> y;
                if (islower(y)) y=toupper(y);
                y-='A';
                if (y<0||y>BOARD_HEIGHT) {
                    cout << "ERROR: invalid coordinate\n";
                    cin.sync();
                }
                else point.y=y;
            }
            while (point.x<0||point.x>BOARD_WIDTH) {
                cout << "Please input a horizontal coordinate to place your " << fleet[index].name << " at (1-" << BOARD_WIDTH << ") :\n";
                cin >> x;
                x-=1;
                if (x<0||x>=BOARD_WIDTH) {
                    cout << "ERROR: invalid coordinate\n";
                    cin.sync();
                }
                else point.x=x;
            }
            while (orientation!='h'&&orientation!='v') {
                cout << "Please input an orientation to place your " << fleet[index].name << " with (h/v)\n";
                cin >> orientation;
                if (orientation!='h'&&orientation!='v') {
                    cout << "ERROR: invalid orientation\n";
                    cin.sync();
                }
            }

            if (validPlacement(point, index, orientation))
                break;
            cout << "ERROR: invalid placement\n";
        }

        occupied = isOccupied(point, index, orientation);

        if (occupied)
            cout << "ERROR: space is occupied\n";

    } while (occupied);

}
bool PlayerBoard::isOccupied(Point point, int index, char orientation) {
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
bool PlayerBoard::isDead(Point point, int size, char orientation) {
        if (orientation=='h') {
            for (int a = 0; a < FLEET_SIZE; a++)
                if (!fleet[a].alive)
                    for (int b = 0; b < fleet[a].size; b++)
                        for (int i=0; i<size; i++)
                            if (fleet[a].points[b].x == point.x+i && fleet[a].points[b].y == point.y)
                                return true;
        } else {
            for (int a = 0; a < FLEET_SIZE; a++)
                if (!fleet[a].alive)
                    for (int b = 0; b < fleet[a].size; b++)
                        for (int i=0; i<size; i++)
                            if (fleet[a].points[b].x == point.x && fleet[a].points[b].y == point.y+i)
                                return true;
        }

    return false;
}
bool PlayerBoard::validPlacement(Point point, int index, char orientation) {
    if (orientation=='h') {
        if (point.x+fleet[index].size-1>=BOARD_WIDTH) return false;
    } else {
        if (point.y+fleet[index].size-1>=BOARD_HEIGHT) return false;
    }
    return true;
}
bool PlayerBoard::beenHit(Point point, int size, char orientation) {
    if (orientation=='h') {
        for (int i=0; i<size; i++)
            if (board[point.x+i][point.y] == 'X') return true;
    } else {
        for (int i=0; i<size; i++)
            if (board[point.x][point.y+i] == 'X') return true;
    }
    return false;
}
bool PlayerBoard::occupied(Point point, int size, char orientation) {
    if (orientation=='h') {
        for (int j=0; j<size;j++)
            if (board[point.x+j][point.y] == 'O') return true;
    } else {
        for (int j=0; j<size;j++)
            if (board[point.x][point.y+j] == 'O') return true;
    }
    return false;
}
bool PlayerBoard::valid(Point point, int size, char orientation) {
    if (orientation=='h') {
        if (point.x+size-1>=BOARD_HEIGHT) return false;
    } else {
        if (point.y+size-1>=BOARD_HEIGHT) return false;
    }
    return true;
}
bool PlayerBoard::isHuntable(Point point, int size, char orientation) {
    if (!valid(point, size, orientation)) return false;
    if (isDead(point, size, orientation)) return false;
    if (occupied(point, size, orientation)) return false;
    if (!beenHit(point, size, orientation)) return false;
    return true;
}
bool PlayerBoard::isSearchable(Point point, int size, char orientation) {
    if (!valid(point, size, orientation)) return false;
    if (isDead(point, size, orientation)) return false;
    if (occupied(point, size, orientation)) return false;
    return true;
}
Point PlayerBoard::mostProbable() {

    vector<int> shipLengths;
    int hitCount=0, killedHitCount=0, ships[BOARD_WIDTH][BOARD_HEIGHT]{};;
    Point point(0,0);
    int max=0;

    for (int x=0; x<BOARD_WIDTH; x++)
        for (int y=0; y<BOARD_HEIGHT; y++)
            if (board[x][y] == 'X')
                hitCount++;

    for (int i=0; i<FLEET_SIZE; i++)
        if (fleet[i].alive)
            shipLengths.push_back(fleet[i].size);
        else
            killedHitCount+=fleet[i].size;

    if (hitCount>killedHitCount) { // hunt

        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (board[x][y] != 'O'&&!isDead(Point(x,y)))
                    for (int i=0; i<shipLengths.size(); i++) {
                        if (isHuntable({x, y}, shipLengths[i], 'h'))// horizontal
                            for (int h = 0; h < shipLengths[i]; h++)
                                if (board[x+h+1][y]=='X'||board[x+h-1][y]=='X'||board[x+h][y+1]=='X'||board[x+h][y-1]=='X')
                                    ships[x+h][y]++;
                        if (isHuntable({x, y}, shipLengths[i], 'v')) // vertical
                            for (int h = 0; h < shipLengths[i]; h++)
                                if (board[x+1][y+h]=='X'||board[x-1][y+h]=='X'||board[x][y+h+1]=='X'||board[x][y+h-1]=='X')
                                    ships[x][y+h]++;
                        }

        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (max < ships[x][y]&&board[x][y]!='X') {
                    max = ships[x][y];
                    point = {x, y};
                }

    } else { // search
        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (board[x][y] != 'O' && board[x][y] != 'X')
                    for (int i = 0; i < shipLengths.size(); i++) {
                            if (isSearchable({x,y},shipLengths[i],'h')) // horizontal
                                for (int h=0; h<shipLengths[i]; h++)
                                    ships[x+h][y]++;
                            if (isSearchable({x,y},shipLengths[i],'v')) // vertical
                                for (int h=0; h<shipLengths[i]; h++)
                                    ships[x][y+h]++;
                        }

        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (max < ships[x][y]) {
                    max = ships[x][y];
                    point = {x, y};
                }

    }

    return point;
}

int genRandom(int a, int b) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(a,b);
    return dist(mt);
}