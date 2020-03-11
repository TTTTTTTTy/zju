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

int dataMap[LENGTHOFMAZE][LENGTHOFMAZE]; /*0Ϊͨ·��1Ϊǽ��*/ 
int pathGone[LENGTHOFMAZE][LENGTHOFMAZE];/*��¼��ͼ�ϵĵ��Ƿ��߹���δ�߼�Ϊ0���߹���Ϊ1*/ 
int path[LENGTHOFMAZE][LENGTHOFMAZE];/*��¼ÿ�����ǵڼ����߹��ģ�Ϊ���ҵ������Ժ����*/ 
int dotX[LENGTHOFMAZE * LENGTHOFMAZE];/*���������߹��ĵ�����±�*/ 
int dotY[LENGTHOFMAZE * LENGTHOFMAZE];/*���������߹��ĵ�����±�*/ 
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
	/*���Թ�*/ 
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
    /*�ҵ����·��*/
    index = 1;
    count = 1;
	path[1][1] = index;
	pathGone[1][1] = 1;
	dotX[count] = 1;
	dotY[count] = 1;
	FindPath(count, count); 	
}  

/*�����ֵ�ͼת��Ϊͼ��*/
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

/*��һ��ʵ���Ҵ���ɫ�߿�ĸ���*/
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

/*��һ������ɫ�ߵİ�ɫʵ��Բ*/
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

/*�õݹ�ʵ�����ֵ�ͼ��ÿһ����ż���������������ǽ�ڣ����������������������*/ 
void SetMaze(int x1, int x2, int y1, int y2){
	int cx, cy, i, j, isClosed;
	/*�������������С�����У�����*/ 
	if (x2 - x1 < 2 || y2 - y1 < 2) return;
 	/*���ȡһ������Ϊż���ĵ�*/ 
 	cx = rand() % (x2 - x1 + 1) + x1;
 	if(cx % 2 != 0) cx += 1;
 	cy = rand() % (y2 - y1 + 1) + y1;
 	if(cy % 2 != 0) cy += 1;
 	/*���õ����ڵ��к�����Ϊǽ��*/ 
 	for(i = x1, j = cy; i <= x2; i++){
        dataMap[i][j] = 1;
	 }
	for(i = cx, j = y1; j <= y2; j++){
		dataMap[i][j] = 1;
	}
	/*���Ķ�ǽ�������������*/
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
	/*��ǽ�ڷֳɵ��ĸ��������ظ���������*/
	SetMaze(x1, cx - 1, y1, cy - 1);
	SetMaze(x1, cx - 1, cy + 1, y2);
	SetMaze(cx + 1, x2, y1, cy - 1);
	SetMaze(cx + 1, x2, cy + 1, y2);
}

/*��һ��ǽ�ڵ��������������һ�ȿ���ͨ������*/
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

/*�ж��Ƿ�����߶�*/ 
bool isMoved(int x, int y, int dx, int dy){
	return(pathGone[x + dx][y + dy] == 0 && dataMap[x + dx][y + dy] == 0
           && x + dx > 0 && x + dx < LENGTHOFMAZE - 1
	       && y + dy > 0 && y + dy < LENGTHOFMAZE - 1);
}

/*�����ܿ����ߵķ������һ�����ж��ܷ��ߵ����ڣ�������ܣ������µĵ�
  ע�⣺x,y�Ƕ�ά������±꣬���Ǻ�������*/
void Move(int x, int y){
	/*����*/ 
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
	/*����*/ 
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
	/*����*/ 
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
	/*����*/ 
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

/*�õݹ�ÿ�ν��������õĵ������ܿ����ߵķ�����һ�񣬲���������Ϊ�µĵ㡣ֱ���ߵ�����*/
void FindPath(int count1, int count2){
	int i;
    index++;
    for(i = count1; i <= count2; i++){
    	Move(dotX[i], dotY[i]); 
    	if (isfind) return;
	}
    FindPath(count2 + 1, count);
} 

/*���ҵ������·����Բ��ǳ���*/
void MarkPath(int i, int j){
    double Ox, Oy;
    Ox = cx + (j + 0.5) * WIDTH + RADIUS;
    Oy = cy + (LENGTHOFMAZE - i - 0.5) * WIDTH;
	DrawWhiteArc(Ox, Oy, RADIUS);
	if(i == 1 && j == 1) return;
	/*�ҵ�֮ǰ���ķ���*/ 
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
