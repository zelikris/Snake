//Kristian Zhelyazkov

#include "mylib.h"
#include "startScreen1.h"
#include "loser.h"
#include "winner.h"
#include <stdlib.h>
#include <stdio.h>

/* The head of the snake. This is the 
   only struct the player can control */
typedef struct {
	int row;
	int col;
	int oldrow;
	int oldcol;
	// [0] == left, [1] == right, [2] == up, [3] == down
	short facing[4];
} PLAYER;

/* The body of the snake */
typedef struct {
	int row;
	int col;
	int oldrow;
	int oldcol;
	short hide;
} PART;

/* Food makes the snake grow in size */
typedef struct {
	int row;
	int col;
	int oldrow;
	int oldcol;
} FOOD;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160
#define MAXPARTS 20
#define PARTSIZE 7
#define FOODSIZE 10

PLAYER pla;
PLAYER* plaPoint = &pla;
FOOD food;
PART parts[MAXPARTS];
int partsCounter;
short score;
int LV1_WIDTH = 240;
int LV1_HEIGHT = 149;
short lvl1 = 1;
short lvl2 = 0;
short lvl3 = 0;
short start;
short gameLoop;
short lost;
short win;

int main() {
	REG_DISPCTL = MODE3 | BG2_ENABLE;

	// initializations
	initPlayer();
	char buffer[14];
	partsCounter = 1;
 	score = 20;
	lost = 0;
	win = 0;
	start = 0;
	gameLoop = 1;
	
	generateNewFood();

	// start screen
	if (lvl1) {
		drawImage3(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, startScreen1);
		while (!start) {
			if (KEY_DOWN_NOW(BUTTON_START)) {
				start = 1;
				drawRect(0, 0, 160, 240, BLACK);
			}
		}
		short lvlMsg = 1;
		char lvl1Msg[] = "Level 1";
		while (lvlMsg) {
			//delay(3);
			drawString(70, 95, lvl1Msg, WHITE);
			
			if (KEY_DOWN_NOW(BUTTON_A)) {
				lvlMsg = 0; 
				drawRect(0, 0, 160, 240, BLACK);
			}
		}
	}
	while (gameLoop) {
		delay(2);
		playerTick();
		partsTick();

		if (foodCollision() == 1) {
			generateNewFood();
			if (partsCounter < MAXPARTS) {
				partsCounter++;
					score--;
			} else {
				win = 1;
				// clear screen
				drawRect(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, BLACK);
				break;
			}	
		}

		renderAll();
		drawScore(score, buffer);

		// border
		if (lvl1) {
			drawHollowRect(0, 0, 239, 149, LTGRAY); 
		} else if (lvl2) {
			drawHollowRect(10, 0, 239, 139, LTGRAY);
		} else if (lvl3) {
			drawHollowRect(15, 15, 205, 120, LTGRAY);
		}

		// restart
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			lvl1 = 1;
			lvl2 = 0;
			lvl3 = 0;
			main();
		}
		if (checkDeath()) {
			gameLoop = 0;
			lost = 1;
			lvl1 = 1;
			lvl2 = 0;	
			lvl3 = 0;
		}
	}

	while(win) {
		char lvl2Msg[] = "Level 2";
		char lvl3Msg[] = "Level 3";

		// next level message
		if (lvl1 == 1) {
			drawString(70, 95, lvl2Msg, WHITE);	
		} else if (lvl2 == 1) {
			drawString(70, 95, lvl3Msg, WHITE);
		} else if (lvl3 == 1) {
			drawImage3(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, winner);
		}
		if (KEY_DOWN_NOW(BUTTON_A)) {
			delay(3);
			if (lvl1 == 1) {
				lvl1 = 0;
				lvl2 = 1;
				lvl3 = 0;
				drawRect(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, BLACK);
			} else if (lvl2 == 1) {
				lvl1 = 0;
				lvl2 = 0;
				lvl3 = 1;
				drawRect(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, BLACK);
			} else {
				lvl1 = 1;
				lvl2 = 0;			
				lvl3 = 0;
			}
			main();
		}
	}
	
	while(lost) {
		drawImage3(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, loser);
		if (KEY_DOWN_NOW(BUTTON_START)) {
			delay(3);
			main();
		}
	}
	
	return 0;
}

/* Generates position for each part of the snake. The snake
   starts with a fixed number of parts, which are all hidden
   and are revealed when the player collides with a piece of food
   one at a time */
