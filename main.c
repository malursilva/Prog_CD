/* 
* Rainbow Game - Versão OpenMP
* Maria Luíza Rodrigues da Silva - 120527
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#define N 2048
#define PRINT_SIZE 50
#define MAX_TURNS 5
#define MAX_THREADS 4

void printGrid(float **grid) {
    int alive = 0;

    for(int i=0; i<PRINT_SIZE; i++) {
        for(int j=0; j<PRINT_SIZE; j++) {
            if(grid[i][j] > 0.00000) {
                printf("o ");
                alive++;
            } else {
                printf("x ");
            }
            
        }
        printf("\n");
    }

    printf("Alive cells: %d\n", alive);
}

int calcMinIndex(int index) {
    if(index - 1 < 0) return N-1;
    else return index - 1;
}

void drawGlider(float **grid) {
    int lin = 1, col = 1;

    grid[lin  ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;
}

void drawRPentomino(float **grid) {
    int lin = 10, col = 30;

    grid[lin  ][col+1] = 1.0;
    grid[lin  ][col+2] = 1.0;
    grid[lin+1][col  ] = 1.0;
    grid[lin+1][col+1] = 1.0;
    grid[lin+2][col+1] = 1.0;
}

void initializeGrid(float **grid) {
    for(int i=0; i<N; i++) {
        grid[i] = (float *) malloc(N * sizeof(float));
        for(int j=0; j<N; j++) {
            grid[i][j] = 0.000;
        }
    }
    drawGlider(grid);
    drawRPentomino(grid);
}

int getNeighbors(float **grid, int i, int j, float *mean) {
    float sum = 0.000;
    int li, ci, k, l, count = 0;
    int lineIndex[3] = {calcMinIndex(i), i, (i+1) % N};
    int colIndex[3] = {calcMinIndex(j), j, (j+1) % N};
    
    for(k=0; k<3; k++) {
        for(l=0; l<3; l++) {
            if(!(lineIndex[k]==i && colIndex[l]==j)) { 
                sum += grid[lineIndex[k]][colIndex[l]]; // Aproveita para achar a média dos vizinhos
                
                if(grid[lineIndex[k]][colIndex[l]] > 0.000) // Acima de 0.000 já é considerado viva 
                    count++;
                
            }
        }
    }
    (*mean) = sum / 8; 

    return count;
}

float getMean(float **grid, int i, int j) {
    int k, l;
    int minL = calcMinIndex(i);
    int minC = calcMinIndex(j);
    int maxL = (i+1) % N;
    int maxC = (j+1) % N;
    float sum = 0.000;

    for(k=minL; k=maxL; k++) {
        for(l=minC; l=minL; l++) {
            if(k!=i && l!=j) { 
                sum += grid[k][l];
            }
        }
    }

    return sum / 8;
}

float getNewCellState(float **grid, int i, int j) {
    float cState = grid[i][j];
    float nState = 0.000;
    float mean;
    int numNeighbors = getNeighbors(grid, i, j, &mean);
    
    // Os casos em que a célula fica viva: 1- Se já estiver viva e ter 2 ou 3 vizinhos vivos; 2- Se estiver morta e ter 3 vizinhos vivos
    if(((cState > 0.000) && (numNeighbors == 2 || numNeighbors == 3)) || (cState == 0.000 && numNeighbors == 3)){
        nState = mean;
    }

    return nState;
}

float** runGeneration(float **grid) {
    int i, j;
    float **newgrid;

    newgrid = (float **) malloc(N * sizeof(float *));
    initializeGrid(newgrid);

    //omp_set_num_threads(MAX_THREADS);
    
    //#pragma omp parallel default(none) private(j) shared(i, newgrid, grid) 
    //{
    //    #pragma omp for
        for(i=0; i<N; i++) {
            for(j=0; j<N; j++) {
                newgrid[i][j] = getNewCellState(grid, i, j);
            }
        }
    //}
    return newgrid;
}

int main() {
    float **grid;
    struct timeval inicio, final;
    int tmili;
    
    gettimeofday(&inicio, NULL);
    grid = (float **) malloc(N * sizeof(float *));      
    initializeGrid(grid);

    for(int i=0; i<MAX_TURNS; i++) {
        grid = runGeneration(grid);
        printf("------------Generation %d------------\n", i+1);
        printGrid(grid);
    }
    
    gettimeofday(&final, NULL);
    tmili = (int) (1000 * (final.tv_sec - inicio.tv_sec) + (final.tv_usec - inicio.tv_usec) / 1000);

    //printGrid(grid);
    printf("Tempo decorrido: %d milisegundos\n", tmili);
}