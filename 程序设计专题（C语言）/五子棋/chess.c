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

#define WIDTH        0.3/*���������*/ 
#define RADIUS       0.1/*�������Ӱ뾶*/ 
#define MAXIMUS 15 /*�������̴�С*/

int p[MAXIMUS][MAXIMUS];/*�洢�Ծ���Ϣ,0��ʾ��λ��δ�¹���1��ʾ���ӣ�2��ʾ����*/ 
int player = 1;/*��ǰ���ߵ���ң�0Ϊ���ӣ�1Ϊ����*/ 
double cx, cy; /*�����������Ͻ�����*/ 

int Check(int i, int j);/*ʤ����飬���жϵ�ǰ����λ����û���������������*/
void Change(void); /*�����*/
void luozi(int i, int j); /*�������*/
void DrawWhiteArc(double x, double y, double r);/*��һ����ɫʵ��Բ*/ 
void DrawGrid(double x, double y, double width, int columns, int rows);/*������*/ 
void DrawBox(double x, double y, double width, string color);/*��һ��ʵ�ķ���*/ 
void MouseEventProcess(int x, int y, int button, int event);/*�����Ϣ�ص�����*/
void CharEventProcess(char c);/*�ַ���Ϣ�ص�����*/

void Main()
{   
	int i, j;
	/*������*/ 
    InitGraphics();
    registerMouseEvent(MouseEventProcess);/*ע�������Ϣ�ص�����*/
    registerCharEvent(CharEventProcess);/*ע���ַ���Ϣ�ص�����*/
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
        player=0;  /*��ұ任*/
        return;
    }
    player=1;
}

void luozi(int i, int j) 
{
	double ccx, ccy;/*��ʼ�����ӵ�λ��*/
	ccx = cx + (j + 1) * WIDTH + RADIUS;
	ccy = cy - (i + 1) * WIDTH; 
    if(p[i][j] == 0) /*�жϵ�ǰλ���Ƿ��Ѿ�����*/
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
        case '\r':  /* ע�⣺�س������ﷵ�ص��ַ���'\r'������'\n'*/
		   DrawBox(cx, cy, WIDTH * (MAXIMUS + 1), "WHITE");/*����֮ǰ�����*/
           DrawGrid(cx, cy, WIDTH, MAXIMUS + 1, MAXIMUS + 1); /*��һ���µ�����*/ 
           for(i = 0; i < MAXIMUS; i++){
           	  for(j = 0; j < MAXIMUS; j++){
				 p[i][j] = 0;     	
			  }
		   }/*���öԾ���Ϣ*/ 
           player = 1;/*��������*/       
   	 	   break;	   
      default:
	 	   break;
    }
}

void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;/*��ǰ�������*/
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
    int w = 1, x = 1, y = 1, z = 1, n;/*�ۼƺ�����б��б�ĸ������������ͬ������Ŀ*/
    /*������*/
    for(n = 1;n < 5; n++){
    	if(j - n >= 0 && p[i][j - n] == player + 1)
		    w++;
		else break;
	}
    /*���Ҽ��*/    
    for(n = 1;n < 5; n++){
    	if(j + n <= 14 && p[i][j + n] == player + 1)
		    w++;
		else break;
	} 
	if(w >= 5) return player;/*���ﵽ5�����жϵ�ǰ�������ΪӮ��*/
    /*���ϼ��*/
    for(n = 1;n < 5; n++){
    	if(i - n >= 0 && p[i - n][j] == player + 1)
		    x++;
		else break;
	}
    /*���¼��*/    
    for(n = 1;n < 5; n++){
    	if(i + n <= 14 && p[i + n][j] == player + 1)
		    x++;
		else break;
	} 
	if(x >= 5) return player;/*���ﵽ5�����жϵ�ǰ�������ΪӮ��*/
    /*�����ϼ��*/
    for(n = 1;n < 5; n++){
    	if(i - n >= 0 && j - n >= 0 && p[i - n][j - n] == player + 1)
		    y++;
		else break;
	}
    /*�����¼��*/    
    for(n = 1;n < 5; n++){
    	if(i + n <= 14 && j + n <= 14 && p[i + n][j + n] == player + 1)
		    y++;
		else break;
	}
	if(y >= 5) return player;/*���ﵽ5�����жϵ�ǰ�������ΪӮ��*/
	/*�����ϼ��*/
    for(n = 1;n < 5; n++){
    	if(i - n >= 0 && j + n <= 14 && p[i - n][j + n] == player + 1)
		    z++;
		else break;
	}
    /*�����¼��*/    
    for(n = 1;n < 5; n++){
    	if(i + n <= 14 && j - n >= 0 && p[i + n][j - n] == player + 1)
		    z++;
		else break;
	}
	if(z >= 5) return player;/*���ﵽ5�����жϵ�ǰ�������ΪӮ��*/          
    return 2;/*��û�м�鵽�����飬�򷵻�2��ʾ��û����Ҵ��ʤ��*/
}