void partsTick() {
	// the head of the snake is at parts[0]
	parts[0].row = pla.row;
	parts[0].col = pla.col;
	parts[0].oldrow = pla.oldrow;
	parts[0].oldcol = pla.oldcol;
	int i;
	for (i = 1; i < MAXPARTS; i++) {
		parts[i].row = parts[i-1].oldrow;
		parts[i].col = parts[i-1].oldcol;
		parts[i].oldrow = parts[i].oldrow;
		parts[i].oldcol = parts[i].oldcol; 
		parts[i].hide = 1;
	}
	// reveal current parts
	for (i = 1; i < partsCounter; i++) {
		parts[i].hide = 0;
	}
}

/* Checks if snake has collided with food */
int foodCollision() {
	int collision = 0;
	
	// check corner pixels in respect to the direction the snake is facing
	if ((pla.facing[0] == 1 && calcOffset(pla.row, pla.col) == MAGENTA) ||
	    (pla.facing[1] == 1 && calcOffset(pla.row, pla.col + PARTSIZE) == MAGENTA) || 
	     (pla.facing[2] == 1 && calcOffset(pla.row, pla.col) == MAGENTA) ||
              (pla.facing[3] == 1 && calcOffset(pla.row + PARTSIZE, pla.col) == MAGENTA)) {
		collision = 1;
	} 
	if ((pla.facing[0] == 1 && calcOffset(pla.row + PARTSIZE, pla.col) == MAGENTA) ||
	     (pla.facing[1] == 1 && calcOffset(pla.row + PARTSIZE, pla.col + PARTSIZE) == MAGENTA) ||
	       (pla.facing[2] == 1 && calcOffset(pla.row, pla.col + PARTSIZE) == MAGENTA) ||
                (pla.facing[3] == 1 && calcOffset(pla.row + PARTSIZE, pla.col + PARTSIZE) == MAGENTA)) {
		collision = 1;
	}
	return collision;
}

/* Calculates the videoBuffer address given a row and a column */
u16 calcOffset(int r, int c) {
	return videoBuffer[OFFSET(r,c, SCREEN_WIDTH)];
}


/* Generates a random location for the food */
void generateNewFood() {

	if (lvl1 == 1) {
		do {
			food.row = rand() % (LV1_HEIGHT - FOODSIZE);
			food.col = rand() % (LV1_WIDTH - FOODSIZE);
		} while ((food.row == pla.row && food.col == pla.col) || 
		            calcOffset(food.row, food.col) == GOLD);	
	} else if (lvl2 == 1) {
		do {
			food.row = rand() % (139 - FOODSIZE);
			food.col = rand() % (239 - FOODSIZE);
		} while ((food.row == pla.row && food.col == pla.col) || 
		            calcOffset(food.row, food.col) == GOLD || food.row < 10);
	} else if (lvl3 == 1) {
		do {
			food.row = rand() % (120 - FOODSIZE);
			food.col = rand() % (205 - FOODSIZE);
		} while ((food.row == pla.row && food.col == pla.col) || 
		            calcOffset(food.row, food.col) == GOLD || food.row < 15 || food.col < 15);
	}	
}

/* Initializes the player's starting position, 
   and facing direction  */
void initPlayer() {
	pla.row = 70;
	pla.oldrow = 70;
	pla.col = 100;
	pla.oldcol = 100;
	// reset facing
	for (int i = 0; i < 3; i++) {
		pla.facing[i] = 0;
	}
	pla.facing[2] = 1;
}

/* Calculates the next position of the player */
void playerTick() {
	short speed = 8;
	if(KEY_DOWN_NOW(BUTTON_UP) && pla.facing[3] == 0) {
	 		
		//sets the facing direction of the player
		int i;
		for (i = 0; i <= 3; i++) {
			pla.facing[i] = 0;
		}
		pla.facing[2] = 1;
	}

	if(KEY_DOWN_NOW(BUTTON_DOWN) && pla.facing[2] == 0) {
		

		//sets the facing direction of the player
		int i;
		for (i = 0; i <= 3; i++) {
			pla.facing[i] = 0;
		}
		pla.facing[3] = 1;
	}

	if(KEY_DOWN_NOW(BUTTON_LEFT) && pla.facing[1] == 0) {
		

		//sets the facing direction of the player
		int i;
		for (i = 0; i <= 3; i++) {
			pla.facing[i] = 0;
		}
		pla.facing[0] = 1;
	}

	if(KEY_DOWN_NOW(BUTTON_RIGHT) && pla.facing[0] == 0) {
		
		//sets the facing direction of the player
		int i;
		for (i = 0; i <= 3; i++) {
			pla.facing[i] = 0;
		}
		pla.facing[1] = 1;
	}
	// continuous movement
	if (pla.facing[0] == 1) { // left
		pla.col -= speed;
	}
	if (pla.facing[1] == 1) { // right
		pla.col += speed;
	}
	if (pla.facing[2] == 1) { // up
		pla.row -= speed;
	}
	if (pla.facing[3] == 1) { // down
		pla.row += speed;
	}	
}

