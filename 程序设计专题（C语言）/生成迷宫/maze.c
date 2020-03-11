#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <wincon.h>
#include <stddef.h>

#include "genlib.h"
#include "gcalloc.h"
#include "strlib.h"
#include "extgraph.h"
#include "graphics.h"

#define WIDTH        0.2
#define RADIUS       0.07
#define LENGTHOFMAZE 31

int dataMap[LENGTHOFMAZE][LENGTHOFMAZE]; /*0为通路，1为墙壁*/ 
int pathGone[LENGTHOFMAZE][LENGTHOFMAZE];/*记录地图上的点是否走过，未走记为0，走过记为1*/ 
int path[LENGTHOFMAZE][LENGTHOFMAZE];/*记录每个点是第几步走过的，为了找到出口以后回溯*/ 
int dotX[LENGTHOFMAZE * LENGTHOFMAZE];/*储存所有走过的点的行下标*/ 
int dotY[LENGTHOFMAZE * LENGTHOFMAZE];/*储存所有走过的点的列下标*/ 
int index, count, isfind;
double cx, cy; 

bool isMoved(int x, int y, int dx, int dy);
void Move(int x, int y);
void FindPath(int count1, int count2);
void MarkPath(int i, int j);
void DrawWhiteArc(double x, double y, double r);
void DrawGrid(double x, double y, double width, int columns, int rows);
void DrawBox(double x, double y, double width, string color);
void SetMaze(int x1, int x2, int y1, int y2);
void SetDoor(int x1, int x2, int y1, int y2);

void Main()
{   
	int i, j;
	/*画迷宫*/ 
	srand(time(0));
	SetMaze(1, LENGTHOFMAZE - 2, 1, LENGTHOFMAZE - 2);
	for(i = 0;i < LENGTHOFMAZE; i++){
		for(j = 0; j < LENGTHOFMAZE; j++){
			if(i == 0 || i == LENGTHOFMAZE - 1 || j == 0 || j == LENGTHOFMAZE - 1)
			     dataMap[i][j] = 1;
		    else if(i == 1 && j == 1 || i == LENGTHOFMAZE - 2 && LENGTHOFMAZE - 2)
		         dataMap[i][j] = 0;
		}
	}	
    InitGraphics();
    cx = GetWindowWidth() / 2 - WIDTH * LENGTHOFMAZE / 2;
	cy = GetWindowHeight() / 2 - WIDTH * LENGTHOFMAZE / 2;
    DrawGrid(cx, cy, WIDTH, LENGTHOFMAZE, LENGTHOFMAZE);  
    /*找到最佳路径*/
    index = 1;
    count = 1;
	path[1][1] = index;
	pathGone[1][1] = 1;
	dotX[count] = 1;
	dotY[count] = 1;
	FindPath(count, count); 	
}  

/*将数字地图转化为图形*/
void DrawGrid(double x, double y, double width, int columns, int rows)
{
    int i, j;   
    for (i = 0; i < columns;i++) {
        for (j = 0; j < rows; j++) {
        	if(dataMap[rows - 1 - j][i] == 0)
            DrawBox(x + i * WIDTH, y + j * WIDTH, WIDTH, "BLUE");
            if(dataMap[rows - 1 - j][i] == 1)
            DrawBox(x + i * WIDTH, y + j * WIDTH, WIDTH, "RED");
        }
    }
}

/*画一个实心且带黑色边框的格子*/
void DrawBox(double x, double y, double width, string color)
{
	StartFilledRegion(1.0);
    SetPenColor(color);
    MovePen(x, y);
    DrawLine(0,width);
    DrawLine(width, 0);
    DrawLine(0, -width);
    DrawLine(-width, 0);
    EndFilledRegion();
	SetPenSize(2);
	SetPenColor("BLACK");
	MovePen(x, y);
    DrawLine(0,width);
    DrawLine(width, 0);
    DrawLine(0, -width);
    DrawLine(-width, 0);
	SetPenSize(1);  
}

/*画一个带黑色边的白色实心圆*/
void DrawWhiteArc(double x, double y, double r){
	MovePen(x, y);
	StartFilledRegion(1.0); 
	SetPenColor("WHITE");
	DrawArc(r, 0, 360);
	EndFilledRegion();
	MovePen(x, y);
	SetPenColor("BLACK");
	SetPenSize(2);
	DrawArc(r, 0, 360);
	SetPenSize(1);
} 

/*用递归实现数字地图，每一次在偶数行随机设置两行墙壁，并在奇数行随机开三个门*/ 
void SetMaze(int x1, int x2, int y1, int y2){
	int cx, cy, i, j, isClosed;
	/*如果行数或列数小于三行，返回*/ 
	if (x2 - x1 < 2 || y2 - y1 < 2) return;
 	/*随机取一个坐标为偶数的点*/ 
 	cx = rand() % (x2 - x1 + 1) + x1;
 	if(cx % 2 != 0) cx += 1;
 	cy = rand() % (y2 - y1 + 1) + y1;
 	if(cy % 2 != 0) cy += 1;
 	/*将该点所在的行和列设为墙壁*/ 
 	for(i = x1, j = cy; i <= x2; i++){
        dataMap[i][j] = 1;
	 }
	for(i = cx, j = y1; j <= y2; j++){
		dataMap[i][j] = 1;
	}
	/*在四堵墙上随机开三扇门*/
	isClosed = rand() % 4;
	switch(isClosed){
		case 0:
			SetDoor(cx + 1, x2, cy, cy);
			SetDoor(cx, cx, cy + 1, y2);
			SetDoor(x1, cx-1, cy, cy);
			break;
		case 1:
			SetDoor(cx, cx, y1, cy - 1);
			SetDoor(cx, cx, cy + 1, y2);
			SetDoor(x1, cx-1, cy, cy);
			break;
		case 2:
			SetDoor(cx, cx, y1, cy - 1);
			SetDoor(cx + 1, x2, cy, cy);
			SetDoor(x1, cx-1, cy, cy);
			break;
		case 3:
			SetDoor(cx, cx, y1, cy - 1);
			SetDoor(cx + 1, x2, cy, cy);
			SetDoor(x1, cx-1, cy, cy);
			break;		
	}
	/*在墙壁分成的四个区域中重复上述操作*/
	SetMaze(x1, cx - 1, y1, cy - 1);
	SetMaze(x1, cx - 1, cy + 1, y2);
	SetMaze(cx + 1, x2, y1, cy - 1);
	SetMaze(cx + 1, x2, cy + 1, y2);
}

