#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "newbackg.h"
#include "bird2.h"
#include "pb1.h"
#include "pb2.h"
#include "pb3.h"
#include "sign.h"
#include "surf.h"
#include "lazy.h"
#include "tumblr.h"
#include "startscreen.h"
#include "end1.h"
#include "end2.h"
#include "end3.h"
#include "end4.h"
#include "yousuck.h"
#include "birdleft.h"

//the videoBuffer is where all the screen pixels go in memory
u16* videoBuffer = (u16*)0x6000000;

//struct for Plastic bags, which have a unique image, starting row and col
typedef struct {
	const u16* image;
	int row;
	int col;
} PLASTICBAG;

//so I can use bool and not int --> true and false defined as 1 and 0 in makefile
typedef int bool;

//the score!!!!
int score;
int seed = 0;

int main() {
	//sets it to mode 3, the bitmap mode
	REG_DISPCTL = MODE3 | BG2_ENABLE;	
	
	//lets gooOooOoo
	startGame();
}

//it begins
void startGame() {
	//this is an array of introductory littering images. Names are weird but related to content
	const u16* startpics[] = {sign, surf, lazy, tumblr};
	
	//this loop goes through and displays every image
	for (int i = 0; i < 4; i++) {
		const u16* c = startpics[i];
		drawImage(0,0,240,160,c);
		//this delays the display of the next image so user has time to take it in and #emote
		int start = 0;
		for (int j = 0; j < 70*4000; j++) {
			start++;
		}
	}
	//startscreen is the main starting image that says save our water, press start to play
	drawImage(0,0,240,160,startscreen);
	
	//have to keep listening for the button start to be pressed. that's why it's while(1)
	while(1) {
		if (KEY_DOWN_NOW(BUTTON_START)) {
			play();
		}
	}
}

//lets goooOOooOoO for real this time
void play() {
	//new background is the playing background - in this case, the ocean
	drawImage(0,0,240,150,newbackg);
	//array of images of plastic bags
	const u16* images[] = {pb1, pb2, pb3};
	//score board rectangle
	drawRectangle(145, 0, 240, 15, BLACK);
	//initial position of the bird
	int row = 50;
	int col = 80;
	//old row and old col to get previous positions blacked out
	int oldrow = row;
	int oldcol = col;
	
	//draws bird initial position
	//birdwidth is 19, birdheight is 15
	drawImage(row,col,19,15,bird2);
	
	//water asks: has a bag hit the water yet? 	
	bool water = 0;
	//left asks: is the bird moving left right now?
	bool left = 0;
	
	score = 0;
	
	//buffer is a place in mmory for the string
	char buffer[41];
	//sprintf puts the string in the array/memory locations buffer
	sprintf(buffer, "Score: %d", score);
	//display iT!!
	drawString(150, 5, buffer, GREEN);
	
	//keep getting new bags til one hits the water
	while(!water) {
		//randomizer work
		seed++; 
		srand(seed);
		
		//current plastic bag
		PLASTICBAG p;
		p.row = 0; 
		
		//all starting-bag-rows will be 0 but col and which picture are the randomized parts
		//for col, make the bags fall from a somewhat centralized region so its a little easier 
		p.col = rand() % 228;
		int picind = rand() % 3;
		
		//the score increments when user catches a bag so we must have this check
		volatile bool caught = 0;
		
		//for one bag-> loop until the bag in question is either caught ot lost
		while (!water && !caught) {
			seed++;
			//draws randomized image
			//pb width = 12, pb heigth = 12;
			drawImage(p.row, p.col, 12, 12, images[picind]);
			
			//loop to slow down the free fall of the bag
			volatile int size = 0;
			for (int i = 0; i < 4000; i++) {
				size = size + 1;
			}
		
			//erasing the bag and redrawing its new position
			drawRectangle(p.row, p.col, 12, 12, BLACK);
			p.row = p.row + 1;
			drawImage(p.row, p.col, 12,12,images[picind]);
			
			//hits water
			if (p.row >=60) {
				water = 1;
				endGame();
			}
			
			//motion keys - user control, bird moves 5 positions at a time
			if (KEY_DOWN_NOW(BUTTON_UP)) {
				row = row - 5;
				if (row < 0) {
					row = 0;
				}
			}
			if (KEY_DOWN_NOW(BUTTON_DOWN)) {
				row = row + 5;
				if (row > 58) {
					row = 58;
				}
			}
			//left functiionality to introduce ability to flip the bird
			if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
				col = col + 5;
				if (col > 239 - 19 + 1) {
					col = 239 - 19;
				}
				left = false;
			}
			if (KEY_DOWN_NOW(BUTTON_LEFT)) {
				col = col - 5;
				if (col < 0) {
					col = 0;
				}
				left = true;
			}
			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				startGame();
			}
			
			//erase and redraw the bird, image depending on current (left) state
			waitForVblank();
			drawRectangle(oldrow, oldcol, 19, 15, BLACK);
			if (left) {
				drawImage(row, col, 19, 15, birdleft);
			} else {
				drawImage(row, col, 19, 15, bird2);
			}
			
			//update oldcol and row so they can erase
			oldcol = col;
			oldrow = row;
			
			//Condition that checks a collision
			//check if plastic bag right and left col lie in between bird's right and left cols
			//same with rows as well	
			if ((((p.row <= row + 19) && (row <= p.row)) || ((p.row + 12 <= row + 19) && (row <= p.row + 12))) 				&&
			(((p.col <= col + 15) && (col <= p.col)) || ((p.col + 12 <= col + 15) && (col <= p.col + 12)))) {
				drawRectangle(p.row, p.col, 12, 12, BLACK);
				//if yes, check which bird to draw
				if (left) {
					drawImage(row, col, 19, 15, birdleft);
				} else {
					drawImage(row, col, 19, 15, bird2);
				}
				
				//increment score and update board
				caught = true;
				score++;
				sprintf(buffer, "Score: %d", score);
				drawRectangle(145, 0, 240, 15, BLACK);
				drawString(150, 5, buffer, GREEN);
			}	
		}
	}
}

//you lose
void endGame() {
	//images of plastic bag in the water
	//loops through to display each one
	const u16* endpics[] = {end3, end4, end2, end1};
	for (int k = 0; k < 4; k++) {
		const u16* n = endpics[k];
		drawImage(0,0,240,160,n);
		int bye = 0;
		for (int l = 0; l < 70*4000; l++) {
			bye++;
		}
	}
	//endscreen
	drawRectangle(0,0,240,160,BLACK);
	char buffer[41];
	sprintf(buffer, "Wow you only caught %d bags", score);
	drawString(5, 35, buffer, RED);
	drawImage(20, 20, 200, 120, yousuck);
	
	//keep waiting to restart 
	while(1) {
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			seed++;
			srand(seed);
			startGame();
		}
	}	
}