/* Checks if the player moves out of the screen or
   collides with itself */
int checkDeath() {
	// check bounds
	int dead = 0;
	
	if (lvl1 == 1) {
		dead = lvl1BoundsCheck();
	} else if (lvl2 == 1) {
		dead = lvl2BoundsCheck();
	} else if (lvl3 == 1) {
		dead = lvl3BoundsCheck();
	}	

	//checks self-collision
	if ((pla.facing[0] == 1 && calcOffset(pla.row + 2, pla.col - 2) == GOLD) ||
	     (pla.facing[1] == 1 && calcOffset(pla.row + 1, pla.col + PARTSIZE + 1) == GOLD) ||
	      (pla.facing[2] == 1 && calcOffset(pla.row - 2, pla.col + (PARTSIZE / 2)) == GOLD) ||
               (pla.facing[3] == 1 && calcOffset(pla.row + PARTSIZE + 1, pla.col + (PARTSIZE / 2)) == GOLD)) {
		dead = 1;
	}
	return dead;
}

/* Checks if the player exceeds the bounds of level 1*/
int lvl1BoundsCheck() {
	int dead = 0;	
	if (pla.row < 0) {
		dead = 1;
	}
	if (pla.row > 149 - PARTSIZE) {
		dead = 1;
	}
	if (pla.col < 0) {
		dead = 1;
	}
	if (pla.col > SCREEN_WIDTH - PARTSIZE) {
		dead = 1;
	}
	return dead;
}

/* Checks if the player exceeds the bounds of level 2 */
int lvl2BoundsCheck() {
	int dead = 0;	
	if (pla.row < 10 - 3) {
		dead = 1;
	}
	if (pla.row > 139) { 
		dead = 1;
	}
	if (pla.col < 0) {
		dead = 1;
	}
	if (pla.col > SCREEN_WIDTH - PARTSIZE) {
		dead = 1;
	}
	return dead;
}

/* Checks if the player exceeds the bounds of level 3 */
int lvl3BoundsCheck() {
	int dead = 0;	
	if (pla.row < 15 - PARTSIZE) {
		dead = 1;
	}
	if (pla.row > 130) { 
		dead = 1;
	}
	if (pla.col < 15) {
		dead = 1;
	}
	if (pla.col > 205) {
		dead = 1;
	}
	return dead;
}

/* Draws the snake on the screen */
void renderPlayer() {
	// draw the head of the snake
	drawRect(pla.oldrow, pla.oldcol, PARTSIZE, PARTSIZE, BLACK);
	drawRect(pla.row, pla.col, PARTSIZE, PARTSIZE, GOLD);
	pla.oldrow = pla.row;
	pla.oldcol = pla.col;

	// draw the body of the snake
	parts[0].row = pla.row;
	parts[0].col = pla.col;
	parts[0].oldrow = pla.oldrow;
	parts[0].oldcol = pla.oldcol;
	int i;
	PART* cur;
	for (i = 1; i < MAXPARTS; i++) {
		cur = parts + i;
		drawRect(cur->oldrow, cur->oldcol, PARTSIZE, PARTSIZE, BLACK);
	}

	for (i = 1; i < MAXPARTS; i++) {
		cur = parts + i;
		if (cur->hide == 0) {
			drawRect(cur->row, cur->col, PARTSIZE, PARTSIZE, GOLD);
		}
		cur->oldrow = cur->row;
		cur->oldcol = cur->col;
	}
}

/* Draws the food on the screen */
void renderFood() {
	drawRect(food.oldrow, food.oldcol, FOODSIZE, FOODSIZE, BLACK);
	drawRect(food.row, food.col, FOODSIZE, FOODSIZE, MAGENTA);
	food.oldrow = food.row;
	food.oldcol = food.col;
}

/* Draws the remaining food on the screen */
void drawScore(short score, char* buffer) {
	drawRect(151, 1, 10, 54, BLACK); // remaining
	drawString(151, 1, "REMAINING: ", WHITE);
	sprintf(buffer, "%d", score);

	drawRect(151, 63, 10, 12, BLACK); // food
	drawString(151, 63, buffer, WHITE);
}

/* Draws the player and the food after vertical blank */
void renderAll() {
	waitForVblank();
	renderPlayer();
	renderFood();
}

/* Delay for a given amount of time */
void delay(int n) {
	int i;
	volatile int x;
	for(i=0; i<n*10000; i++)
	{
		x++;
	}
}