/*在一行墙壁的奇数列上随机开一扇可以通过的门*/
void SetDoor(int x1, int x2, int y1, int y2){
	int cx, cy;
	if(x1 == x2){
		cx = x1;
		cy = rand() % (y2 - y1 + 1) + y1;
		if(cy % 2 == 0) cy += 1;
		dataMap[cx][cy] = 0; 
	}
	if(y1 == y2){
		cy = y1;
		cx = rand() % (x2 - x1 + 1) + x1;
		if(cx % 2 == 0) cx += 1;
		dataMap[cx][cy] = 0;
	}
}

/*判断是否可以走动*/ 
bool isMoved(int x, int y, int dx, int dy){
	return(pathGone[x + dx][y + dy] == 0 && dataMap[x + dx][y + dy] == 0
           && x + dx > 0 && x + dx < LENGTHOFMAZE - 1
	       && y + dy > 0 && y + dy < LENGTHOFMAZE - 1);
}

/*往四周可以走的方向各走一步并判断能否走到出口，如果不能，设置新的点
  注意：x,y是二维数组的下标，并非横纵坐标*/
void Move(int x, int y){
	/*向左*/ 
    if(isMoved(x, y, 0, -1)){
    	if(x == LENGTHOFMAZE - 2 && y - 1 == LENGTHOFMAZE - 2){
    		index--;
    		MarkPath(LENGTHOFMAZE - 2, LENGTHOFMAZE - 2);
    		isfind = 1;
    		return;
		}   	    
    	path[x][y - 1] = index;
    	count++;
    	dotX[count] = x;
    	dotY[count] = y - 1;
    	pathGone[x][y - 1] = 1;
	}
	/*向下*/ 
    if(isMoved(x, y, 1, 0)){
    	if(x + 1 == LENGTHOFMAZE - 2 && y == LENGTHOFMAZE - 2){
    		index--;
    		MarkPath(LENGTHOFMAZE - 2, LENGTHOFMAZE - 2);
    		isfind = 1;
    		return;
		}   	
    	path[x + 1][y] = index;
    	count++;
    	dotX[count] = x + 1; 
    	dotY[count] = y;
    	pathGone[x + 1][y] = 1;
	}
	/*向右*/ 
	if(isMoved(x, y, 0, 1)){
		if(x == LENGTHOFMAZE - 2 && y + 1 == LENGTHOFMAZE - 2){
			index--;
    		MarkPath(LENGTHOFMAZE - 2, LENGTHOFMAZE - 2);
    		isfind = 1;
    		return;
		}   	
    	path[x][y + 1] = index;
    	count++;
    	dotX[count] = x;
    	dotY[count] = y + 1;
    	pathGone[x][y + 1] = 1;
	}
	/*向上*/ 
	if(isMoved(x, y, -1, 0)){
		if(x - 1 == LENGTHOFMAZE - 2 && y == LENGTHOFMAZE - 2){
			index--;
    		MarkPath(LENGTHOFMAZE - 2, LENGTHOFMAZE - 2);
    		isfind = 1;
    		return;
		}   	
    	path[x - 1][y] = index;
    	count++;
    	dotX[count] = x - 1;
    	dotY[count] = y;
    	pathGone[x - 1][y] = 1;
	}
} 

/*用递归每次将所有设置的点往四周可以走的方向走一格，并将其设置为新的点。直至走到出口*/
void FindPath(int count1, int count2){
	int i;
    index++;
    for(i = count1; i <= count2; i++){
    	Move(dotX[i], dotY[i]); 
    	if (isfind) return;
	}
    FindPath(count2 + 1, count);
} 

/*将找到的最短路线用圆标记出来*/
void MarkPath(int i, int j){
    double Ox, Oy;
    Ox = cx + (j + 0.5) * WIDTH + RADIUS;
    Oy = cy + (LENGTHOFMAZE - i - 0.5) * WIDTH;
	DrawWhiteArc(Ox, Oy, RADIUS);
	if(i == 1 && j == 1) return;
	/*找到之前来的方向*/ 
	if(path[i - 1][j] == index){
		index--;
		MarkPath(i - 1, j);	
		return;	
	}
	if(path[i][j - 1] == index){
		index--;
		MarkPath(i, j - 1);	
		return;	
	}
	if(path[i + 1][j] == index){
		index--;
		MarkPath(i + 1, j);	
		return;	
	}
	if(path[i][j + 1] == index){
		index--;
		MarkPath(i, j + 1);
		return;		
	}
} 
