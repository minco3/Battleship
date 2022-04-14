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
void displayBoard(PlayerBoard player);
void displayEnemyBoard(PlayerBoard player);
void initFleet(PlayerBoard &board);
void playerSetup(PlayerBoard (&players)[PLAYER_COUNT], int mode);
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]);
void placeShip(PlayerBoard &board, int index);
void placeShipRandom(PlayerBoard &board, int index);
void getValidShipInfo(Point &point, char &orientation, PlayerBoard &board, int index);
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index);
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index);
void fire(PlayerBoard (&players)[PLAYER_COUNT], int target, int attacker, Point targetPoint);
void clearConsole();
bool spaceOccupied(PlayerBoard board, Point point, int index, char orientation);
bool firedAt(PlayerBoard player, Point point);
bool killed(PlayerBoard player, Point point);
bool validPlacement(PlayerBoard board, Point point, int index, char orientation);
pair<int,int> game(PlayerBoard (&players)[PLAYER_COUNT]);
int modeSelect();
int lastAlive(PlayerBoard (&players)[PLAYER_COUNT]);
int genRandom(int a, int b);
Point mostProbable(PlayerBoard targetPlayer);

int main() {

    int mode = modeSelect();

    if (mode!=3) {
        PlayerBoard players[2];
        for (int x=0; x<PLAYER_COUNT; x++)
            players[x].name="Player "+to_string(x+1);
        playerSetup(players, mode);
        boardSetup(players);
        pair<int,int> winner = game(players);
        cout << players[winner.first].name << " won the game in " << winner.second << " turns!\n";
        displayBoards(players);
    } else { // simulate ai mode
        int times;
        vector<int> turns;
        cout << "How many games would you like to simulate the game?\n";
        cin >> times;
        char ans;
        cout << "Would you like to see each winning board?\n";
        cin >> ans;
        if (isupper(ans)) ans=tolower(ans);

        for (int i=0; i<times; i++) {
            PlayerBoard players[2];
            for (int x=0; x<PLAYER_COUNT; x++)
                players[x].name="Player "+to_string(x);
            playerSetup(players, mode);
            boardSetup(players);
            pair<int,int> winner = game(players);
            turns.push_back(winner.second);
            if (ans=='y') {
                cout<<"Board " << i << '/' << times << ":\n";
                displayBoards(players);
            }
            else if (i==times-1) {
                clearConsole();
                cout << "The last board: \n";
                displayBoards(players);
            }
        }
        int  average=0, min=turns[0], max=turns[0];
        for (int i=0; i<turns.size(); i++) {
            average+=turns[i];
            if (turns[i]<min)
                min=turns[i];
            if (turns[i]>max)
                max=turns[i];
        }
        cout << "Simulation complete!\n"
             << "After " << times << " iterations the winning computer took on average " << (double)average/turns.size() << "turns to win the game!\n"
             << "The fastest game took " << min << " turns and the longest game took " << max << " turns.\n";
    }
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
void displayEnemyBoard(PlayerBoard player) {
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
            cout << ((isalpha(player.board[x][y])&&player.board[x][y]!='S') ? player.board[x][y] : ' ') << " | ";
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
void playerSetup(PlayerBoard (&players)[PLAYER_COUNT], int mode) {
    if (mode == 2) {
        players[1].isHuman=false;
        players[1].name+=" (Computer)";
    }
    if (mode == 3) {
        players[0].isHuman=false;
        players[0].name+=" (Computer)";
        players[1].isHuman=false;
        players[1].name+=" (Computer)";
    }
}
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]) {
    for (int x = 0; x < PLAYER_COUNT; x++)
        initFleet(players[x]);
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
    displayEnemyBoard(players[target]);
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
        if (firedAt(players[target],Point(x,y))) {
            cout << "ERROR: target has already been fired at.\n";
        }
    } while (firedAt(players[target],Point(x,y)));

    fire(players, target, index, Point(x,y));
}
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index) {
    int target=index?0:1;
    fire(players, target, index, mostProbable(players[target]));
}
void fire(PlayerBoard (&players)[PLAYER_COUNT], int target, int attacker, Point targetPoint) {
    if (players[target].board[targetPoint.x][targetPoint.y]=='S') {
        int index;
        for (int i=0; i<FLEET_SIZE; i++) {
            for (int j=0; j<players[target].fleet[i].size; j++)
                if (players[target].fleet[i].points[j].x==targetPoint.x && players[target].fleet[i].points[j].y==targetPoint.y)
                    index=i;
        }

        if (++players[target].fleet[index].hitcount==players[target].fleet[index].size) {
            players[target].fleet[index].alive=false;
            cout << players[attacker].name << " hit and sunk " << players[target].name << "\'s " << players[target].fleet[index].name << "!\n";
        } else cout << players[attacker].name << " hit " << players[target].name << "\'s ship!\n";

        players[target].board[targetPoint.x][targetPoint.y]='X';
    } else if (players[target].board[targetPoint.x][targetPoint.y]=='X') {
    } else players[target].board[targetPoint.x][targetPoint.y]='O';
}
void clearConsole() {
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
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
bool firedAt(PlayerBoard player, Point point) {
    if (player.board[point.x][point.y]=='X'||player.board[point.x][point.y]=='O')
        return true;
    return false;
}
bool killed(PlayerBoard player, Point point) {
    for (int a=0; a<FLEET_SIZE; a++)
        if (!player.fleet[a].alive)
            for (int b=0; b<player.fleet[a].size; b++)
                if (player.fleet[a].points[b].x==point.x&&player.fleet[a].points[b].y==point.y)
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
pair<int,int> game(PlayerBoard (&players)[PLAYER_COUNT]) {
    int winner, turns=0;
    while (true) {
        for (int i = 0; i < PLAYER_COUNT; i++) {
            if (players[i].isHuman) {
                displayBoard(players[i]);
                playerTurn(players, i);
            } else {
                computerTurn(players, i);
            }
            winner = lastAlive(players);
            if (winner!=-1) return {winner,++turns};
        }
        turns++;
    }
}
int modeSelect() {
    int mode;
    while (true) {
        cin.sync();
        cout << "Would you like to play 2 player or vs. AI?\n1. 2 player\n2. vs. AI\n3. simulation mode (AI vs AI)\n";
        cin >> mode;
        if (mode>0&&mode<4)
            break;
        cout << "ERROR: invalid mode.\n";
    }
    return mode;
}
int lastAlive(PlayerBoard (&players)[PLAYER_COUNT]) {
    int numAlive=0, temp;
    for (int i=0; i<PLAYER_COUNT; i++) {
        bool alive=false;
        for (int j=0; j<FLEET_SIZE; j++) {
            if (players[i].fleet[j].alive)
                alive=true;
        }
        if (alive) {
            numAlive++;
            temp = i;
        }
    }
    if (numAlive==1)
        return temp;
    else return -1;
}
int genRandom(int a, int b) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(a,b);
    return dist(mt);
}
Point mostProbable(PlayerBoard targetPlayer) {
    
    int hitCount=0, killedHitCount=0, shipLengths[FLEET_SIZE]{}, ships[BOARD_WIDTH][BOARD_HEIGHT]{};;
    Point point(0,0);
    
    for (int x=0; x<BOARD_WIDTH; x++)
        for (int y=0; y<BOARD_HEIGHT; y++)
            if (targetPlayer.board[x][y] == 'X')
                hitCount++;

    for (int i=0; i<FLEET_SIZE; i++)
        if (targetPlayer.fleet[i].alive)
            shipLengths[i]=targetPlayer.fleet[i].size;
        else
            killedHitCount+=targetPlayer.fleet[i].size;

    if (hitCount>killedHitCount) {
        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (targetPlayer.board[x][y] != 'O'&&!killed(targetPlayer, Point(x,y)))
                    for (int i=0; i<FLEET_SIZE; i++)
                        if (shipLengths[i] != 0) {
                            //vertical
                            bool valid=true, occupied=false, beenHit=false;
                            if (y + shipLengths[i]-1>=BOARD_HEIGHT) valid=false;
                            if (valid) {
                                for (int j= 1; j<shipLengths[i]; j++)
                                    if (targetPlayer.board[x][y+j] == 'O'||killed(targetPlayer, Point(x,y+j))) occupied=true;
                                if (!occupied) {
                                    for (int g = 0; g < shipLengths[i]; g++)
                                        if (targetPlayer.board[x][y + g] == 'X') beenHit = true;
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
                                    if (targetPlayer.board[x+j][y] == 'O'||killed(targetPlayer, Point(x+j,y))) occupied = true;
                                if (!occupied) {
                                    for (int g = 0; g < shipLengths[i]; g++)
                                        if (targetPlayer.board[x + g][y] == 'X') beenHit = true;
                                    if (beenHit)
                                        for (int h = 0; h < shipLengths[i]; h++)
                                            ships[x + h][y]++;
                                }
                            }
                        }

        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (ships[x][y] > ships[point.x][point.y]&&targetPlayer.board[x][y]!='X')
                    point = Point(x, y);
    } else {
        for (int x = 0; x < BOARD_WIDTH; x++)
            for (int y = 0; y < BOARD_HEIGHT; y++)
                if (targetPlayer.board[x][y] != 'O' && targetPlayer.board[x][y] != 'X')
                    for (int i = 0; i < FLEET_SIZE; i++)
                        if (shipLengths[i] != 0) {
                            //vertical
                            bool valid = true, occupied = false;
                            if (y + shipLengths[i] - 1 >= BOARD_HEIGHT) valid = false;
                            if (valid) {
                                for (int j = 0; j < shipLengths[i]; j++)
                                    if (firedAt(targetPlayer, Point(x, y + j))) occupied = true;
                                if (!occupied)
                                    for (int h = 0; h < shipLengths[i]; h++)
                                        ships[x][y + h]++;
                            }
                            //horizontal
                            valid = true, occupied = false;
                            if (x + shipLengths[i] - 1 >= BOARD_WIDTH) valid = false;
                            if (valid) {
                                for (int j = 0; j < shipLengths[i]; j++)
                                    if (firedAt(targetPlayer, Point(x+j, y))) occupied = true;
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