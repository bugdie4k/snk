// shamelessly copied from
// http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/

#include "nonblock.h"

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
    struct termios ttystate;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    if (state == 1)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state == 0)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
 
}

void _test_nonblock()
{
    char c;
    int i = 0;
 
    nonblock(1);
    while(!i)
    {
        usleep(1);
        i = kbhit();
        if (i != 0)
        {
            c = fgetc(stdin);
            if (c == 'q')
                i = 1;
            else
                i = 0;
        }
 
        printf("%c", c);
    }
    printf("\n you hit %c. \n",c);
    nonblock(0);
}
