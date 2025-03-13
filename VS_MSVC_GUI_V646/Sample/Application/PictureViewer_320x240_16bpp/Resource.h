/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.46 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : Resource.h
---------------------------END-OF-HEADER------------------------------
*/

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmpic0;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic1;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic2;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic3;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic4;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic5;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic6;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic7;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic8;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic9;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic10;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic11;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic12;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic13;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic14;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic15;

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  int xStart;
  int xDest;
  I8  Dir;
} ANIM_DATA;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
static void _cbTimerAnim(GUI_TIMER_MESSAGE * pTM);


/*************************** End of file ****************************/
