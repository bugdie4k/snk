#include <math.h>
#include <time.h> 
#include "nonblock.h"

#define N 10
#define M 20

/* #define BLANKVAL 0 // blank value */
/* #define SNAKEVAL 1 // snake value */
/* #define FOODVAL 2  // food value */

/* #define BLANKSTR "_" // blank string */
/* #define SNAKESTR "*" // snake string */
/* #define FOODSTR "."  // food string */

#define BLANK '_'
#define SNAKE '*'
#define FOOD  '.'

struct Point {
    int x;
    int y;
};

enum Direction {UP, DOWN, RIGHT, LEFT};

struct Snake {
    struct Point cells[N * M];
    int len;
    enum Direction dir;
};

void display_field(char field[N][M])
{
    int i = 0;
    int j;
    // printf("\n");
    system("clear");
    while (i < N)
    {
        j = 0;
        while (j < M)
        {
            char cell = field[i][j];
            if (cell == BLANK) {
                printf("%c", BLANK);
            } else if (cell == SNAKE) {
                printf("%c", SNAKE);
            } else if (cell == FOOD) {
                printf("%c", FOOD);
            }
                
            j = j + 1;
        }
        printf("\n");
        i = i + 1;
    }
}

struct Point place_food(char field[N][M], struct Point old_food_point)
{
    int food_x = rand() % N;
    int food_y = rand() % M;

    field[food_x][food_y] = FOOD;
    field[old_food_point.x][old_food_point.y] = BLANK;

    struct Point new_food_point;
    new_food_point.x = food_x;
    new_food_point.y = food_y;
    return new_food_point;
}

void initialize_field(char field[N][M], struct Point snake_point, struct Point food_point)
{
    int i = 0;
    int j;
    while (i < N)
    {
        j = 0;
        while (j < M)
        {
            if ((i == snake_point.x) && (j == snake_point.y)) {
                field[i][j] = SNAKE;
            } else if ((i == food_point.x) && (j == food_point.y)) {
                field[i][j] = FOOD;
            } else {
                field[i][j] = BLANK;
            }
            j = j + 1;
        }
        i = i + 1;
    }
}

void move_snake()
{
    
}

void main_loop(char field[N][M], struct Point food_point, struct Snake* snakep)
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
                // now it just updates food location (which is useless)
                ishit = 0;
                food_point = place_food(field, food_point);
            }
        }
        display_field(field);
        usleep(50000); 
    }
    nonblock(0);
}

int main()
{   
    char field[N][M];
    
    struct Point snake_initial_point;
    snake_initial_point.x = N / 2;
    snake_initial_point.y = M / 2;

    struct Point food_initial_point;
    srand(time(NULL)); // to make new random values for food each time
    food_initial_point.x = rand() % N;
    food_initial_point.y = rand() % M;
    
    initialize_field(field, snake_initial_point, food_initial_point);

    struct Snake snake;
    snake.cells[0] = snake_initial_point;
    snake.len = 1;
    snake.dir = UP;
    struct Snake* snakep = &snake;
    
    display_field(field);
    main_loop(field, food_initial_point, snakep);
    
    return 0;
}
