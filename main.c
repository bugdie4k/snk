#include <math.h>
#include <time.h> 
#include "nonblock.h"

#define N 10
#define M 20

#define BLANKVAL 0 // blank value
#define SNAKEVAL 1 // snake value
#define FOODVAL 2  // food value

#define BLANKSTR "_" // blank string
#define SNAKESTR "*" // snake string
#define FOODSTR "."  // food string

struct Point {
    int x;
    int y;
};

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
            if (cell == BLANKVAL) {
                printf(BLANKSTR);
            } else if (cell == SNAKEVAL) {
                printf(SNAKESTR);
            } else if (cell == FOODVAL) {
                printf(FOODSTR);
            }
                
            j = j + 1;
        }
        printf("\n");
        i = i + 1;
    }
}

struct Point place_food(int field[N][M], struct Point old_food_point)
{
    int food_x = rand() % N;
    int food_y = rand() % M;

    field[food_x][food_y] = FOODVAL;
    field[old_food_point.x][old_food_point.y] = BLANKVAL;

    struct Point new_food_point;
    new_food_point.x = food_x;
    new_food_point.y = food_y;
    return new_food_point;
}

struct Point initialize_snake(int field[N][M], struct Point snake_point)
{
    int i = 0;
    int j;
    int food_x = rand() % N;
    int food_y = rand() % M;
    while (i < N)
    {
        j = 0;
        while (j < M)
        {
            if ((i == snake_point.x) && (j == snake_point.y)) {
                field[i][j] = SNAKEVAL;
            } else if ((i == food_x) && (j == food_y)) {
                field[i][j] = FOODVAL;
            } else {
                field[i][j] = BLANKVAL;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    
    struct Point food_point;
    food_point.x = food_x;
    food_point.y = food_y;
    return food_point;
}

void main_loop(int field[N][M], struct Point food_point_arg)
{
    struct Point food_point = food_point_arg;

    char ch;
    int ishit = 0;

    display_field(field);
    
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
                // now it just updates food location (which is useless)
                food_point = place_food(field, food_point);
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
    
    struct Point snake_initial_point;
    snake_initial_point.x = N/2;
    snake_initial_point.y = M/2;
    
    struct Point food_point = initialize_snake(field, snake_initial_point);
    
    main_loop(field, food_point);
    
    return 0;
}
