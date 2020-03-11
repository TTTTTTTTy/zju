#include "../include/graphics.h"
#include "../include/extgraph.h"
#include "../include/genlib.h"
#include "../include/simpio.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#define deltax 0.03
#define deltay 0.03

#define TIMER_BLINK500  1     /*500ms��ʱ���¼���־��*/


const int mseconds500 = 500;   

static char cursor[] = {'|'};/*������*/ 
static char LeftText[100]; /*�������ַ���������*/
static char RightText[100]; /*����ұ��ַ���������*/
static int LeftTextLen = 0;/*�������ַ�������*/
static int RightTextLen = 0;/*����ұ��ַ�������*/
static double textx, texty; /*�ַ�������ʼλ��*/
static double cursorx, cursory;/*���λ��*/ 

static bool isBlink = TRUE;   /*�Ƿ���˸��־*/
static bool isDisplayText = FALSE; /*�ַ�����ʾ��־*/

void DelText(void);/*ɾ�������ַ�����*/
void PutText(void);/*����������������ַ�����*/
 
void KeyboardEventProcess(int key,int event);/*������Ϣ�ص�����*/
void CharEventProcess(char c);/*�ַ���Ϣ�ص�����*/
void TimerEventProcess(int timerID);/*��ʱ����Ϣ�ص�����*/


void Main() /*����ʼ��ִ��һ��*/
{
    InitGraphics();        	
	
	registerKeyboardEvent(KeyboardEventProcess);/*ע�������Ϣ�ص�����*/
	registerCharEvent(CharEventProcess);/*ע���ַ���Ϣ�ص�����*/
	registerTimerEvent(TimerEventProcess);/*ע�ᶨʱ����Ϣ�ص�����*/
    
    textx = GetWindowWidth()/20;
    texty = GetWindowHeight()/2;
    MovePen(textx, texty);/*�ƶ���굽��ʼλ��*/ 
    startTimer(TIMER_BLINK500, mseconds500);/*500ms��ʱ������*/
}

void DelText(void){
	SetEraseMode(TRUE);
    MovePen(textx, texty);
	DrawTextString(LeftText);
	DrawTextString(cursor);
	DrawTextString(RightText);
	SetEraseMode(FALSE);
} 

void PutText(void){
	MovePen(textx, texty);
	DrawTextString(LeftText);
	DrawTextString(cursor);
	DrawTextString(RightText);
}

void KeyboardEventProcess(int key,int event)/*ÿ������������Ϣ�����ô˻ص�����*/
{
 	 int i = 0;
     switch (event) {
	 	case KEY_DOWN:
			 switch (key) {
			 	 case VK_DELETE:/*deleteɾ������ұ�һ���ַ�*/ 
			 	 	if (RightTextLen > 0){
					     DelText();/*�����Ѿ�д��ȥ�ķ���*/ 
		                 RightTextLen--;                         
				    	 for(i = 0; i <= RightTextLen; i++) {
					 	    RightText[i] = RightText[i + 1];
					     } /*ɾ���Ҳ��ַ��������׸��ַ������������������һλ*/ 
	                     PutText();/*�����ǰ�ַ�������ȫ������ */
		        	}
		        	break;
			     case VK_LEFT:
			          DelText();/*�����Ѿ�д��ȥ�ķ���*/ 			         
					 for(i = RightTextLen; i > 0; i--) {
					 	RightText[i] = RightText[i - 1];
					 } 
					 RightText[0] = LeftText[--LeftTextLen];
					 LeftText[LeftTextLen] = '\0';
					 RightText[++RightTextLen] = '\0';  
					  /*�����ַ����������һ���ַ��Ƶ��Ҳ���������λ���Ҳ�����������κ���һλ*/ 
					 PutText();/*�����ǰ�ַ�������ȫ������ */
                     break;
			     case VK_RIGHT:
			         DelText();/*�����Ѿ�д��ȥ�ķ���*/ 			          
                     LeftText[LeftTextLen++] = RightText[0];
                     LeftText[LeftTextLen] = '\0';
                     RightTextLen--;
					 for(i = 0; i <= RightTextLen; i++) {
					 	RightText[i] = RightText[i + 1];
					 }/*���Ҳ��ַ��������׸��ַ��Ƶ��󲿻�����ĩλ���Ҳ����������������һλ*/
					 PutText();/*�����ǰ�ַ�������ȫ������ */
                     break;
			 }
			 break;
		case KEY_UP:
			 break;
	 }	 
}

void CharEventProcess(char c)
{
 	 static char str[2] = {0, 0};

     switch (c) {
       case '\r':  /*���س�ʱ�������ַ����ն˴�����ʾ*/
           InitConsole();
           printf("%s%s",LeftText,RightText);
           FreeConsole();
   	 	   break;
 	   case 27: /*ESC*/
 	       break;
 	   case 8:/*backspaceɾ��������һ���ַ�*/
 	       if (LeftTextLen > 0){
 	       	   DelText();/*�����Ѿ�д��ȥ�ķ���*/ 
		       LeftText[--LeftTextLen] = '\0';
			   PutText();/*�����ǰ�ַ�������ȫ������ */
			}
 	       
		   break;
      default:
      	   DelText();/*�����Ѿ�д��ȥ�ķ���*/ 
	 	   LeftText[LeftTextLen++] = c;/*����ǰ�ַ����뵽�������ַ���������*/
	 	   LeftText[LeftTextLen] = '\0';
	 	   PutText();/*�����ǰ�ַ�������ȫ������ */                                                           
	 	   break;
    }
}

void TimerEventProcess(int timerID)/*��ʱ��ʵ�ֹ����˸*/ 
{
      bool erasemode;
	  cursorx = textx +TextStringWidth(LeftText);
      cursory = texty;
      if (!isBlink) return;
	      erasemode = GetEraseMode();
          MovePen(cursorx, cursory);/*���λ��*/
		  SetEraseMode(isDisplayText);/*���ݵ�ǰ��ʾ��־����������ʾ���������ַ���*/
		  DrawTextString(cursor);
	      SetEraseMode(erasemode);
		  isDisplayText = !isDisplayText;/*������ʾ/�����ַ�������*/
}
