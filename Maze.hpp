#pragma once
#include "tema2.h"
#define GRID_SIZE 8

pair<int, int> find_neighbors(int row, int col);
vector<vector<int>> GenerateMaze();
