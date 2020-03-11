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

#define TIMER_BLINK500  1     /*500ms定时器事件标志号*/


const int mseconds500 = 500;   

static char cursor[] = {'|'};/*储存光标*/ 
static char LeftText[100]; /*光标左边字符串缓冲区*/
static char RightText[100]; /*光标右边字符串缓冲区*/
static int LeftTextLen = 0;/*光标左边字符串长度*/
static int RightTextLen = 0;/*光标右边字符串长度*/
static double textx, texty; /*字符串的起始位置*/
static double cursorx, cursory;/*光标位置*/ 

static bool isBlink = TRUE;   /*是否闪烁标志*/
static bool isDisplayText = FALSE; /*字符串显示标志*/

void DelText(void);/*删除所有字符函数*/
void PutText(void);/*输出缓冲区中所有字符函数*/
 
void KeyboardEventProcess(int key,int event);/*键盘消息回调函数*/
void CharEventProcess(char c);/*字符消息回调函数*/
void TimerEventProcess(int timerID);/*定时器消息回调函数*/


void Main() /*仅初始化执行一次*/
{
    InitGraphics();        	
	
	registerKeyboardEvent(KeyboardEventProcess);/*注册键盘消息回调函数*/
	registerCharEvent(CharEventProcess);/*注册字符消息回调函数*/
	registerTimerEvent(TimerEventProcess);/*注册定时器消息回调函数*/
    
    textx = GetWindowWidth()/20;
    texty = GetWindowHeight()/2;
    MovePen(textx, texty);/*移动光标到初始位置*/ 
    startTimer(TIMER_BLINK500, mseconds500);/*500ms定时器触发*/
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

void KeyboardEventProcess(int key,int event)/*每当产生键盘消息，调用此回调函数*/
{
 	 int i = 0;
     switch (event) {
	 	case KEY_DOWN:
			 switch (key) {
			 	 case VK_DELETE:/*delete删除光标右边一个字符*/ 
			 	 	if (RightTextLen > 0){
					     DelText();/*擦除已经写上去的符号*/ 
		                 RightTextLen--;                         
				    	 for(i = 0; i <= RightTextLen; i++) {
					 	    RightText[i] = RightText[i + 1];
					     } /*删除右部字符缓冲区首个字符，其余符号依次左移一位*/ 
	                     PutText();/*输出当前字符缓冲区全部符号 */
		        	}
		        	break;
			     case VK_LEFT:
			          DelText();/*擦除已经写上去的符号*/ 			         
					 for(i = RightTextLen; i > 0; i--) {
					 	RightText[i] = RightText[i - 1];
					 } 
					 RightText[0] = LeftText[--LeftTextLen];
					 LeftText[LeftTextLen] = '\0';
					 RightText[++RightTextLen] = '\0';  
					  /*将左部字符缓冲区最后一个字符移到右部缓冲区首位，右部其余符号依次后移一位*/ 
					 PutText();/*输出当前字符缓冲区全部符号 */
                     break;
			     case VK_RIGHT:
			         DelText();/*擦除已经写上去的符号*/ 			          
                     LeftText[LeftTextLen++] = RightText[0];
                     LeftText[LeftTextLen] = '\0';
                     RightTextLen--;
					 for(i = 0; i <= RightTextLen; i++) {
					 	RightText[i] = RightText[i + 1];
					 }/*将右部字符缓冲区首个字符移到左部缓冲区末位，右部其余符号依次左移一位*/
					 PutText();/*输出当前字符缓冲区全部符号 */
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
       case '\r':  /*按回车时将所有字符在终端窗口显示*/
           InitConsole();
           printf("%s%s",LeftText,RightText);
           FreeConsole();
   	 	   break;
 	   case 27: /*ESC*/
 	       break;
 	   case 8:/*backspace删除光标左边一个字符*/
 	       if (LeftTextLen > 0){
 	       	   DelText();/*擦除已经写上去的符号*/ 
		       LeftText[--LeftTextLen] = '\0';
			   PutText();/*输出当前字符缓冲区全部符号 */
			}
 	       
		   break;
      default:
      	   DelText();/*擦除已经写上去的符号*/ 
	 	   LeftText[LeftTextLen++] = c;/*将当前字符加入到光标左边字符缓冲区中*/
	 	   LeftText[LeftTextLen] = '\0';
	 	   PutText();/*输出当前字符缓冲区全部符号 */                                                           
	 	   break;
    }
}

void TimerEventProcess(int timerID)/*定时器实现光标闪烁*/ 
{
      bool erasemode;
	  cursorx = textx +TextStringWidth(LeftText);
      cursory = texty;
      if (!isBlink) return;
	      erasemode = GetEraseMode();
          MovePen(cursorx, cursory);/*光标位置*/
		  SetEraseMode(isDisplayText);/*根据当前显示标志来决定是显示还是隐藏字符串*/
		  DrawTextString(cursor);
	      SetEraseMode(erasemode);
		  isDisplayText = !isDisplayText;/*交替显示/隐藏字符串符号*/
}
