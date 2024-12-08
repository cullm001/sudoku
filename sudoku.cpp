#include <iostream>
#include <chrono>
using namespace std;

void printGrid(int grid[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

bool validate_grid(int grid[9][9], int row, int col, int value) {
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] == value) 
            return false;
    }
    for (int i = 0; i < 9; i++) {
        if (grid[i][col] == value) 
            return false;
    }
    int box_row = row - row % 3;
    int box_col = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[box_row + i][box_col + j] == value) 
                return false;
        }
    }
    return true;
}

bool solve(int grid[9][9], int row, int col) {
    if (row == 9) 
        return true;

    if (col == 9) 
        return solve(grid, row + 1, 0);
    if (grid[row][col] != 0) 
        return solve(grid, row, col + 1);

    for (int i = 1; i < 10; i++) {
        if (validate_grid(grid, row, col, i)) {
            grid[row][col] = i;
            if (solve(grid, row, col + 1)) 
                return true;
            grid[row][col] = 0;
        }
    }
    return false;
}

int main() {
    int grid[9][9] = { 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    auto start = chrono::high_resolution_clock::now();
    if (solve(grid, 0, 0)) {
        auto end = chrono::high_resolution_clock::now();
        printGrid(grid);
        cout << "Time taken: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " microseconds\n";
    } else {
        cout << "Unsolvable\n";
    }

    return 0;
}
