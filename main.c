#include <math.h>
#include <time.h> 
#include "nonblock.h"

#define N 10
#define M 20

#define BLANK 0
#define SNAKE 1
#define FOOD 2

#define BLANK_S "_"
#define SNAKE_S "*"
#define FOOD_S "."

void display_field(int field[N][M])
{
    int i = 0;
    int j;
    printf("\n");
    while (i < N)
    {
        j = 0;
        while (j < M)
        {
            int cell = field[i][j];
            if (cell == BLANK) {
                printf(BLANK_S);
            } else if (cell == SNAKE) {
                printf(SNAKE_S);
            } else if (cell == FOOD) {
                printf(FOOD_S);
            }
                
            j = j + 1;
        }
        printf("\n");
        i = i + 1;
    }
}

void initialize_snake(int field[N][M], int x, int y) {
    int i = 0;
    int j;
    int food_x = rand() % N;
    int food_y = rand() % M;
    while (i < N)
    {
        j = 0;
        while (j < M)
        {
            if ((i == x) && (j == y)) {
                field[i][j] = SNAKE;
            } else if ((i == food_x) && (j == food_y)) {
                field[i][j] = FOOD;
            } else {
                field[i][j] = BLANK;
            }
            j = j + 1;
        }
        i = i + 1;
    }
}

void main_loop(int field[N][M])
{
    char ch;
    int ishit = 0;
    
    nonblock(1);
    while(!ishit)
    {
        usleep(1);
        ishit = kbhit();
        if (ishit != 0)
        {
            ch = fgetc(stdin);
            if (ch == 'q')
            {
                ishit = 1;
            } else {
                display_field(field);
                ishit = 0;
            }
        }
    }
    nonblock(0);
}

int main()
{
    srand(time(NULL)); // to make new random values for food each time
    
    int field[N][M];
    initialize_snake(field, N/2, M/2);
    display_field(field);
    main_loop(field);
    
    return 0;
}
