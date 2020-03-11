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

#define WIDTH        0.3/*定义棋格宽度*/ 
#define RADIUS       0.1/*定义棋子半径*/ 
#define MAXIMUS 15 /*定义棋盘大小*/

int p[MAXIMUS][MAXIMUS];/*存储对局信息,0表示此位置未下过，1表示白子，2表示黑子*/ 
int player = 1;/*当前行走的玩家，0为白子，1为黑子*/ 
double cx, cy; /*储存棋盘左上角坐标*/ 

int Check(int i, int j);/*胜负检查，即判断当前走子位置有没有造成五连珠的情况*/
void Change(void); /*换玩家*/
void luozi(int i, int j); /*玩家落子*/
void DrawWhiteArc(double x, double y, double r);/*画一个白色实心圆*/ 
void DrawGrid(double x, double y, double width, int columns, int rows);/*画棋盘*/ 
void DrawBox(double x, double y, double width, string color);/*画一个实心方格*/ 
void MouseEventProcess(int x, int y, int button, int event);/*鼠标消息回调函数*/
void CharEventProcess(char c);/*字符消息回调函数*/

void Main()
{   
	int i, j;
	/*画棋盘*/ 
    InitGraphics();
    registerMouseEvent(MouseEventProcess);/*注册鼠标消息回调函数*/
    registerCharEvent(CharEventProcess);/*注册字符消息回调函数*/
    cx = GetWindowWidth() / 2 - WIDTH * (MAXIMUS + 1) / 2;
	cy = GetWindowHeight() / 2 + WIDTH * (MAXIMUS + 1) / 2;
	DrawGrid(cx, cy, WIDTH, MAXIMUS + 1, MAXIMUS + 1);
    	
}  

void DrawGrid(double x, double y, double width, int columns, int rows)
{
    int i, j;   
    for (i = 0; i < columns;i++) {
        for (j = 0; j < rows; j++) {
            DrawBox(x + i * WIDTH, y - j * WIDTH, WIDTH, "YELLOW");
        }
    }
}

void DrawBox(double x, double y, double width, string color)
{
	StartFilledRegion(1.0);
    SetPenColor(color);
    MovePen(x, y);
    DrawLine(width, 0);
	DrawLine(0, -width);
    DrawLine(-width, 0);
    DrawLine(0, width);  
    EndFilledRegion();
	SetPenColor("BLACK");
	MovePen(x, y);
    DrawLine(width, 0);
	DrawLine(0, -width);
    DrawLine(-width, 0);
    DrawLine(0, width);   
}

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

void Change(void) 
{
    if(player)
    {
        player=0;  /*玩家变换*/
        return;
    }
    player=1;
}

void luozi(int i, int j) 
{
	double ccx, ccy;/*开始画棋子的位置*/
	ccx = cx + (j + 1) * WIDTH + RADIUS;
	ccy = cy - (i + 1) * WIDTH; 
    if(p[i][j] == 0) /*判断当前位置是否已经落子*/
    {
    	p[i][j] = player + 1;
        if(player)
        {
            MovePen(ccx, ccy);
            StartFilledRegion(1.0); 
	        SetPenColor("BLACK");
	        DrawArc(RADIUS, 0, 360);
        	EndFilledRegion();
        }
        else
        {
            DrawWhiteArc(ccx, ccy, RADIUS);
        }
    }
}

void CharEventProcess(char c)
{
	int i, j;
    switch (c) {	
        case '\r':  /* 注意：回车在这里返回的字符是'\r'，不是'\n'*/
		   DrawBox(cx, cy, WIDTH * (MAXIMUS + 1), "WHITE");/*擦除之前的棋局*/
           DrawGrid(cx, cy, WIDTH, MAXIMUS + 1, MAXIMUS + 1); /*画一个新的棋盘*/ 
           for(i = 0; i < MAXIMUS; i++){
           	  for(j = 0; j < MAXIMUS; j++){
				 p[i][j] = 0;     	
			  }
		   }/*重置对局信息*/ 
           player = 1;/*黑子先行*/       
   	 	   break;	   
      default:
	 	   break;
    }
}

