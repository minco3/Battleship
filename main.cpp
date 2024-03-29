/*
I, Marco Miralles, do hereby certify that I have derived no assistance for this project or examination from any sources whatever, whether oral, written, or in print.
battleship game
Input: file with ships and commands
Output: board
*/
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "includes/battleship/battleship.h"
using namespace std;

static const string SHIP_INFO_PATH = "ships.txt";

bool logging = true;

PlayerBoard initFleet(); //returns a temp player with the fleet from ships.txt
void displayBoards(PlayerBoard players[PLAYER_COUNT]); // displays both boards without ships
void playerSetup(PlayerBoard (&players)[PLAYER_COUNT], int mode); // sets the value of the isHuman bool in the PlayerBoard class and changes the name based on the game mode
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]); // handles placing ships for each player
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index); // asks the player where they want to fire
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index); // fires at the point generated by mostProbable()
void fire(PlayerBoard (&players)[PLAYER_COUNT], int target, int attacker, Point targetPoint); // handles firing logic
void clearConsole(); // clears the console
pair<int,int> game(PlayerBoard (&players)[PLAYER_COUNT]); // handles turns
int modeSelect(); // simple input validation for the mode selection
int lastAlive(PlayerBoard (&players)[PLAYER_COUNT]); // used to tell if the game is over
void simulate(); // simulation mode

