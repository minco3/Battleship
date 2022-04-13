#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <random>
using namespace std;

static const int BOARD_WIDTH=10, BOARD_HEIGHT=10, FLEET_SIZE=5, PLAYER_COUNT=2, BOARD_OFFSET=7;
static const string SHIP_INFO_PATH = "ships.txt";

struct Point {
    int x,y;
    Point(int a, int b) {
        x=a; y=b;
    }
};

struct Ship {
    string name;
    int size, hitcount=0;
    vector<Point> points;
    bool alive=true;

    Ship(string str, int num) {
        name = str;
        size = num;
    }
    Ship() {}
};

struct PlayerBoard {
    string name;
    char board[10][10]{};
    Ship fleet[FLEET_SIZE];
    bool isHuman=true;
};

void displayBoards(PlayerBoard players[PLAYER_COUNT]);
void displayBoards(PlayerBoard players[PLAYER_COUNT], int index);
void displayBoard(PlayerBoard player);
void initFleet(PlayerBoard &board);
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]);
void modeSelect(PlayerBoard (&players)[PLAYER_COUNT]);
void placeShip(PlayerBoard &board, int index);
void placeShipRandom(PlayerBoard &board, int index);
void getValidShipInfo(Point &point, char &orientation, PlayerBoard &board, int index);
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index);
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index);
void fire(PlayerBoard &targetPlayer, Point targetPoint);
bool spaceOccupied(PlayerBoard board, Point point, int index, char orientation);
bool spaceOccupied(PlayerBoard board, Point point);
bool validPlacement(PlayerBoard board, Point point, int index, char orientation);
bool lastAlive(PlayerBoard (&players)[PLAYER_COUNT]);
int genRandom(int a, int b);

int main() {
    PlayerBoard players[2];
    boardSetup(players);
    displayBoards(players);
    do {
        for (int i=0; i<PLAYER_COUNT; i++) {
            if (players[i].isHuman) {
                playerTurn(players, i);
            } else {
                playerTurn(players, i);
            }
        }
    } while (!lastAlive(players));
    return 0;
}


