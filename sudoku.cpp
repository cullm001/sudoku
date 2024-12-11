#include <iostream>
#include <chrono>
#include <pthread.h>
#include <vector>

using namespace std;

//CREATED A BARRIER
pthread_barrier_t barrier;

struct ThreadsData {
    int (*grid)[9];
    int row;
    int col;
    int value;
    bool* results;        // Pointer to an array of results
    pthread_mutex_t* mutex; // Mutex for synchronization
};

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
    //checks row
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] == value) 
            return false;
    }
    //Checks column
    for (int i = 0; i < 9; i++) {
        if (grid[i][col] == value) 
            return false;
    }
    //Solves the entire boz
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

void* threadFunction(void* arg) {
    ThreadsData* data = (ThreadsData*)arg;
    bool valid = validate_grid(data->grid, data->row, data->col, data->value);

    if (valid) {
        pthread_mutex_lock(data->mutex);
        data->results[data->value - 1] = true; // Mark as valid
        pthread_mutex_unlock(data->mutex);
        cout << "VALID" << endl;
    }

    pthread_exit(nullptr);
}


bool solve(int grid[9][9], int row, int col) {
    if (row == 9) 
        return true;

    if (col == 9) 
        return solve(grid, row + 1, 0);
    if (grid[row][col] != 0) 
        return solve(grid, row, col + 1);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    vector<pthread_t> threads;
    vector<ThreadsData> tasks;
    bool results[9] = {false};

    for (int i = 1; i < 10; i++) {
            pthread_t thread;
            ThreadsData task = {grid, row, col, i, results, &mutex};
            tasks.push_back(task);
            pthread_create(&thread, nullptr, threadFunction, &tasks.back());
            threads.push_back(thread);
    }

    //WAITING FOR RESULTS
    for (auto& thread : threads) {
        pthread_join(thread, nullptr);
    }

    for (int i = 0; i < 9; ++i) {  // Change 10 to 9
        if (results[i]) {
            grid[row][col] = i + 1;  // Assign value i+1, since values are from 1 to 9
            if (solve(grid, row, col + 1)){
                return true;
            }   
            grid[row][col] = 0;
        }
    }

    pthread_mutex_destroy(&mutex);
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