// Kristian Zhelyazkov

#include "mylib.h"

u16 *videoBuffer = (u16 *)0x6000000;


void setPixel(int r, int c, u16 color)
{
	videoBuffer[OFFSET(r,c, 240)] = color; 
}

void drawRect(int row, int col, int height, int width, volatile u16 color) {
	
	for (int r = 0; r < height; r++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(row + r, col, 240)];
		DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
	}
}

void drawHollowRect(int r, int c, int width, int height, u16 color)
{
	int i, j;
	for (i = 0; i <= width; i++) {
		setPixel(r, c + i, color);
		setPixel(r + height, c + i, color);
	}

	for (j = 0; j <= height; j++) {
		setPixel(r + j, c, color);
		setPixel(r + j, c + width, color);
	}

}

void drawChar(int row, int col, char ch, u16 color)
{
	int r,c;
	for(r=0; r<8; r++)
	{
		for(c=0; c<6; c++)
		{
			if(fontdata_6x8[OFFSET(r, c, 6)+ch*48])
			{
				setPixel(row+r, col+c, color);
			}
		}
	}
}

void drawString(int row, int col, char *str, u16 color)
{
	while(*str)
	{
		drawChar(row, col, *str++, color);
		col += 6;
	}
}

void drawImage3(int r, int c, int width, int height, const u16* image) {
	int row;
	for(row = 0; row < height; row++) {
		DMA[3].src = image+row * width;
		DMA[3].dst = videoBuffer + OFFSET(row+r, c, 240);
		DMA[3].cnt = DMA_ON | width;
	}
}
				
void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}
