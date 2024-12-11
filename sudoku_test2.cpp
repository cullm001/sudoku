#include <iostream>
#include <chrono>
#include <pthread.h>
#include <vector>
#include <cstdlib>

using namespace std;

//CREATED A BARRIER
pthread_barrier_t barrier;

struct ThreadsData {
    int (*grid)[9];
    int row;
    int col;
    int start_value;
    int end_value;
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
    //Solves the entire box
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
    for (int value = data->start_value; value <= data->end_value; ++value) {
        if (validate_grid(data->grid, data->row, data->col, value)) {
            pthread_mutex_lock(data->mutex);
            data->results[value - 1] = true; // Mark as valid
            pthread_mutex_unlock(data->mutex);
        }
    }
    delete data; // Free dynamically allocated memory
    pthread_exit(nullptr);
}

bool solve(int grid[9][9], int row, int col, int chunk_size) {
    if (row == 9) 
        return true;

    if (col == 9) 
        return solve(grid, row + 1, 0, chunk_size);
    if (grid[row][col] != 0) 
        return solve(grid, row, col + 1, chunk_size);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    vector<pthread_t> threads;
    bool results[9] = {false};

    for (int start_value = 1; start_value <= 9; start_value += chunk_size) {
        int end_value = min(start_value + chunk_size - 1, 9);
        pthread_t thread;
        ThreadsData* task = new ThreadsData{grid, row, col, start_value, end_value, results, &mutex};
        pthread_create(&thread, nullptr, threadFunction, task);
        threads.push_back(thread);
    }

    for (auto& thread : threads) {
        pthread_join(thread, nullptr);
    }

    for (int i = 0; i < 9; ++i) {
        if (results[i]) {
            grid[row][col] = i + 1;
            if (solve(grid, row, col + 1, chunk_size)) {
                pthread_mutex_destroy(&mutex);
                return true;
            }
            grid[row][col] = 0;
        }
    }

    pthread_mutex_destroy(&mutex);
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <chunk_size>\n";
        return 1;
    }

    int chunk_size = atoi(argv[1]);
    if (chunk_size < 1 || chunk_size > 9) {
        cout << "Chunk size must be between 1 and 9.\n";
        return 1;
    }

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
    if (solve(grid, 0, 0, chunk_size)) {
        auto end = chrono::high_resolution_clock::now();
        printGrid(grid);
        cout << "Time taken: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " microseconds\n";
    } else {
        cout << "Unsolvable\n";
    }

    return 0;
}
