#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define GRID_SIZE 100
#define NUM_THREADS 4

int grid[GRID_SIZE][GRID_SIZE];
int newGrid[GRID_SIZE][GRID_SIZE];
pthread_mutex_t mutex;
pthread_cond_t generationDone;
int generationCount = 0;

void printGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0)
                printf(".");
            else
                printf("*");
        }
        printf("\n");
    }
    printf("\n");
}

int countLiveNeighbors(int x, int y) {
    int count = 0;
    int neighbors[] = {-1, 0, 1};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!(neighbors[i] == 0 && neighbors[j] == 0)) {
                int row = x + neighbors[i];
                int col = y + neighbors[j];

                if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
                    if (grid[row][col] == 1)
                        count++;
                }
            }
        }
    }

    return count;
}

void* updateGrid(void* threadId) {
    long tid = (long)threadId;
    int chunkSize = GRID_SIZE / NUM_THREADS;
    int startRow = tid * chunkSize;
    int endRow = (tid + 1) * chunkSize;

    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int liveNeighbors = countLiveNeighbors(i, j);

            if (grid[i][j] == 1) {
                if (liveNeighbors < 2 || liveNeighbors > 3)
                    newGrid[i][j] = 0;
                else
                    newGrid[i][j] = 1;
            } else {
                if (liveNeighbors == 3)
                    newGrid[i][j] = 1;
                else
                    newGrid[i][j] = 0;
            }
        }
    }

    pthread_mutex_lock(&mutex);
    generationCount++;
    if (generationCount == NUM_THREADS) {
        generationCount = 0;
        pthread_cond_signal(&generationDone);
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}
int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&generationDone, NULL);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = rand() % 2;
        }
    }

    int generations = 10;

    printf("Generation 0:\n");
    printGrid();

    for (int gen = 1; gen <= generations; gen++) {    
        for (long i = 0; i < NUM_THREADS; i++) {
            pthread_create(&threads[i], NULL, updateGrid, (void*)i);
        }

        pthread_mutex_lock(&mutex);
        while (generationCount != NUM_THREADS) {
            pthread_cond_wait(&generationDone, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grid[i][j] = newGrid[i][j];
            }
        }

        printf("Generation %d:\n", gen);
        printGrid();
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&generationDone);

    return 0;
}

