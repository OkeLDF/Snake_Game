#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LINES 10
#define COLS 20

#define APPLE 64
#define SNAKE 254
#define BLANK 250

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

int main(){
	char grid[LINES][COLS];
	
	int apple[2] = {2, 3};
	
	Snake* head = new_Snake(0, 0);
	Snake* tail = head;
	Snake* curr;
	Snake* next;
	int running=1, apple_count=0;
	int x[2] = {0, 0};
	int y[2] = {0, 0};
	int move_x = 1, move_y = 0;
	
	int i=0, j, k;
	
	srand(time(NULL));
	
	while(running==1){
		//sleep(1);
		system("cls");
		
		for(i=0;i<LINES;i++) memset(grid[i], BLANK, COLS);
		
		if(head->x == apple[1] && head->y == apple[0]){
			apple[0] = rand()%LINES;
			apple[1] = rand()%COLS;
			
			tail->next = new_Snake(0,0);
			tail = tail->next;
			
			apple_count++;
		}
		
		x[0] = head->x;
		y[0] = head->y;
		head->x = (head->x + move_x) % COLS;
		head->y = (head->y + move_y) % LINES;
		if(head->x < 0) head->x = COLS-1;
		if(head->y < 0) head->y = LINES-1;
		
		for(curr=head->next; curr!=NULL; curr=curr->next){
			x[1] = curr->x;
			y[1] = curr->y;
			curr->x = x[0];
			curr->y = y[0];
			x[0] = x[1];
			y[0] = y[1];
		}
		
		for(curr=head->next; curr!=NULL; curr=curr->next){
			if(head->x == curr->x && head->y == curr->y){
				running = 0;
				continue;
			}
		}
		
		printf("\033[32m%c\033[m snake: (%d, %d)\n", SNAKE, head->x, head->y);
		printf("\033[31m%c\033[m apples: %d\n\n", APPLE, apple_count);
		
		grid[apple[0]][apple[1]] = APPLE;
		for(curr=head; curr!=NULL; curr=curr->next){
			grid[curr->y][curr->x] = SNAKE;
		}
		
		for(i=0; i<LINES; i++){
			for(j=0; j<COLS; j++){
				printf("\033[%sm", (grid[i][j]==APPLE)?"31":"32");
				putc(grid[i][j], stdout);
				printf("\033[m");
			}
			puts("");
		}
		puts("");
		
		char key = getch(); // AAAAA I need threads
		switch(key){
			case 'w':
			case 'W': 
				move_x = 0;
				move_y = -1;
				break;
			
			case 'a':
			case 'A': 
				move_x = -1;
				move_y = 0;
				break;
			
			case 'd':
			case 'D': 
				move_x = 1;
				move_y = 0;
				break;
			
			case 's':
			case 'S': 
				move_x = 0;
				move_y = 1;
				break;
				
			case 13:
				running=0;
				break;
			
			default: break;
		}
	}
	
	printf("\033[33mGame Over!\033[m\n");
	
	release(head);
	return 0;
}
