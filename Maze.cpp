#include "Maze.h"



pair<int, int> find_neigh(int row, int col) {
    pair<int, int> p;
    vector<pair<int, int>> neighbors;
    vector<pair<int, int>> directions;

    directions.push_back(make_pair(1, -1)); // sus stanga
    directions.push_back(make_pair(1, 1)); // sus dreapta
    directions.push_back(make_pair(-1, -1)); // jos stanga
    directions.push_back(make_pair(-1, 1)); // jos dreapta
    directions.push_back(make_pair(0, 1)); // dreapta
    directions.push_back(make_pair(0, -1)); // stanga
    directions.push_back(make_pair(-1, 0)); // jos
    directions.push_back(make_pair(1, 0)); // sus

    for (auto i : directions) {
        int neigh_row = row + i.first;
        int neigh_col = col + i.second;
        if (neigh_row > 0 && neigh_row < GRID_SIZE) {
            if (neigh_col > 0 && neigh_col < GRID_SIZE) {
                neighbors.push_back(make_pair(neigh_row, neigh_col));
            }
        }
    }
    if (neighbors.empty()) {
        return make_pair(row, col);
    }
    auto it = neighbors.begin();
    std::advance(it, rand() % neighbors.size());
    int rand_row = it->first;
    int rand_col = it->second;
    return make_pair(rand_row, rand_col);
}

bool isBorder(pair<int, int> poz) {
    if (poz.first == 0 || poz.first == GRID_SIZE - 1 ||
        poz.second == 0 || poz.second == GRID_SIZE - 1) {
        return true;
    }
    return false;
}

// 1 e zid
// 0 e cale libera
// 2 o sa fie inamic
pair<vector<vector<int>>, set<pair<int, int>>> GenerateMaze() {
    srand(time(0));
    cout << "generate maze" << endl;
    int gridSize = GRID_SIZE;
    set<pair<int, int>> validPlayerPositions;
    vector<vector<int>> grid(GRID_SIZE);
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[i].resize(GRID_SIZE);
    }

    for (int i = 0; i < gridSize; i++)
        for (int j = 0; j < gridSize; j++)
            grid[i][j] = 1;

    for (int i = 0; i < gridSize; i ++) {
        for (int j = 0; j < gridSize; j ++) {
            pair<int, int> p = find_neigh(i, j);
            grid[p.first][p.second] = 0;
            if (!isBorder(p)) {
                validPlayerPositions.insert(p);
            }
        }
    }
    return make_pair(grid, validPlayerPositions);
}
    