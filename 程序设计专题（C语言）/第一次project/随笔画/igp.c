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

void MouseEventProcess(int x, int y, int button, int event);/*鼠标消息回调函数*/


void Main() /*仅初始化执行一次*/
{
    InitGraphics();        	
	registerMouseEvent(MouseEventProcess);/*注册鼠标消息回调函数*/
    SetPenSize(1);  
}

void MouseEventProcess(int x, int y, int button, int event)
{
 	 static bool isMove = FALSE;/*画线标志*/
 	 static double omx = 0.0, omy = 0.0;
     double mx, my;

 	 mx = ScaleXInches(x);/*pixels --> inches*/
 	 my = ScaleYInches(y);/*pixels --> inches*/

     switch (event) {
         case BUTTON_DOWN:
   		      if (button == LEFT_BUTTON) {
				isMove = TRUE;/*开始画线*/	   
			  } 
              MovePen(mx, my);
		      omx = mx; omy = my;
              break;
    	 case BUTTON_DOUBLECLICK:
			  break;
         case BUTTON_UP:
  		 	  if (button == LEFT_BUTTON) {
  		 	  	isMove = FALSE;	/*结束画线*/ 
  		 	  }
              break;
         case MOUSEMOVE:
			  if (isMove) {
				  DrawLine(mx - omx, my - omy);
				  omx = mx;
				  omy = my;
			  } 
              break;
     }	
}


