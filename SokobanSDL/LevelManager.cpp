#include "LevelManager.h"

void LevelManager::LoadLevel()
{
    string path = "Levels/SokobanLevel" + to_string(currentLevel) + ".txt";

    LevelFile.open(path);

    cout << "NEW MAP LOADING...!" << endl;

    char c;
    for(int row = 0; row < TILE_ROWS; row++)
    {
        for(int col = 0; col < TILE_COLS; col++)
        {
            LevelFile >> c;
            LevelMap[col][row] = c;
            cout << LevelMap[col][row];
        }
        cout << endl;
    }
    LevelFile.close();
}

void LevelManager::UpdateLevel()
{
    currentLevel++;

}
