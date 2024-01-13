#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

#define LINES 8
#define COLS 16

#define APPLE 64
#define SNAKE 254
#define BLANK 250

#define RED "31"
#define GREEN "32"

int running=1;
int addto_x = 0, addto_y = 1;

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

void error(char str[]){
	fprintf(stderr, "\033[31mError: %s.\033[m", str);
	exit(1);
}

DWORD WINAPI getkey(LPVOID Param){
	char key;
	while(running){
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

DWORD WINAPI game(LPVOID Param){
	char grid[LINES][COLS];
	
	int apple[2] = {2, 3};
	
	Snake* head = new_Snake(0, 0);
	Snake* tail = head;
	Snake* curr;
	Snake* next;
	int apple_count=0;
	int x_temp[2] = {0, 0};
	int y_temp[2] = {0, 0};
	
	int i=0, j, k;
	
	srand(time(NULL));
	
	while(running){
		Sleep(350);
		system("cls");
		
		for(i=0;i<LINES;i++) memset(grid[i], BLANK, COLS);
		
		if(head->x == apple[0] && head->y == apple[1]){
			if(apple_count == 15) running = 0;
			apple[0] = rand()%LINES;
			apple[1] = rand()%COLS;
			
			for(curr=head; curr!=NULL; curr=curr->next){
				if(curr->x == apple[0] && curr->y == apple[1]){
					apple[0] = rand()%LINES;
					apple[1] = rand()%COLS;
				}
			}
			
			tail->next = new_Snake(0,0);
			tail = tail->next;
			
			apple_count++;
		}
		
		x_temp[0] = head->x;
		y_temp[0] = head->y;
		head->x = (head->x + addto_x) % LINES;
		head->y = (head->y + addto_y) % COLS;
		if(head->x < 0) head->x = LINES-1;
		if(head->y < 0) head->y = COLS-1;
		
		for(curr=head->next; curr!=NULL; curr=curr->next){
			x_temp[1] = curr->x;
			y_temp[1] = curr->y;
			curr->x = x_temp[0];
			curr->y = y_temp[0];
			x_temp[0] = x_temp[1];
			y_temp[0] = y_temp[1];
		}
		
		for(curr=head->next; curr!=NULL; curr=curr->next){
			if(head->x == curr->x && head->y == curr->y){
				running = 0;
				continue;
			}
		}
		
		printf("\033[92m~~ SNAKE GAME ~~\n\n");
		printf("\033[32m%c\033[m snake: (%d, %d)\n", SNAKE, head->x, head->y);
		printf("\033[31m%c\033[m apples: %d\n\n", APPLE, apple_count);
		
		grid[apple[0]][apple[1]] = APPLE;
		for(curr=head; curr!=NULL; curr=curr->next){
			grid[curr->x][curr->y] = SNAKE;
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
	
	if(apple_count==16) printf("\033[33mCongratulations!\n\n\033[m\n");
	else printf("\033[33mGame Over!\033[m\n");
	
	release(head);
	return 0;
}

int main(){
	DWORD ThreadId[2];
	HANDLE ThreadHandle[2];
	
	ThreadHandle[0] = CreateThread(NULL, 0, game, NULL, 0, &(ThreadId[0]));
	ThreadHandle[1] = CreateThread(NULL, 0, getkey, NULL, 0, &(ThreadId[1]));
	
	if(!ThreadHandle[0]) error("Can't create thread 0 (game)");
	if(!ThreadHandle[1]) error("Can't create thread 1 (getkey)");
	
	WaitForSingleObject(ThreadHandle[0], INFINITE);
	WaitForSingleObject(ThreadHandle[1], INFINITE);
	
	CloseHandle(ThreadHandle[0]);
	CloseHandle(ThreadHandle[1]);
	
	return 0;
}