void displayBoards(PlayerBoard players[PLAYER_COUNT]) {
    for (int a=0; a<PLAYER_COUNT; a++) {
        string name = players[a].name+"\'s Board";
        cout << setw((BOARD_WIDTH*4+4-name.length())/2) << ' ' << name
             << setw(((BOARD_WIDTH*4+4-name.length()))-((BOARD_WIDTH*4+4-name.length())/2)) << ' ';
        if (a!=PLAYER_COUNT-1)
            cout << setw(BOARD_OFFSET) << ' ';
        else cout << '\n';
    }
    for (int a=0; a<PLAYER_COUNT; a++) {
        for (int x = 0; x <= BOARD_WIDTH; x++) // first row
            if (x == 0)
                cout << ' ';
            else
                cout << setw(4) << x;

        if (a!=PLAYER_COUNT-1)
            cout << setw(BOARD_OFFSET+3) << ' ';
        else cout << '\n';
    }
    for (int y = 0; y <=BOARD_HEIGHT; y++) {
        for (int a=0; a<PLAYER_COUNT; a++) {
            cout << setw(2) << ' ';
            string line(BOARD_WIDTH * 4 + 1, '-');
            cout << line;

            if (a!=PLAYER_COUNT-1)
                cout << setw(BOARD_OFFSET+1) << ' ';
            else cout << '\n';
        }
        for (int a=0; a<PLAYER_COUNT; a++) {
            if (y==BOARD_HEIGHT) break;
            cout << char(y + 'A') << " | ";
            for (int x = 0; x < BOARD_WIDTH; x++)
                cout << (isalpha(players[a].board[x][y]) ? players[a].board[x][y]: ' ') << " | ";

            if (a!=PLAYER_COUNT-1)
                cout << setw(BOARD_OFFSET) << ' ';
            else cout << '\n';
        }
    }
}
void displayBoards(PlayerBoard players[PLAYER_COUNT], int index) {
    for (int a=0; a<PLAYER_COUNT; a++) {
        string name = players[a].name+" Board";
        cout << setw((BOARD_WIDTH*4+4-name.length())/2) << ' ' << name
             << setw(((BOARD_WIDTH*4+4-name.length()))-((BOARD_WIDTH*4+4-name.length())/2)) << ' ';
        if (a!=PLAYER_COUNT-1)
            cout << setw(BOARD_OFFSET) << ' ';
        else cout << '\n';
    }
    for (int a=0; a<PLAYER_COUNT; a++) {
        for (int x = 0; x <= BOARD_WIDTH; x++) // first row
            if (x == 0)
                cout << ' ';
            else
                cout << setw(4) << x;

        if (a!=PLAYER_COUNT-1)
            cout << setw(BOARD_OFFSET+3) << ' ';
        else cout << '\n';
    }
    for (int y = 0; y <=BOARD_HEIGHT; y++) {
        for (int a=0; a<PLAYER_COUNT; a++) {
            cout << setw(2) << ' ';
            string line(BOARD_WIDTH * 4 + 1, '-');
            cout << line;

            if (a!=PLAYER_COUNT-1)
                cout << setw(BOARD_OFFSET+1) << ' ';
            else cout << '\n';
        }
        for (int a=0; a<PLAYER_COUNT; a++) {
            if (y==BOARD_HEIGHT) break;
            cout << char(y + 'A') << " | ";
            for (int x = 0; x < BOARD_WIDTH; x++) {
                char c;
                if (a == index) c=isalpha(players[a].board[x][y]) ? players[a].board[x][y]: ' ';
                else c=(isalpha(players[a].board[x][y])&&players[a].board[x][y]!='S') ? players[a].board[x][y] : ' ';
                cout << c << " | ";
            }

            if (a!=PLAYER_COUNT-1)
                cout << setw(BOARD_OFFSET) << ' ';
            else cout << '\n';
        }
    }
}
void displayBoard(PlayerBoard player) {
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
                cout << (isalpha(player.board[x][y]) ? player.board[x][y] : ' ') << " | ";
            cout << '\n';
    }
}
void initFleet(PlayerBoard &board) {
    string a;
    ifstream stream(SHIP_INFO_PATH);
    if (!stream.is_open()) {
        cout << "ERROR: Error opening ship info file, check path";
        exit(1);
    }
    int i=0;
    while (getline(stream,a))
        if (isdigit(a[0])) {
            stringstream basicStringstream(a);
            int index, size;
            string name;
            basicStringstream >> index >> name >> size;
            board.fleet[index-1]=Ship(name, size);
            i++;
        }
    if (i!=FLEET_SIZE) {
        cout << "ERROR: ship info file provided wrong amount of ships";
        exit(2);
    }
    stream.close();
}
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]) {
    for (int x = 0; x < PLAYER_COUNT; x++)
        initFleet(players[x]);
    modeSelect(players);
    displayBoards(players);
    for (int h=0; h<PLAYER_COUNT; h++)
        if (players[h].isHuman) {
            for (int i=0; i<FLEET_SIZE; i++) {
                placeShip(players[h], i);
                for (int j=0; j<players[h].fleet[i].size; j++)
                    players[h].board[players[h].fleet[i].points[j].x][players[h].fleet[i].points[j].y] = 'S';
                displayBoard(players[h]);
            }
        } else {
            for (int i=0; i<FLEET_SIZE; i++) {
                placeShipRandom(players[h], i);
                for (int j=0; j<players[h].fleet[i].size; j++)
                    players[h].board[players[h].fleet[i].points[j].x][players[h].fleet[i].points[j].y] = 'S';
            }
        }
}
void modeSelect(PlayerBoard (&players)[PLAYER_COUNT]) {
    int mode;
    while (true) {
        cout << "Would you like to play 2 player or vs. AI?\n1. 2 player\n2. vs. AI\n";
        cin >> mode;
        if (mode==1 || mode==2 || mode==3)
            break;
        cout << "ERROR: invalid mode.\n";
    }
    if (mode==1) {
        players[0].name = "Player 1";
        players[1].name = "Player 2";
    } else if (mode==2) {
        players[0].name = "Player 1";
        players[1].name = "Player 2 (Computer)";
        players[1].isHuman = false;
    } else {
        players[0].name = "Player 1 (Computer)";
        players[1].name = "Player 2 (Computer)";
        players[0].isHuman=false;
        players[1].isHuman=false;
    }
}
void placeShip(PlayerBoard &board, int index) {
    char orientation;
    getValidShipInfo(board.fleet[index].points[0], orientation, board, index);
}
void placeShipRandom(PlayerBoard &board, int index) {
    int x,y;
    char orientation;
    do {
        orientation=genRandom(0,1)?'h':'v';
        if (orientation=='h') { // 0 is vertical 1 is horizontal
            // horizontal
            x=genRandom(0,BOARD_WIDTH-board.fleet[index].size);
            y=genRandom(0, BOARD_HEIGHT-1);
        } else {
            // vertical
            x=genRandom(0,BOARD_WIDTH-1);
            y=genRandom(0, BOARD_HEIGHT-board.fleet[index].size);
        }
    } while (spaceOccupied(board, Point(x,y), index, orientation));
    for (int i=0; i<board.fleet[index].size; i++)
        if (orientation=='h')
            board.fleet[index].points.emplace_back(x+i,y);
        else
            board.fleet[index].points.emplace_back(x,y+i);
}
void getValidShipInfo(Point &point, char &orientation, PlayerBoard &board, int index) {

    int x;
    char y;
    bool occupied=false;
    cin.sync();
    do {
        while (true) {
            while (true) {
                cout << "Please input a vertical coordinate to place your " << board.fleet[index].name << " at (A-" << char(BOARD_HEIGHT+'A'-1) << ") :\n";
                cin >> y;
                if (islower(y)) y=toupper(y);
                y-='A';
                if (y>=0&&y<BOARD_HEIGHT)
                    break;
                cout << "ERROR: invalid coordinate\n";
                cin.sync();
            }
            while (true) {
                cout << "Please input a horizontal coordinate to place your " << board.fleet[index].name << " at (1-" << BOARD_WIDTH << ") :\n";
                cin >> x;
                x-=1;
                if (x>=0&&x<BOARD_WIDTH)
                    break;
                cout << "ERROR: invalid coordinate\n";
                cin.sync();
            }
            while (true) {
                cout << "Please input an orientation to place your " << board.fleet[index].name << " with (h/v)\n";
                cin >> orientation;
                if (orientation=='h'||orientation=='v')
                    break;
                cout << "ERROR: invalid orientation\n";
                cin.sync();
            }

            if (validPlacement(board, Point(x,y), index, orientation))
                break;
            cout << "ERROR: invalid placement\n";
        }

        occupied = spaceOccupied(board, Point(x,y), index, orientation);

        if (occupied)
            cout << "ERROR: space is occupied\n";

    } while (occupied);

    for (int i=0; i<board.fleet[index].size; i++)
        if (orientation=='h')
            board.fleet[index].points.emplace_back(x+i,y);
        else
            board.fleet[index].points.emplace_back(x,y+i);
}
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index) {
    int x, target=index?0:1;
    char y;
    displayBoards(players, index);
    do {
        while (true) {
            cout << "Please input a vertical coordinate to fire at (A-" << char(BOARD_HEIGHT+'A'-1) << ") :\n";
            cin.sync();
            cin >> y;
            if (islower(y)) y=toupper(y);
            y-='A';
            if (y>=0&&y<BOARD_HEIGHT)
                break;
            cout << "ERROR: invalid coordinate.\n";
            cin.sync();
        }
        while (true) {
            cout << "Please input a horizontal coordinate to fire at (1-" << BOARD_WIDTH << ") :\n";
            cin >> x;
            x-=1;
            if (x>=0&&x<BOARD_WIDTH)
                break;
            cout << "ERROR: invalid coordinate.\n";
            cin.sync();
        }
        if (players[target].board[x][y]=='X'||players[target].board[x][y]=='O') {
            cout << "ERROR: target has already been fired at.\n";
        }
    } while (players[target].board[x][y]=='X'||players[target].board[x][y]=='O');

    fire(players[target], Point(x,y));
}
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index) {

}
void fire(PlayerBoard &targetPlayer, Point targetPoint) {
    if (targetPlayer.board[targetPoint.x][targetPoint.y]=='S') {
        int index;
        for (int i=0; i<FLEET_SIZE; i++) {
            for (int j=0; j<targetPlayer.fleet[i].size; j++)
                if (targetPlayer.fleet[i].points[j].x==targetPoint.x && targetPlayer.fleet[i].points[j].y==targetPoint.y)
                    index=i;
        }

        if (++targetPlayer.fleet[index].hitcount==targetPlayer.fleet[index].size) {
            targetPlayer.fleet[index].alive=false;
            cout << "You hit and sunk " << targetPlayer.name << "\'s " << targetPlayer.fleet[index].name << "!\n";
        } else cout << "You hit " << targetPlayer.name << "\'s ship!\n";

        targetPlayer.board[targetPoint.x][targetPoint.y]='X';
    } else targetPlayer.board[targetPoint.x][targetPoint.y]='O';
}
bool spaceOccupied(PlayerBoard board, Point point, int index, char orientation) {
    for (int i=0; i<board.fleet[index].size; i++)
        if (orientation=='h') {
            if (board.board[point.x+i][point.y]=='S') return true;
        } else {
            if (board.board[point.x][point.y+i]=='S') return true;
        }

    return false;
}
bool spaceOccupied(PlayerBoard board, Point point) {
    if (isalnum(board.board[point.x][point.y]))
        return true;
    return false;
}
bool validPlacement(PlayerBoard board, Point point, int index, char orientation) {
    if (orientation=='v') {
        if (point.y+board.fleet[index].size-1>=BOARD_HEIGHT) return false;
    } else {
        if (point.x+board.fleet[index].size-1>=BOARD_WIDTH) return false;
    }
    return true;
}
bool lastAlive(PlayerBoard (&players)[PLAYER_COUNT]) {
    int numAlive=0;
    for (int i=0; i<PLAYER_COUNT; i++) {
        bool alive=false;
        for (int j=0; j<FLEET_SIZE; j++) {
            if (players[i].fleet[j].alive)
                alive=true;
        }
        if (alive) numAlive++;
    }
    if (numAlive==1)
        return true;
    else return false;
}
int genRandom(int a, int b) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(a,b);
    return dist(mt);
}