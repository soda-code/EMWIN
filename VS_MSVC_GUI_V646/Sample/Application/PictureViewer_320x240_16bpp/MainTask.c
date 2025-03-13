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
File        : MainTask.c
---------------------------END-OF-HEADER------------------------------
*/

#include "DIALOG.h"
#include "Resource.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define NUM_PICTURES         GUI_COUNTOF(_apbmPictures)
#define WAIT_TOUCH_PERIOD    3000
#define ANIM_PERIOD          1000
//
// Macros
//
#define START_TIMER()        _hTimer = GUI_TIMER_Create(_cbTimerAnim, 0, 0, 0);  \
                             GUI_TIMER_SetPeriod(_hTimer, WAIT_TOUCH_PERIOD);    \
                             GUI_TIMER_Restart(_hTimer)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static WM_HWIN          _hWin;
static GUI_TIMER_HANDLE _hTimer;
static GUI_ANIM_HANDLE  _hAnim;
static ANIM_DATA        _Data;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _apbmPictures: Pictures
*/
static const GUI_BITMAP * _apbmPictures[] = {
  &bmpic0,
  &bmpic1,
  &bmpic2,
  &bmpic3,
  &bmpic4,
  &bmpic5,
  &bmpic6,
  &bmpic7,
  &bmpic8,
  &bmpic9,
  &bmpic10,
  &bmpic11,
  &bmpic12,
  &bmpic13,
  &bmpic14,
  &bmpic15,
};

/*********************************************************************
*
*       _DeleteTimer
*/
static void _DeleteTimer(void) {
  if (_hTimer) {
    GUI_TIMER_Delete(_hTimer);
    _hTimer = 0;
  }
}

/*********************************************************************
*
*       _AnimMove
*/
static void _AnimMove(GUI_ANIM_INFO * pInfo, void * pVoid) {
  int xPos;

  GUI_USE_PARA(pVoid);
  xPos = _Data.xStart + (((_Data.xDest - _Data.xStart) * pInfo->Pos) / GUI_ANIM_RANGE);
  WM_MoveTo(_hWin, xPos, 0);
}

/*********************************************************************
*
*       _OnAnimEnd
*/
static void _OnAnimEnd(void * pVoid) {
  GUI_USE_PARA(pVoid);
  //
  // Start timer on animation end
  //
  _hAnim = 0;
  START_TIMER();
}

/*********************************************************************
*
*       _cbTimerAnim
*/
static void _cbTimerAnim(GUI_TIMER_MESSAGE * pTM) {
  GUI_USE_PARA(pTM);
  //
  // Set up animation data
  //
  _Data.xStart = WM_GetWindowOrgX(_hWin);
  if (_Data.xStart == 0) {
    _Data.Dir = 1;
  }
  if (_Data.xStart == -((int)(GUI_COUNTOF(_apbmPictures) - 1) * LCD_GetXSize())) {
    _Data.Dir = -1;
  }
  if (_Data.Dir > 0) {
    _Data.xDest = _Data.xStart - LCD_GetXSize();
  } else {
    _Data.xDest = _Data.xStart + LCD_GetXSize();
  }
  //
  // Create animation and start it
  //
  _hAnim = GUI_ANIM_Create(ANIM_PERIOD, 25, NULL, NULL);
  GUI_ANIM_AddItem(_hAnim, 0, ANIM_PERIOD, ANIM_ACCELDECEL, NULL, _AnimMove);
  GUI_ANIM_StartEx(_hAnim, 1, _OnAnimEnd);
}

/*********************************************************************
*
*       _cbWin
*/
static void _cbWin(WM_MESSAGE * pMsg) {
  GUI_RECT              Rect;
  WM_MOTION_INFO      * pInfo;
  unsigned              i;
  int                   xDist;
  int                   xPos;
  const GUI_PID_STATE * pState;

  switch (pMsg->MsgId) {
  case WM_CREATE:
    //
    // Start animation timer
    //
    START_TIMER();
    break;
  case WM_MOTION:
    pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
    switch (pInfo->Cmd) {
    case WM_MOTION_INIT:
      WM_GetClientRectEx(pMsg->hWin, &Rect);
      //
      // Snap window at each quarter of the entire window
      // Therefore we get several 'pages', each as big as the screen
      //
      pInfo->SnapX = (Rect.x1 + 1) / NUM_PICTURES;
      break;
    }
    break;
  case WM_PAINT:
    WM_GetClientRect(&Rect);
    xDist = (Rect.x1 + 1) / NUM_PICTURES;

    for (i = 0; i < NUM_PICTURES; i++) {
      xPos = i * xDist;
      GUI_DrawBitmap(*(_apbmPictures + i), xPos, 0);
    }
    break;
  case WM_TOUCH:
    pState = (const GUI_PID_STATE *)pMsg->Data.p;
    if (pState) {
      //
      // If pressed stop animation and timer
      // 
      if (pState->Pressed == 1) {
        _DeleteTimer();
        if (_hAnim) {
          GUI_ANIM_Stop(_hAnim);
          GUI_ANIM_Delete(_hAnim);
          _hAnim = 0;
        }
      }
      //
      // Start timer when released
      //
      if (pState->Pressed == 0) {
        START_TIMER();
      }
    } else {
      START_TIMER();
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  int xSize;
  int ySize;

  //
  // Start GUI
  //
  GUI_Init();
  //
  // Enable motion support
  //
  WM_MOTION_Enable(1);
  //
  // Enable multi-buffering to avoid flickering.
  //
  WM_MULTIBUF_Enable(1);
  //
  // Set size of Window Manager
  // This is necessary since this sample does not include an actual parent window
  // By default the background window has a size of 16383 x 16383
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  WM_SetSize(WM_HBKWIN, xSize, ySize);
  //
  // Create a window
  // The flag WM_CF_MOTION_X tells the window that it can be moved via the X-axis.
  //
  _hWin = WM_CreateWindowAsChild(0, 0, LCD_GetXSize() * NUM_PICTURES, LCD_GetYSize(), WM_HBKWIN, WM_CF_SHOW | WM_CF_MOTION_X, _cbWin, 0);

  while (1) {
    GUI_Delay(100);
  }
}

/*************************** End of file ****************************/
