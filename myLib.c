#include "myLib.h"
//using DMA to draw the image to the screen given parameters
void drawImage(int row, int col, int width, int height, const u16* image) {
    for (int r = 0; r < height; r++) {
        DMA[3].src = &image[(width * r)];
        DMA[3].dst = &(videoBuffer[((r+row) * 240) + col]);
        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_ON;
    }
}

void waitForVblank() {
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 161);
}

//draw a rectangle of color
void drawRectangle(int row, int col, int width, int height, unsigned short color) {
    for (int r = row; r < height + row; r++) {
        DMA[3].src = &color;
        DMA[3].dst = &(videoBuffer[(r * 240) + col]);
        DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_ON;
    }
} 

//set a pixel to a certain color, used for writing words
void setPixel(int row, int col, unsigned short color) {
	videoBuffer[OFFSET(row, col, 240)] = color;
}

//draw a character using font.c. calls setpixel
void drawChar(int row, int col, char ch, unsigned short color) {
	for(int r = 0; r< 8; r++)
	{
		for(int c=0; c< 6; c++)
		{
			if( fontdata_6x8[OFFSET(r, c, 6) + ch*48] == 1)
			{
				setPixel(r+row, c+col, color);
			}
		}

	}
}

//takes a string to draw, calls drawchar on each char, which calls setpixel on each pixel
void drawString(int row, int col, char str[], unsigned short color) {
	while(*str) {
		drawChar(row, col, *str++, color);
		col += 6;
	}
}