int main() {

    int mode = modeSelect();

    if (mode!=3) {
        PlayerBoard players[2], temp=initFleet();
        for (int x=0; x<PLAYER_COUNT; x++)
            players[x]=temp;

        for (int x = 0; x < PLAYER_COUNT; x++)
            players[x].name = "Player " + to_string(x + 1);
        playerSetup(players, mode);
        boardSetup(players);
        clearConsole();
        pair<int,int> winner = game(players);
        displayBoards(players);
        cout << players[winner.first].name << " won the game in " << winner.second << " turns!\n";
    } else if (mode==3) // simulate ai mode
        simulate();
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
                cout << (isalpha(players[a].board[x][y])&&players[a].board[x][y]!='S' ? players[a].board[x][y]: ' ') << " | ";

            if (a!=PLAYER_COUNT-1)
                cout << setw(BOARD_OFFSET) << ' ';
            else cout << '\n';
        }
    }
}
PlayerBoard initFleet() {
    PlayerBoard player;
    string a;
    ifstream stream(SHIP_INFO_PATH);
    if (!stream.is_open()) {
        cout << "ERROR: Error opening ship info file, check path";
        exit(1);
    }

    int i=0, index[FLEET_SIZE], size[FLEET_SIZE];
    string name[FLEET_SIZE];
    while (getline(stream,a))
        if (isdigit(a[0])) {
            stringstream basicStringstream(a);
            basicStringstream >> index[i] >> name[i] >> size[i];
            i++;
        }

    if (i!=FLEET_SIZE) {
        cout << "ERROR: ship info file provided wrong amount of ships";
        exit(2);
    }

        for (int i=0; i<FLEET_SIZE; i++)
            player.fleet[index[i]-1]=Ship(name[i], size[i]);

    stream.close();
    return player;
}
void playerSetup(PlayerBoard (&players)[PLAYER_COUNT], int mode) {
    if (mode == 2) {
        players[1].isHuman=false;
        players[1].name+=" (Computer)";
    }
    if (mode >= 3) {
        players[0].isHuman=false;
        players[0].name+=" (Computer)";
        players[1].isHuman=false;
        players[1].name+=" (Computer)";
    }
}
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]) {
    for (int h=0; h<PLAYER_COUNT; h++)
        if (players[h].isHuman) {
            players[h].display();
            for (int i=0; i<FLEET_SIZE; i++) {
                players[h].placeShip(i);
                players[h].display();
            }
        } else {
            for (int i=0; i<FLEET_SIZE; i++) {
                players[h].placeShipRandom(i);
            }
        }
}
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index) {
    int x, target=index?0:1;
    char y;
    displayBoards(players);
    cout << players[index].name << ", \n";
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
            cin.ignore(10000,'\n');
        }
        while (true) {
            cout << "Please input a horizontal coordinate to fire at (1-" << BOARD_WIDTH << ") :\n";
            cin >> x;
            x-=1;
            if (x>=0&&x<BOARD_WIDTH)
                break;
            cout << "ERROR: invalid coordinate.\n";
            cin.sync();
            cin.ignore(10000,'\n');
        }
        if (players[target].isFiredAt(Point(x, y))) {
            cout << "ERROR: target has already been fired at.\n";
        }
    } while (players[target].isFiredAt(Point(x, y)));

    fire(players, target, index, Point(x,y));
}
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index) {
    int target=index?0:1;
    fire(players, target, index, players[target].mostProbable());
}
void fire(PlayerBoard (&players)[PLAYER_COUNT], int target, int attacker, Point targetPoint) {
    if (players[target].board[targetPoint.x][targetPoint.y]!='S'&&players[target].board[targetPoint.x][targetPoint.y]!='X'){
        players[target].board[targetPoint.x][targetPoint.y]='O';
        return;
    }

    if (players[target].board[targetPoint.x][targetPoint.y]=='X') return;

    int index;
    for (int i=0; i<FLEET_SIZE; i++) {
        for (int j=0; j<players[target].fleet[i].size; j++)
            if (players[target].fleet[i].points[j].x==targetPoint.x && players[target].fleet[i].points[j].y==targetPoint.y)
                index=i;
    }

    players[target].fleet[index].hitcount++; // hit registered

    if (players[target].fleet[index].hitcount==players[target].fleet[index].size) { // checking if alive
        players[target].fleet[index].alive=false;
        if (logging) cout << players[attacker].name << " hit and sunk " << players[target].name << "\'s " << players[target].fleet[index].name << "!\n";
    } else if (logging) cout << players[attacker].name << " hit " << players[target].name << "\'s ship!\n";

    players[target].board[targetPoint.x][targetPoint.y]='X';
}
void clearConsole() {
    for (int x=0; x<100; x++)
        cout << '\n';
}
pair<int,int> game(PlayerBoard (&players)[PLAYER_COUNT]) {
    int winner, turns=0;
    while (true) {
        for (int i = 0; i < PLAYER_COUNT; i++) {
            if (players[i].isHuman) {
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
        cin.sync();
        cin.ignore(10000,'\n');
    }
    return mode;
}
int lastAlive(PlayerBoard (&players)[PLAYER_COUNT]) {
    int numAlive=0, winner;
    for (int i=0; i<PLAYER_COUNT; i++) {
        bool alive=false;
        for (int j=0; j<FLEET_SIZE; j++) {
            if (players[i].fleet[j].alive)
                alive=true;
        }
        if (alive) {
            numAlive++;
            winner = i;
        }
    }
    if (numAlive==1)
        return winner;
    else return -1;
}

void simulate() {
    logging = false;
        int times=0, progress=0;
        vector<int> turns;
        cout << "\nHow many times would you like to simulate the game?\n";
        cin >> times;

        PlayerBoard temp = initFleet();

        for (int i=0; i<times; i++) {
            PlayerBoard players[2];
            for (int x=0; x<PLAYER_COUNT; x++)
                players[x]=temp;
            playerSetup(players, 3);
            boardSetup(players);
            pair<int,int> winner = game(players);
            turns.push_back(winner.second);

            if (((int(double(i)/times*25))+1)>progress) {
                progress = ((double)i/times*25)+1;
                string l(progress,'|'), r(25-l.length(), ' ');
                cout << "\rProgress [" << l << r << ']';
            }
        }
        int  average=0, min=turns[0], max=turns[0];

        for (int turn : turns) {
            average+=turn;
            if (turn<min)
                min=turn;
            if (turn>max)
                max=turn;
        }
        cout << "\rSimulation complete!                \n"
             << "After " << times << " iterations the winning computer took on average " << (double)average/turns.size() << " turns to win the game!\n"
             << "The fastest game took " << min << " turns and the longest game took " << max << " turns.\n";
}

/*
Sample Run
Would you like to play 2 player or vs. AI?
1. 2 player
2. vs. AI
3. simulation mode (AI vs AI)
2
Please input a vertical coordinate to place your Carrier at (A-J) :
a1v
Please input a horizontal coordinate to place your Carrier at (1-10) :
Please input an orientation to place your Carrier with (h/v)
                 Your Board
    1   2   3   4   5   6   7   8   9  10
  -----------------------------------------
A | S |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
B | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
C | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
D | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
E | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
F |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
G |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
H |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
I |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
J |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
Please input a vertical coordinate to place your Battleship at (A-J) :
a2v
Please input a horizontal coordinate to place your Battleship at (1-10) :
Please input an orientation to place your Battleship with (h/v)
                 Your Board
    1   2   3   4   5   6   7   8   9  10
  -----------------------------------------
A | S | S |   |   |   |   |   |   |   |   |
  -----------------------------------------
B | S | S |   |   |   |   |   |   |   |   |
  -----------------------------------------
C | S | S |   |   |   |   |   |   |   |   |
  -----------------------------------------
D | S | S |   |   |   |   |   |   |   |   | 
  -----------------------------------------
E | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
F |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
G |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
H |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
I |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
J |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
Please input a vertical coordinate to place your Cruiser at (A-J) :
a3v
Please input a horizontal coordinate to place your Cruiser at (1-10) :
Please input an orientation to place your Cruiser with (h/v)
                 Your Board
    1   2   3   4   5   6   7   8   9  10
  -----------------------------------------
A | S | S | S |   |   |   |   |   |   |   |
  -----------------------------------------
B | S | S | S |   |   |   |   |   |   |   |
  -----------------------------------------
C | S | S | S |   |   |   |   |   |   |   |
  -----------------------------------------
D | S | S |   |   |   |   |   |   |   |   |
  -----------------------------------------
E | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
F |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
G |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
H |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
I |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
J |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
Please input a vertical coordinate to place your Submarine at (A-J) :
a4v
Please input a horizontal coordinate to place your Submarine at (1-10) :
Please input an orientation to place your Submarine with (h/v)
                 Your Board
    1   2   3   4   5   6   7   8   9  10
  -----------------------------------------
A | S | S | S | S |   |   |   |   |   |   | 
  -----------------------------------------
B | S | S | S | S |   |   |   |   |   |   |
  -----------------------------------------
C | S | S | S | S |   |   |   |   |   |   |
  -----------------------------------------
D | S | S |   |   |   |   |   |   |   |   |
  -----------------------------------------
E | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
F |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
G |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
H |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
I |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
J |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
Please input a vertical coordinate to place your Destroyer at (A-J) :
a5v
Please input a horizontal coordinate to place your Destroyer at (1-10) :
Please input an orientation to place your Destroyer with (h/v)
                 Your Board
    1   2   3   4   5   6   7   8   9  10
  -----------------------------------------
A | S | S | S | S | S |   |   |   |   |   |
  -----------------------------------------
B | S | S | S | S | S |   |   |   |   |   |
  -----------------------------------------
C | S | S | S | S |   |   |   |   |   |   | 
  -----------------------------------------
D | S | S |   |   |   |   |   |   |   |   |
  -----------------------------------------
E | S |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
F |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
G |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
H |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
I |   |   |   |   |   |   |   |   |   |   |
  -----------------------------------------
J |   |   |   |   |   |   |   |   |   |   | 
  -----------------------------------------
*/