void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;/*当前鼠标坐标*/
	int i, j;
	char text1[] = {"Game Over! White is the winner!"};
	char text2[] = {"Game Over! Black is the winner!"};
	char text3[] = {"Press enter key to start a new game."};
	mx = ScaleXInches(x);/*pixels --> inches*/
	my = ScaleYInches(y);/*pixels --> inches*/
	i = (cy - 0.5 * WIDTH - my) / WIDTH;
	j = (mx - 0.5 * WIDTH - cx) / WIDTH;
	switch (event) {
		case BUTTON_DOWN:
			if(i >= 0 && i < 15 && j >= 0 && j < 15){
				luozi(i, j);
			    switch(Check(i, j)){
			    	case 0:
			    		MovePen(GetWindowWidth() / 2 - TextStringWidth(text1) / 2, GetWindowHeight() / 2 + 0.1);
			    		SetPenColor("RED");
					    DrawTextString(text1);
					    MovePen(GetWindowWidth() / 2 - TextStringWidth(text3) / 2, GetWindowHeight() / 2 - 0.1);
					    DrawTextString(text3);
					    break;
					case 1:
						MovePen(GetWindowWidth() / 2 - TextStringWidth(text2) / 2, GetWindowHeight() / 2 + 0.1);
			    		SetPenColor("RED");
						DrawTextString(text2);
						MovePen(GetWindowWidth() / 2 - TextStringWidth(text3) / 2, GetWindowHeight() / 2 - 0.1);
					    DrawTextString(text3);
					    break;
				    case 2:
				    	break;
				}
				Change();
			}   
			break;
		case BUTTON_UP: 
		    break;
    }
}

int Check(int i, int j)
{
    int w = 1, x = 1, y = 1, z = 1, n;/*累计横竖正斜反斜四个方向的连续相同棋子数目*/
    /*向左检查*/
    for(n = 1;n < 5; n++){
    	if(j - n >= 0 && p[i][j - n] == player + 1)
		    w++;
		else break;
	}
    /*向右检查*/    
    for(n = 1;n < 5; n++){
    	if(j + n <= 14 && p[i][j + n] == player + 1)
		    w++;
		else break;
	} 
	if(w >= 5) return player;/*若达到5个则判断当前走子玩家为赢家*/
    /*向上检查*/
    for(n = 1;n < 5; n++){
    	if(i - n >= 0 && p[i - n][j] == player + 1)
		    x++;
		else break;
	}
    /*向下检查*/    
    for(n = 1;n < 5; n++){
    	if(i + n <= 14 && p[i + n][j] == player + 1)
		    x++;
		else break;
	} 
	if(x >= 5) return player;/*若达到5个则判断当前走子玩家为赢家*/
    /*向左上检查*/
    for(n = 1;n < 5; n++){
    	if(i - n >= 0 && j - n >= 0 && p[i - n][j - n] == player + 1)
		    y++;
		else break;
	}
    /*向右下检查*/    
    for(n = 1;n < 5; n++){
    	if(i + n <= 14 && j + n <= 14 && p[i + n][j + n] == player + 1)
		    y++;
		else break;
	}
	if(y >= 5) return player;/*若达到5个则判断当前走子玩家为赢家*/
	/*向右上检查*/
    for(n = 1;n < 5; n++){
    	if(i - n >= 0 && j + n <= 14 && p[i - n][j + n] == player + 1)
		    z++;
		else break;
	}
    /*向左下检查*/    
    for(n = 1;n < 5; n++){
    	if(i + n <= 14 && j - n >= 0 && p[i + n][j - n] == player + 1)
		    z++;
		else break;
	}
	if(z >= 5) return player;/*若达到5个则判断当前走子玩家为赢家*/          
    return 2;/*若没有检查到五连珠，则返回2表示还没有玩家达成胜利*/
}
