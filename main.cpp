#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "battleship.h"
using namespace std;

static const string SHIP_INFO_PATH = "ships.txt";


void displayBoards(PlayerBoard players[PLAYER_COUNT]);
void initFleet(PlayerBoard (&players)[PLAYER_COUNT]);
void playerSetup(PlayerBoard (&players)[PLAYER_COUNT], int mode);
void boardSetup(PlayerBoard (&players)[PLAYER_COUNT]);
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index);
void computerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index);
void fire(PlayerBoard (&players)[PLAYER_COUNT], int target, int attacker, Point targetPoint);
void clearConsole();
pair<int,int> game(PlayerBoard (&players)[PLAYER_COUNT]);
int modeSelect();
int lastAlive(PlayerBoard (&players)[PLAYER_COUNT]);

int main() {

    int mode = modeSelect();

    if (mode!=3) {
        PlayerBoard players[2];
        for (int x = 0; x < PLAYER_COUNT; x++)
            players[x].name = "Player " + to_string(x + 1);
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
void initFleet(PlayerBoard (&players)[PLAYER_COUNT]) {
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

    for (int x=0; x<PLAYER_COUNT; x++)
        for (int i=0; i<FLEET_SIZE; i++)
            players[x].fleet[index[i]-1]=Ship(name[i], size[i]);

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
        initFleet(players);
    for (int h=0; h<PLAYER_COUNT; h++)
        if (players[h].isHuman) {
            for (int i=0; i<FLEET_SIZE; i++) {
                players[h].placeShip(i);
                for (int j=0; j<players[h].fleet[i].size; j++)
                    players[h].board[players[h].fleet[i].points[j].x][players[h].fleet[i].points[j].y] = 'S';
                players[h].display();
            }
        } else {
            for (int i=0; i<FLEET_SIZE; i++) {
                players[h].placeShipRandom(i);
                for (int j=0; j<players[h].fleet[i].size; j++)
                    players[h].board[players[h].fleet[i].points[j].x][players[h].fleet[i].points[j].y] = 'S';
            }
        }
}
void playerTurn(PlayerBoard (&players)[PLAYER_COUNT], int index) {
    int x, target=index?0:1;
    char y;
    players[target].displayAsEnemy();
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
    if (players[target].board[targetPoint.x][targetPoint.y]=='S') {
        int index;
        for (int i=0; i<FLEET_SIZE; i++) {
            for (int j=0; j<players[target].fleet[i].size; j++)
                if (players[target].fleet[i].points[j].x==targetPoint.x && players[target].fleet[i].points[j].y==targetPoint.y)
                    index=i;
        }
        clearConsole();
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
