#include "nonblock.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/************************************************/

// field size

#define HEIGHT 15
#define WIDTH  15

// margins chars

static const char* HORIZONTAL_MARGIN  = "─";
static const char* VERTICAL_MARGIN    = "│";
static const char* UPPER_LEFT_CORNER  = "┌";
static const char* UPPER_RIGHT_CORNER = "┐";
static const char* LOWER_LEFT_CORNER  = "└";
static const char* LOWER_RIGHT_CORNER = "┘";

// ingame elements chars

/// ASCII (obsolete)

static const char BLANK = '.';
static const char SNAKE = '@';
static const char SNAKE_HEAD_L = '<';
static const char SNAKE_HEAD_R = '>';
static const char SNAKE_HEAD_U = '^';
static const char SNAKE_HEAD_D = 'v';
static const char FOOD  = '*';

/// COLORFUL UNICODE

#define ESC "\x1b["
#define RESET "\x1b[0m"
#define FG(COLOR, TEXT) ESC "3" #COLOR "m" TEXT RESET
#define BG(COLOR, TEXT) ESC "4" #COLOR "m" TEXT RESET
#define FGBG(FCOLOR, BCOLOR, TEXT) ESC "3" #FCOLOR "m" ESC "4" #BCOLOR "m" TEXT RESET

static const char* BLANK2 = "..";
static const char* SNAKE2 = BG(2, "  ");
static const char* FOOD2  = BG(3, "  ");;
static const char* SNAKE_HEAD_L2 = FGBG(0, 2, "\xe2\x97\x80\xe2\x96\xa0");
static const char* SNAKE_HEAD_R2 = FGBG(0, 2, "\xe2\x96\xa0\xe2\x96\xb6");
static const char* SNAKE_HEAD_U2 = FGBG(0, 2, "\xe2\x97\xa2\xe2\x97\xa3");
static const char* SNAKE_HEAD_D2 = FGBG(0, 2, "\xe2\x97\xa5\xe2\x97\xa4");

// controls

static const char UP_CH    = 'w';
static const char DOWN_CH  = 's';
static const char RIGHT_CH = 'd';
static const char LEFT_CH  = 'a';
static const char QUIT_CH  = 'q';

// gameplay settings

static const int SCORE_LEN_RATIO = 1;
static const int MOVE_DELAY = 200000; // ms

/************************************************/

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

typedef struct
{
     int x;
     int y;
}
    Point;

typedef enum
{
    UP, DOWN, RIGHT, LEFT
}
    Direction;

typedef struct
{
     Point cells[HEIGHT * WIDTH];
     int len;
     Direction dir;
     int score;
}
    Snake;

Point* get_head(Snake* snakep)
{
    return &snakep->cells[snakep->len - 1];
}

void display_upper_margin()
{
    printf("%s", UPPER_LEFT_CORNER);
    int i;
    for (i = 0; i < WIDTH * 2; ++i) printf("%s", HORIZONTAL_MARGIN);
    printf("%s\n", UPPER_RIGHT_CORNER);
}

void display_lower_margin()
{
    printf("%s", LOWER_LEFT_CORNER);
    int i;
    for (i = 0; i < WIDTH * 2; ++i) printf("%s", HORIZONTAL_MARGIN);
    printf("%s\n", LOWER_RIGHT_CORNER);
}

void display_field(char field[HEIGHT][WIDTH], int score)
{
    int i;
    int j;
    system("clear");
    printf(FG(7, "KEYS: WASD, Q to quit\n"));
    // printf("─────\n");
    display_upper_margin();
    for (i = 0; i < HEIGHT; ++i)
    {
        printf("%s", VERTICAL_MARGIN);
        for (j = 0; j < WIDTH; ++j)
        {
            char fch = field[i][j];
            if (fch == BLANK)
            {
                printf("%s", BLANK2);
            }
            else if (fch == SNAKE)
            {
                printf("%s", SNAKE2);
            }
            else if (fch == SNAKE_HEAD_L)
            {
                printf("%s", SNAKE_HEAD_L2);
            }
            else if (fch == SNAKE_HEAD_R)
            {
                printf("%s", SNAKE_HEAD_R2);
            }
            else if (fch == SNAKE_HEAD_U)
            {
                printf("%s", SNAKE_HEAD_U2);
            }
            else if (fch == SNAKE_HEAD_D)
            {
                printf("%s", SNAKE_HEAD_D2);
            }
            else if (fch == FOOD)
            {
                printf("%s", FOOD2);
            }
            else
            {
                printf("%c%c", fch, fch);
            }

        }
        printf("%s\n", VERTICAL_MARGIN);
    }
    display_lower_margin();
    printf("SCORE: " FG(1, "%4d") "\n", score);
}

int validate_food_point(Point food_point, Snake* snakep)
{
    int i;
    for (i = 0; i < snakep->len; ++i)
    {
        if ((snakep->cells[i].x == food_point.x) && (snakep->cells[i].y == food_point.y))
        {
            return 0;
        }
    }
    return 1;
}

