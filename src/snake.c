#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#ifdef _WIN32
	#include <conio.h>
	#define SNAKE 254
	#define BLANK 250
#else
	#define SNAKE '#'
	#define BLANK '-'
	#include <termios.h>
	char getch(){
		char c;
		struct termios oldattr, newattr;

		tcgetattr(STDIN_FILENO, &oldattr);
		
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON);
		newattr.c_lflag &= ~(ECHO);

		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
		c = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

		return c;
	}
#endif

#define APPLE 64

#define goto(x, y) printf("\033[%d;%dH",(x),(y));
#define clrscr() printf("\033[H\033[J");
#define hidecursor() printf("\e[?25l");
#define showcursor() printf("\e[?25h");

#define LINES 8
#define COLS 16

#define RED "31"
#define GREEN "32"

void error(char str[]){
	fprintf(stderr, "\033[31mError: %s.\033[m", str);
	exit(1);
}

int msleep(long msec){
	// https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
	struct timespec ts;
    int res;

    if (msec < 0){
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do{
        res = nanosleep(&ts, &ts);
    } while(res && errno == EINTR);

    return res;
}

int running = 1;
int addto_x = 0, addto_y = 1;

void go_up(){
	if(addto_x) return;
	addto_x = -1;
	addto_y = 0;
}

void go_down(){
	if(addto_x) return;
	addto_x = 1;
	addto_y = 0;
}

void go_right(){
	if(addto_y) return;
	addto_x = 0;
	addto_y = 1;	
}

void go_left(){
	if(addto_y) return;
	addto_x = 0;
	addto_y = -1;
}

void* getkey(void* param){
	char key;
	while(running==1){
		key = getch();
		switch(key){
			case 'w':
			case 'W':
			case 'H': go_up(); break;
			
			case 'a':
			case 'A':
			case 'K': go_left(); break;
			
			case 'd':
			case 'D': 
			case 'M': go_right(); break;
			
			case 's':
			case 'S':
			case 'P': go_down(); break;
				
			case 13: running=0; break;
			
			default: break;
		}
	}
	return 0;
}

void* game(void* param){
	char grid[LINES][COLS];
	
	int apple[2] = {2, 3};
	
	int snake[LINES*COLS][2];
	int snake_length = 1;
	memset(snake, 0, sizeof(snake));

	int apple_count=0;
	int x_temp[2] = {0, 0};
	int y_temp[2] = {0, 0};
	
	int i=0, j, k;
	
	hidecursor();
	clrscr();
	srand(time(NULL));
	
	while(running==1){
		msleep(200);
		goto(0, 0);
		
		for(i=0;i<LINES;i++) memset(grid[i], BLANK, COLS);
		
		if(snake[0][0] == apple[0] && snake[0][1] == apple[1]){
			apple[0] = rand()%LINES;
			apple[1] = rand()%COLS;
			
			for(i=0; i<snake_length ; i++){
				if(snake[i][0] == apple[0] && snake[i][1] == apple[1]){
					apple[0] = rand()%LINES;
					apple[1] = rand()%COLS;
					i = -1;
				}
			}
			snake_length++;
			apple_count++;
		}
		
		x_temp[0] = snake[0][0];
		y_temp[0] = snake[0][1];
		snake[0][0] += addto_x;
		snake[0][1] += addto_y;
		
		if(snake[0][0] < 0 || snake[0][0] == LINES ||
		   snake[0][1] < 0 || snake[0][1] == COLS){
			snake[0][0] -= addto_x;
			snake[0][1] -= addto_y;
			clrscr();
			running = 0;
		}
		
		for(i=1; i<snake_length; i++){
			x_temp[1] = snake[i][0];
			y_temp[1] = snake[i][1];
			snake[i][0] = x_temp[0];
			snake[i][1] = y_temp[0];
			x_temp[0] = x_temp[1];
			y_temp[0] = y_temp[1];
		}
		
		for(i=1; i<snake_length; i++){
			if(snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1]){
				clrscr();
				running = 0;
				continue;
			}
		}
		
		printf("\033[92m~~ SNAKE GAME ~~\n\n");
		printf("\033[32m%c\033[m snake: (%d, %d)\n", SNAKE, snake[0][0], snake[0][1]);
		printf("\033[31m%c\033[m apples: %d\n\n", APPLE, apple_count);
		
		grid[apple[0]][apple[1]] = APPLE;
		for(i=0; i<snake_length; i++){
			grid[snake[i][0]][snake[i][1]] = SNAKE;
		}
		
		for(i=0; i<LINES; i++){
			for(j=0; j<COLS; j++){
				printf("\033[%sm", (grid[i][j]==APPLE)? RED : GREEN);
				putc(grid[i][j], stdout);
				printf("\033[m");
			}
			puts("");
		}
		puts("");
	}
	
	if(apple_count>=(LINES*COLS)-1) printf("\033[33mCongratulations!\n\n\033[m\n");
	else printf("\033[33mGame Over!\033[m\n");
	
	showcursor();
	return 0;
}

int main(){
	pthread_t threads[2];
	void* results;

	if(pthread_create(&threads[0], NULL, game, NULL) == -1) error("Can't create thread 0 (game)");
	if(pthread_create(&threads[1], NULL, getkey, NULL) == -1) error("Can't create thread 1 (getkey)");

	if(pthread_join(threads[0], &results) == -1) error("Can't join thread 0 (game)");
	if(pthread_join(threads[1], &results) == -1) error("Can't join thread 1 (getkey)");
	return 0;
}