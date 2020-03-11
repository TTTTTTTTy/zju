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

void MouseEventProcess(int x, int y, int button, int event);/*�����Ϣ�ص�����*/


void Main() /*����ʼ��ִ��һ��*/
{
    InitGraphics();        	
	registerMouseEvent(MouseEventProcess);/*ע�������Ϣ�ص�����*/
    SetPenSize(1);  
}

void MouseEventProcess(int x, int y, int button, int event)
{
 	 static bool isMove = FALSE;/*���߱�־*/
 	 static double omx = 0.0, omy = 0.0;
     double mx, my;

 	 mx = ScaleXInches(x);/*pixels --> inches*/
 	 my = ScaleYInches(y);/*pixels --> inches*/

     switch (event) {
         case BUTTON_DOWN:
   		      if (button == LEFT_BUTTON) {
				isMove = TRUE;/*��ʼ����*/	   
			  } 
              MovePen(mx, my);
		      omx = mx; omy = my;
              break;
    	 case BUTTON_DOUBLECLICK:
			  break;
         case BUTTON_UP:
  		 	  if (button == LEFT_BUTTON) {
  		 	  	isMove = FALSE;	/*��������*/ 
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