Point place_food(char field[HEIGHT][WIDTH], Snake* snakep)
{
    Point new_food_point;
    do
    {
        new_food_point.x = rand() % HEIGHT;;
        new_food_point.y = rand() % WIDTH;
    }
    while(!validate_food_point(new_food_point, snakep));

    field[new_food_point.x][new_food_point.y] = FOOD;

    return new_food_point;
}

void update_field(char field[HEIGHT][WIDTH], Snake* snakep, Point food_point)
{
    int i;
    int j;
    for (i = 0; i < HEIGHT; ++i)
    {
        for (j = 0; j < WIDTH; ++j)
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

    Point p = snakep->cells[snakep->len - 1];
    if (snakep->dir == LEFT)
    {
        field[p.x][p.y] = SNAKE_HEAD_L;
    }
    else if (snakep->dir == RIGHT)
    {
        field[p.x][p.y] = SNAKE_HEAD_R;
    }
    else if (snakep->dir == UP)
    {
        field[p.x][p.y] = SNAKE_HEAD_U;
    }
    else if (snakep->dir == DOWN)
    {
        field[p.x][p.y] = SNAKE_HEAD_D;
    }
    for (i = 0; i < snakep->len - 1; ++i)
    {
        p = snakep->cells[i];
        field[p.x][p.y] = SNAKE;
    }
}

void game_over(int exceeds, int hit_itself)
{
    printf("\n *** GAME OVER ***\n\n");
    if (exceeds)
    {
        printf("Snake hit the border\n\n");
    }
    else if (hit_itself)
    {
        printf("Snake hit itself\n\n");
    }
    printf("Press ENTER to exit\n");

    char c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    system("clear");
    exit(0);
}

void adjust_snake(Snake* snakep, int dx, int dy)
{
    Point prev = *get_head(snakep);
    get_head(snakep)->x += dx;
    get_head(snakep)->y += dy;

    Point tmp;
    int i;
    for (i = snakep->len - 2; i >= 0; --i)
    {
        tmp = snakep->cells[i];
        snakep->cells[i] = prev;
        prev = tmp;
    }
}

// add new point to the cells array
void grow_snake(Snake* snakep, Point food_point)
{
    ++snakep->len;
    *get_head(snakep) = food_point;
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
    return ((point.x >= HEIGHT) || (point.x < 0) ||
            (point.y >= WIDTH) || (point.y < 0));
}

int if_snake_hit_itself(Snake* snakep, Point moved_head)
{
    int i;
    for (i = 3; i < snakep->len - 1; ++i)
    {
        if (points_eq(snakep->cells[i], moved_head))
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
    moved_head.x = get_head(snakep)->x + dx;
    moved_head.y = get_head(snakep)->y + dy;

    int exceeds = if_point_exceeds_field(moved_head);
    int hit_itself = if_snake_hit_itself(snakep, moved_head);
    if (exceeds || hit_itself)
    {
#ifdef DEBUG
        fprintf(logfile, "  GAME OVER: exceeds = %d, hit-itself = %d, score = %d, len = %d\n", exceeds, hit_itself, snakep->score, snakep->len); fflush(logfile);
#endif

        game_over(exceeds, hit_itself);
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

void main_loop(char field[HEIGHT][WIDTH], Point food_point, Snake* snakep)
{
    char ch;
    int ishit = 0; // it shows if a key was pressed and if to quit at the same time

    nonblock(1);
    while(!ishit)
    {
        usleep(1);
        ishit = kbhit();
        if (ishit != 0)
        {
            ch = fgetc(stdin);
            if (ch == QUIT_CH)
            {
                display_field(field, snakep->score);
                game_over(0, 0);
            }
            else if ((ch == UP_CH) && (snakep->dir != DOWN))
            {
                snakep->dir = UP;
            }
            else if ((ch == DOWN_CH) && (snakep->dir != UP))
            {
                snakep->dir = DOWN;
            }
            else if ((ch == RIGHT_CH) && (snakep->dir != LEFT))
            {
                snakep->dir = RIGHT;
            }
            else if ((ch == LEFT_CH) && (snakep->dir != RIGHT))
            {
                snakep->dir = LEFT;
            }
            ishit = 0;
        }

        int food_eaten = move_snake(snakep, food_point);
        if (food_eaten)
        {
            food_point = place_food(field, snakep);
        }

        update_field(field, snakep, food_point);
        display_field(field, snakep->score);

        usleep(MOVE_DELAY);
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

    char field[HEIGHT][WIDTH];

    Point snake_initial_points[2];
    snake_initial_points[0].x = HEIGHT / 2;
    snake_initial_points[0].y = WIDTH / 2;
    snake_initial_points[1] = snake_initial_points[0];
    snake_initial_points[1].x += 1;

    Point food_initial_point;
    srand(time(NULL)); // to make new random values for food each time
    food_initial_point.x = rand() % HEIGHT;
    food_initial_point.y = rand() % WIDTH;

    Snake snake;
    snake.cells[0] = snake_initial_points[0];
    snake.cells[1] = snake_initial_points[1];
    snake.len = 2;
    snake.dir = UP;
    snake.score = 0;

    Snake* snakep = &snake;

    update_field(field, snakep, food_initial_point);

    display_field(field, snake.score);
    main_loop(field, food_initial_point, snakep);

#ifdef DEBUG
    fclose(logfile);
#endif

    return 0;
}
