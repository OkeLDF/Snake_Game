#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define LINES 8
#define COLS 16

#define APPLE 64
#define SNAKE 254
#define BLANK 250

#define RED "31"
#define GREEN "32"

void error(char str[]){
	fprintf(stderr, "\033[31mError: %s.\033[m", str);
	exit(1);
}

#if __has_include(<windows.h>)
	#include <windows.h>

	DWORD ThreadId[2];
	HANDLE ThreadHandle[2];

	void createThreads(DWORD WINAPI (*gameFunction)(LPVOID), DWORD WINAPI (*getkeyFunction)(LPVOID)){
		ThreadHandle[0] = CreateThread(NULL, 0, gameFunction, NULL, 0, &(ThreadId[0]));
		ThreadHandle[1] = CreateThread(NULL, 0, getkeyFunction, NULL, 0, &(ThreadId[1]));
		
		if(!ThreadHandle[0]) error("Can't create thread 0 (game)");
		if(!ThreadHandle[1]) error("Can't create thread 1 (getkey)");
	}

	void executeThreads(){
		WaitForSingleObject(ThreadHandle[0], INFINITE);
		WaitForSingleObject(ThreadHandle[1], INFINITE);
		
		CloseHandle(ThreadHandle[0]);
		CloseHandle(ThreadHandle[1]);
	}

#else
	#include <pthread.h>

	pthread_t threads[2];
	void* results;

	void createThreads(void* (*game_func)(void*), void* (*getkey_func)(void*)){
		if(pthread_create(&threads[0], NULL, game_func, NULL) == -1) error("Can't create thread 0 (game)");
		if(pthread_create(&threads[1], NULL, getkey_func, NULL) == -1) error("Can't create thread 1 (getkey)");
	}

	void executeThreads(){
		if(pthread_join(threads[0], &results) == -1) error("Can't join thread 0 (game)");
		if(pthread_join(threads[1], &results) == -1) error("Can't join thread 1 (getkey)");
	}
#endif

int running = 1;
int addto_x = 0, addto_y = 1;

/*
typedef struct snake{
	int x;
	int y;
	struct snake* next;
} Snake;

Snake* new_Snake(int x, int y){
	Snake* snake;
	snake = (Snake*) malloc(sizeof(snake));
	snake->x = x;
	snake->y = y;
	snake->next = NULL;
	return snake;
}

void release(Snake* snake_head){
	if(snake_head->next) release(snake_head->next);
	free(snake_head);
}
*/

#if __has_include(<windows.h>)
DWORD WINAPI getkey(LPVOID Param)
#else
void* getkey(void* param)
#endif
{
	char key;
	while(running==1){
		key = getch();
		switch(key){
			case 'w':
			case 'W':
			case 'H':
				if(addto_x) break;
				addto_x = -1;
				addto_y = 0;
				break;
			
			case 'a':
			case 'A':
			case 'K':
				if(addto_y) break;
				addto_x = 0;
				addto_y = -1;
				break;
			
			case 'd':
			case 'D': 
			case 'M':
				if(addto_y) break;
				addto_x = 0;
				addto_y = 1;
				break;
			
			case 's':
			case 'S':
			case 'P':
				if(addto_x) break;
				addto_x = 1;
				addto_y = 0;
				break;
				
			case 13:
				running=0;
				break;
			
			default: break;
		}
	}
	return 0;
}

#if __has_include(<windows.h>)
DWORD WINAPI game(LPVOID Param)
#else
void* game(void* param)
#endif
{
	char grid[LINES][COLS];
	
	int apple[2] = {2, 3};
	
	int snake[LINES*COLS][2];
	int snake_length = 1;
	memset(snake, 0, sizeof(snake));

	int apple_count=0;
	int x_temp[2] = {0, 0};
	int y_temp[2] = {0, 0};
	
	int i=0, j, k;
	
	srand(time(NULL));
	
	while(running==1){
		Sleep(350);
		system("cls");
		
		for(i=0;i<LINES;i++) memset(grid[i], BLANK, COLS);
		
		if(snake[0][0] == apple[0] && snake[0][1] == apple[1]){
			// if(apple_count == 15) running = 0;
			apple[0] = rand()%LINES;
			apple[1] = rand()%COLS;
			
			for(i=0; i<snake_length ; i++){
				if(snake[i][0] == apple[0] && snake[i][1] == apple[1]){
					apple[0] = rand()%LINES;
					apple[1] = rand()%COLS;
				}
			}
			snake_length++;
			apple_count++;
		}
		
		x_temp[0] = snake[0][0];
		y_temp[0] = snake[0][1];
		snake[0][0] = (snake[0][0] + addto_x) % LINES;
		snake[0][1] = (snake[0][1] + addto_y) % COLS;
		if(snake[0][0] < 0) snake[0][0] = LINES-1;
		if(snake[0][1] < 0) snake[0][1] = COLS-1;
		
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
		
		if(apple_count < 2){
			printf("\033[90mUse the arrow keys or WASD\nto move the snake\n\n");
			printf("\033[90mUse ENTER to end the game\n\n");
		}
	}
	
	if(apple_count==(LINES*COLS)-1) printf("\033[33mCongratulations!\n\n\033[m\n");
	else printf("\033[33mGame Over!\033[m\n");
	
	return 0;
}

int main(){
	createThreads(game, getkey);
	executeThreads();
	return 0;
}