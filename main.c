#include <math.h>
#include <time.h> 
#include "nonblock.h"

#define N 10
#define M 20

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
    int i;
    int j;
    system("clear");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            printf("%c", field[i][j]);            
        }
        printf("\n");
    }
}

int validate_food_point(struct Point food_point, struct Snake snake)
{
    int i;
    for (i = 0; i < snake.len; i++)
    {
        if ((snake.cells[i].x == food_point.x) && (snake.cells[i].y == food_point.y))
        {
            return 0;
        }
    }
    return 1;
}

struct Point place_food(char field[N][M], struct Snake snake)
{
    struct Point new_food_point;
    do
    {
        new_food_point.x = rand() % N;;
        new_food_point.y = rand() % M;
    }
    while(!validate_food_point(new_food_point, snake));
    
    field[new_food_point.x][new_food_point.y] = FOOD;

    return new_food_point;
}

void update_field(char field[N][M], struct Snake snake, struct Point food_point)
{
    int i;
    int j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            if ((i == food_point.x) && (j == food_point.y))
            {
                field[i][j] = FOOD;
            }
            else
            {
                field[i][j] = BLANK;
            }
        }
    }

    for (i = 0; i < snake.len; i++)
    {
        field[snake.cells[i].x][snake.cells[i].y]= SNAKE;
    }
}

void game_over()
{
    system("clear");
    printf("GAME OVER.\nPress any key.");
    system ("/bin/stty raw");
    getchar();
    system ("/bin/stty cooked");
    system("clear");
    exit(0);
}

void adjust_snake(struct Snake* snakep, int x, int y)
{
    int i;
    for (i = 0; i < snakep->len; i++)
    {
        snakep->cells[i].x += x;
        snakep->cells[i].y += y;
        if ((snakep->cells[i].x >= N) || (snakep->cells[i].x < 0) ||
            (snakep->cells[i].y >= M) || (snakep->cells[i].y < 0))
        {
            game_over();
        }
    }
    
}

int move_snake(struct Snake* snakep)
{
    if (snakep->dir == UP)
    {
        adjust_snake(snakep, -1, 0);
    }
    else if (snakep->dir == DOWN)
    {
        adjust_snake(snakep, 1, 0);
    }
    else if (snakep->dir == RIGHT)
    {
        adjust_snake(snakep, 0, 1);
    }
    else if (snakep->dir == LEFT)
    {
        adjust_snake(snakep, 0, -1);
    }
    return 0; 
}

void main_loop(char field[N][M], struct Point food_point, struct Snake* snakep)
{
    char ch;
    int ishit = 0;
    int food_eaten;
    
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
            }
            else
            {
                ishit = 0;
            }
        }
        else
        {
            food_eaten = move_snake(snakep);
            if (food_eaten)
            {
                food_point = place_food(field, *snakep);
            }
        }
        update_field(field, *snakep, food_point);
        display_field(field);
        usleep(100000); // 0.1s
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

    struct Snake snake;
    snake.cells[0] = snake_initial_point;
    snake.len = 1;
    snake.dir = UP;
    
    update_field(field, snake, food_initial_point);

    struct Snake* snakep = &snake;
    
    display_field(field);
    main_loop(field, food_initial_point, snakep);
    
    return 0;
}
