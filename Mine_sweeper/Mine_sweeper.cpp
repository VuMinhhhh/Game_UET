#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

int n;
class Mines_Game;

int dx[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
int dy[8] = {0, 1, -1, 0, -1, 1, -1, 1};

class Mines_Game {
public:
    char display_map[100][100];
    int mine_field[100][100];

public:
    void display() {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++)
                cout << display_map << " ";
            cout << endl;
        }
    }

};

void init (Mines_Game &game) {
    srand(time(0));
    cin >> n;
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            game.display_map[i][j] = '*';
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            game.mine_field[i][j] = rand() % 2;
}

void loang(Mines_Game &game, int i, int j) {
    int count = 0;
    for(int k = 0; k < 8; k++) {
        int inew = i + dx[k];
        int jnew = j + dy[k];
        if(inew >= 0 && inew < n && jnew >= 0 && jnew < n) {
            if(k < 4)
                count += game.mine_field[inew][jnew];
            if(game.mine_field[inew][jnew] == 0)
                loang(gmae, inew, jnew);
        }
    }
    if(count == 0)
        game.display_map[i][j] = ' ';
    else
        game.display_map[i][j] = char(count + 48);
}

int main() {
    init();

}
