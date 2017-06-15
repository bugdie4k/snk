#include <math.h>
#include <time.h>
#include "nonblock.h"

#define N 10
#define M 20

#define BLANK '_'
#define SNAKE '*'
#define FOOD  '.'

#define UPCH    'w'
#define DOWNCH  's'
#define RIGHTCH 'd'
#define LEFTCH  'a'

#define QUITCH  'q'

#define SCORE_LEN_RATIO 2

#ifdef DEBUG

#include <limits.h>

FILE *logfile;

void log_header()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(logfile, "---------------------\n");
    fprintf(logfile, "SNK IS RUN BY:      %s\n", getenv("LOGNAME"));
    fprintf(logfile, "STARTED LOGGING AT: %02d.%02d.%04d %02d:%02d\n", t->tm_mday, t->tm_mon+1, t->tm_year + 1900, t->tm_hour,t->tm_min);
    fprintf(logfile, "---------------------\n\n");
    fflush(logfile);
}
#endif

typedef struct {
    int x;
    int y;
} Point;

typedef enum {UP, DOWN, RIGHT, LEFT} Direction;

typedef struct {
    Point cells[N * M];
    int len;
    Direction dir;
    int score;
} Snake;

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

int validate_food_point(Point food_point, Snake snake)
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

Point place_food(char field[N][M], Snake snake)
{
    Point new_food_point;
    do
    {
        new_food_point.x = rand() % N;;
        new_food_point.y = rand() % M;
    }
    while(!validate_food_point(new_food_point, snake));
    
    field[new_food_point.x][new_food_point.y] = FOOD;

    return new_food_point;
}

void update_field(char field[N][M], Snake snake, Point food_point)
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

void game_over(int score)
{
    system("clear");
    printf(" *** GAME OVER ***\n\n");
    printf("YOUR SCORE IS\n");
    printf("\033[31m");
    printf("%d\n\n", score);
    printf("\033[0m");
    printf("Press any key to exit\n");
    system ("/bin/stty raw");
    getchar();
    system ("/bin/stty cooked");
    system("clear");
    exit(0);
}

void adjust_snake(Snake* snakep, int x, int y)
{
    Point prev = snakep->cells[0];
    snakep->cells[0].x += x;
    snakep->cells[0].y += y;
    
    Point tmp;
    int i;
    for (i = 1; i < snakep->len; i++)
    {
        tmp = snakep->cells[i];
        snakep->cells[i] = prev;
        prev = tmp;
    }
}

// push new point from head
void grow_snake(Snake* snakep, Point food_point)
{
    int i;
    for (i = snakep->len; i > 0; i--)
    {
        snakep->cells[i] = snakep->cells[i - 1];
    }
    snakep->cells[0] = food_point;
    snakep->len++;
}

int points_eq(Point p1, Point p2)
{
    if ((p1.x == p2.x) && (p1.y == p2.y))
    {
        return 1;
    }
    return 0;
}

int if_point_exceeds_field(Point point)
{
    return ((point.x >= N) || (point.x < 0) ||
            (point.y >= M) || (point.y < 0));
}

int if_snake_hit_itself(Snake snake, Point moved_head)
{
    int i;
    for (i = 3; i < snake.len - 1; i++)
    {
        if (points_eq(snake.cells[i], moved_head))
        {
            return 1;
        }
    }
    return 0;
}

int move_snake(Snake* snakep, Point food_point)
{   
    int dx;
    int dy;
    
    switch (snakep->dir)
    {
    case UP:
        dx = -1;
        dy = 0;
        break;
    case DOWN:
        dx = 1;
        dy = 0;
        break;
    case RIGHT:
        dx = 0;
        dy = 1;
        break;
    case LEFT:
        dx = 0;
        dy = -1;
        break;
    }

    #ifdef DEBUG
    fprintf(logfile, "MOVE: dx = %2d, dy = %2d\n", dx, dy); fflush(logfile);
    #endif

    Point moved_head;
    moved_head.x = snakep->cells[0].x + dx;
    moved_head.y = snakep->cells[0].y + dy;

    int exceeds = if_point_exceeds_field(moved_head);
    int hit_itself = if_snake_hit_itself(*snakep, moved_head);
    if (exceeds || hit_itself)
    {
        #ifdef DEBUG
        fprintf(logfile, "  GAME OVER: exceeds = %d, hit-itself = %d, score = %d, len = %d\n", exceeds, hit_itself, snakep->score, snakep->len); fflush(logfile);
        #endif
        
        game_over(snakep->score);
    }

    if (points_eq(moved_head, food_point))
    {
        #ifdef DEBUG
        fprintf(logfile, "  FOOD EATEN\n"); fflush(logfile);
        #endif

        if (++snakep->score % SCORE_LEN_RATIO == 0)
        {
            grow_snake(snakep, food_point);   
        }
        else
        {
            adjust_snake(snakep, dx, dy);
        }
        return 1;
    }
    
    adjust_snake(snakep, dx, dy);
    return 0; 
}

void main_loop(char field[N][M], Point food_point, Snake* snakep)
{
    char ch;
    int ishit = 0; // it shows if a key was pressed and if to quit at the same time
    int food_eaten;
    
    nonblock(1);
    while(!ishit)
    {
        usleep(1);
        ishit = kbhit();
        if (ishit != 0)
        {
            ch = fgetc(stdin);
            if (ch == QUITCH)
            {
                game_over(snakep->score);
            }
            else if ((ch == UPCH) && (snakep->dir != DOWN))
            {
                snakep->dir = UP;
            }
            else if ((ch == DOWNCH) && (snakep->dir != UP))
            {
                snakep->dir = DOWN;
            }
            else if ((ch == RIGHTCH) && (snakep->dir != LEFT))
            {
                snakep->dir = RIGHT;
            }
            else if ((ch == LEFTCH) && (snakep->dir != RIGHT))
            {
                snakep->dir = LEFT;
            }
            ishit = 0;
        }
        else
        {
            food_eaten = move_snake(snakep, food_point);
            if (food_eaten)
            {
                food_point = place_food(field, *snakep);
            }
        }
        update_field(field, *snakep, food_point);
        display_field(field);
        usleep(200000); // 0.2s
    }
    nonblock(0);
}

int main()
{
    #ifdef DEBUG
    char logfile_name[PATH_MAX]; // from limits.h
    getcwd(logfile_name, sizeof(logfile_name));
    sprintf(logfile_name, "%s/snk_log.txt", logfile_name);
    logfile = fopen(logfile_name, "w");
    log_header();
    #endif
        
    char field[N][M];
    
    Point snake_initial_point;
    snake_initial_point.x = N / 2;
    snake_initial_point.y = M / 2;

    Point food_initial_point;
    srand(time(NULL)); // to make new random values for food each time
    food_initial_point.x = rand() % N;
    food_initial_point.y = rand() % M;

    Snake snake;
    snake.cells[0] = snake_initial_point;
    snake.len = 1;
    snake.dir = UP;
    snake.score = 0;
    
    update_field(field, snake, food_initial_point);

    Snake* snakep = &snake;
    
    display_field(field);
    main_loop(field, food_initial_point, snakep);

    #ifdef DEBUG
    fclose(logfile);
    #endif
    
    return 0;
}
