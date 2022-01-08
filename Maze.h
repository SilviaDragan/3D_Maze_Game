#pragma once
#include "tema2.h"
#define GRID_SIZE 10

using namespace std;


pair<int, int> find_neigh(int row, int col);
bool isBorder(pair<int, int> poz);
pair<vector<vector<int>>, set<pair<int, int>>> GenerateMaze();