#include <stdlib.h>
#include "DIALOG.h"
#include "SWIPELIST.h"
#include <stdio.h>
#include <stdint.h>
// Choose proper bitmaps depending on color format
//
extern GUI_CONST_STORAGE GUI_BITMAP bmBackgroundWindow_480x272;
extern GUI_CONST_STORAGE GUI_BITMAP bmBackground_480x272;


//
// Swipelist bitmaps
//
#define BM_BACKGROUND_WIN bmBackgroundWindow_480x272
#define BM_BACKGROUND     bmBackground_480x272


typedef enum
{
    SLIDER_NULL = 0,
    SLIDER_UP = 1,
    SLIDER_DOWN = 3,

}SLIDER_TREAT;

typedef struct
{
    U8 slider_tick;
    U8 slider_lock;
    SLIDER_TREAT bSildUpDownSta;
    U8 reg[1];

}TRAET_SLIDER_STRUICT;

TRAET_SLIDER_STRUICT slider_infor;
static	SLIDER_TREAT bSildUpDownSta_old = SLIDER_NULL;


uint8_t u_alarm_apap_type = 0;



/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0  				(GUI_ID_USER + 0x00)
#define ID_TEXT_TIME  			(GUI_ID_USER + 0x01)//标题
#define ID_BUTTON_RAMP 			(GUI_ID_USER + 0x03)//延时升压
#define ID_BUTTON_Humidity  (GUI_ID_USER + 0x04)//加温加湿
#define ID_BUTTON_Arrow  		(GUI_ID_USER + 0x05)//箭头
#define ID_TEXT_Info  					(GUI_ID_USER + 0x0A)//压力，压力范围，血氧 脉率等
#define ID_TEXT_Remind 			(GUI_ID_USER + 0x10)//漏气提示页面	

#define ID_TEXT_Arc 					(GUI_ID_USER + 0x1C)
#define ID_TEXT_BigNum 					(GUI_ID_USER + 0x1D)
//#define ID_TEXT_24			  (GUI_ID_USER + 0x1E)//信号展示
#define ID_TEXT_ThreeGoals 					(GUI_ID_USER + 0x1F)
#define ID_TEXT_Net_PIPE  		(GUI_ID_USER + 0x20)//信号强度加温管路


//*********************颜色****************************

#define ID_TIMER_100ms 0
#define ID_TIMER_xms 1





WM_HWIN hWin_TREAT;
WM_HWIN hWin_Treat_Child1;
WM_HTIMER hTimer_scroll, hTimer_scroll_1;

GUI_MEMDEV_Handle  treat_mem_hadle = 0;


static float pi = 3.1415926f;


static const char _LeftSmallGoalCenter[10][2] =
{
		{ 89, 16},
	  { 87, 15},
		{ 85, 14},
	  { 82, 13},
	  { 79, 12},
	  { 76, 10},
		{ 73, 9},
		{ 70, 8},
		{ 67, 6},
		{ 64, 4},
};

static const char _MidSmallGoalCenter[1][2] =
{
		{101, 16},
};

static const char _RightSmallGoalCenter[10][2] =
{
			{ 113, 16},
			{ 115, 15},
			{ 117, 14},
			{ 120, 13},
			{ 123, 12},
			{ 126, 10},
			{ 129, 9},
			{ 132, 8},
			{ 135, 6},
			{ 138, 4},

};


const char* const Name_Under_display[6][7] =
{
	{"Pmax",		 "Presión máx.",	"Pression Max",		"Pres max",	"Pressão máx",	"Maks. ciś.",			"Maks basınç"},
	{"Prange",		 "Presión gama.",	"Pression Min",		"Prange ",	" Prange",		"Prange",				"Prange"},
	{"Pinit",		 "P inic.",			"Pression Initiale","Pres ini",	"Press in",		"Ciś. p.",				"Başlama basıncı"},
	{"Ptreat",		 "P terap.",		"P traiter",		"Pre tera",	"Pterap",		"Ciś. zab.",			"Tedavi basıncı"},
	{"SpO2(%)",		 "SpO2(%)",			"SpO2(%)",			"SpO2(%)",	"SpO2(%)",		"SpO2(%)",				"SpO2(%)"},
	{"PR(bpm)",		 "PR(bpm)",			"FC(bpm)",			"PR(bpm)",	"PR(bpm)",		"PR(bpm)",				"PR(bpm)"},

};







GUI_TIMER_HANDLE g_TimeTimer = NULL;
static 	uint8_t 	u_ChangePic = 0;

extern uint16_t u_serialcnt; //血氧连续计数

//static  bool  bCirCompFlg=0;//圆环跑完标志 
//bool          bCirUpDownMoving=0;//圆圈上下移动标志
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/*********************************************************************
*
*       _aDialogCreate  资源表
*/
#define RampNUM   4
#define HumdityNUM   5
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
	{ WINDOW_CreateIndirect, "Theary", 	ID_WINDOW_0		,	0,0, 480, 320, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, 	 "",     	ID_TEXT_TIME,  		15,  8, 120, 30, 0, 0x0, 0 },  //日期时间
	{ TEXT_CreateIndirect, 	 "",     	ID_TEXT_Net_PIPE,	140,  8, 322, 30, 0, 0x0, 0 },//信号强度和加温管路
	{ TEXT_CreateIndirect, 	 "",     	ID_TEXT_Remind, 	18,   51,  444, 260,  0, 0x0, 0 }, //提示页面	
	{ BUTTON_CreateIndirect, "", 	 	ID_BUTTON_RAMP, 	15,  116, 110,  96, 0, 0x0, 0 },//2
	{ BUTTON_CreateIndirect, "", 	 	ID_BUTTON_Humidity, 355, 116, 110,  96, 0, 0x0, 0 },//3	
	{ BUTTON_CreateIndirect, "", 	 	ID_BUTTON_Arrow    ,222, 290, 36,  30, 0, 0x0, 0 },//4	
	{ TEXT_CreateIndirect, 	 "",    	ID_TEXT_Info,  		0,	 225, 480, 66, 0, 0x0, 0 },//压力，压力范围，血氧 脉率等
};


//语言  中文 英语  葡萄牙语 波兰语  意大利语 西班牙语  土耳其语
const char* const Lan_map_monitor[][7] =
{
	{"Ramp",					"Rampa",				"Rampe",	 "Rampa",			"Rampa",			"Rampa",					"Rampa"},//0
	{"Humidity",			"Humedad",			"Humidité","Umidità",		"Humidade",		"Wilgot.",				"Nem"},
	{"lv.",						"Nvl.",					"Tx",			 "lv.",				"Nvl.",				"Po.",						"dzy"},	//加热盘
	{"min.",					"min",					"Min",	 	 "Min",				"Min",				"Min.",						"dk."},	//延时升压 
	{"Mode",					"Modo",					"Mode",		 "Modalità",	"Modo",				"Tryb",				 	  "Mod"},	//模式	
	{"Pinit",					"P inic.","Pression Initiale","Pres ini","Press in",	"Ciś. p.",				"Başlama basıncı"},//5
	{"P range",				"Rango P","Gamme de pression","Pres","Pressão",		"Zak. ciś",				"Basınç aralığı"},//6		
	{"SpO2(%)",				"SpO2(%)",			"SpO2(%)",	"SpO2(%)",	"SpO2(%)",		"SpO2(%)",				"SpO2(%)"},
	{"PR(bpm)",				"PR(bpm)",			"FC(bpm)",	"PR(bpm)",	"PR(bpm)",		"PR(bpm)",				"PR(bpm)"},
	{"Auto",					"Auto",					"Auto",			"Auto",			"Auto",				"Auto",						"Oto"},	 //9
	{"Ptreat",				"P terap.",			"P traiter","Pre tera",	"Pr terap",		"Ciś. zab.",			"Tedavi basıncı"},	//10
};


//最上方文本控件显示报警内容   报警提示
const char* const Alarm_State[][7] =
{
	{	"Attention: High leakage",
		"Atención: Fuga elevada",
		"Attention: Fuite importante",
		"Attenzione: Perdita elevata",
		"Atenção: Fuga elevada",
		"Uwaga: Wysoki wyciek",
		"Dikkat: Yüksek sızıntı"},
};

//详细提示信息
const char* const Alarm_Detail[][7] =
{
	{ "Possible causes:",
		"Posibles causas:",
		"Causes possibles:",
		"Possibili cause:",
		"Possíveis causas:",
		"Możliwe przyczyny:",
	  "Olası sebepler"},

};

//**********************************************************************************
	//*************************圆环******************************
//**@autor：
//**@说 明： 
//**@形 参：
//**@返 值：
//**********************************************************************************
//#pragma pack(4)

//三个球自绘 
int RadVal = 0;


static void _cbText_ThreeGoals(WM_MESSAGE* pMsg) {
	GUI_RECT Rect, RectLeft, RectMid, RectRight;
	unsigned char Goal_r = 4;//圆球的半径 

	switch (pMsg->MsgId)
	{
	case WM_PAINT:
	{
		WM_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_BLACK);
		GUI_AA_SetFactor(6);

		GUI_SetColor(GUI_BLUE);

		RectLeft.x0 = Rect.x0 + _LeftSmallGoalCenter[RadVal][0];
		RectLeft.y0 = Rect.y0 + _LeftSmallGoalCenter[RadVal][1];
		GUI_AA_FillCircle(RectLeft.x0, RectLeft.y0, Goal_r);
		RectRight.x0 = Rect.x0 + _RightSmallGoalCenter[RadVal][0];
		RectRight.y0 = Rect.y0 + _RightSmallGoalCenter[RadVal][1];
		GUI_AA_FillCircle(RectRight.x0, RectRight.y0, Goal_r);

		RectMid.x0 = Rect.x0 + _MidSmallGoalCenter[0][0];
		RectMid.y0 = Rect.y0 + _MidSmallGoalCenter[0][1];
		GUI_AA_FillCircle(RectMid.x0, RectMid.y0, Goal_r);

	}
	break;
	default:
		TEXT_Callback(pMsg);
		break;
	}
}






static WM_HWIN WM_hdate;

//展示机器左上角的时间
static void ShowDateTimeTitle(void) {
	char str[50];

	{
		//sprintf(str, "%.2d:%.2d", PARAMETER_HOU, PARAMETER_MIN);
		TEXT_SetText(WM_hdate, str);
	}
}

static  WM_HWIN  WM_BigNum;
uint8_t tick_disply = 0;
float set_press = 0;
static void ShowBigTreatPress(void)
{

	char str[10];
	static float display_press = 0;
	sprintf(str, "%3.1f", set_press);
	if (display_press != set_press)
	{
		TEXT_SetText(WM_BigNum, str);
	}
	else
	{
		tick_disply++;
		if (tick_disply == 20)
		{
			tick_disply = 0;
			TEXT_SetText(WM_BigNum, str);
		}
	}
	display_press = set_press;
}

//**********************************************************************************
	//*************************上滑显示******************************
//**@autor：
//**@说 明： 
//**@形 参：
//**@返 值：
//**********************************************************************************

static void _bottom_infor(WM_MESSAGE* pMsg)
{
	GUI_RECT Rect, Rect1, Rect2, Rect3, Rect4;

	char str[8][20] = { 0 };

	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);
		//******************绘制竖线



		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}
}
//单位压力

static void _cbNetPipe(WM_MESSAGE* pMsg)
{
	GUI_RECT Rect, Rect1;
	char str[50];
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
	
#if 1

		Rect1.x0 = Rect.x1 - 30;
		Rect1.x1 = Rect.x1;
		Rect1.y0 = Rect.y0;
		Rect1.y1 = Rect.y1;


#endif

		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

}

#define PARAMETER_RAMP 0
static void _cbButton_Ramp(WM_MESSAGE* pMsg)
{
	GUI_RECT Rect, Rect1, Rect2;
	char str[10];
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);
		{
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
			GUI_SetColor(GUI_BLUE);
			GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 8);

			GUI_SetTextMode(GUI_TM_TRANS);
		
				if (PARAMETER_RAMP < 10)
				{
					Rect2.x0 = Rect.x0 + 25;
					Rect2.x1 = Rect.x0 + 50;
					Rect2.y0 = Rect.y0 + 5;
					Rect2.y1 = Rect.y0 + 55;
					GUI_SetColor(GUI_WHITE);

					sprintf(str, "%d", PARAMETER_RAMP);
					GUI_DispStringInRect(str, &Rect2, GUI_TA_RIGHT | GUI_TA_BOTTOM);
					Rect2.x0 = Rect.x0 + 53;
					Rect2.x1 = Rect.x1;
					Rect2.y0 = Rect.y0 + 5;
					Rect2.y1 = Rect.y0 + 50;
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringInRect(Lan_map_monitor[3][1], &Rect2, GUI_TA_LEFT | GUI_TA_BOTTOM);

				}
				else
				{

					Rect2.x0 = Rect.x0 + 16;
					Rect2.x1 = Rect.x0 + 55;
					Rect2.y0 = Rect.y0 + 5;
					Rect2.y1 = Rect.y0 + 55;
					GUI_SetColor(GUI_WHITE);

					sprintf(str, "%d", PARAMETER_RAMP);
					GUI_DispStringInRect(str, &Rect2, GUI_TA_HCENTER | GUI_TA_BOTTOM);
					Rect2.x0 = Rect.x0 + 58;
					Rect2.x1 = Rect.x1;
					Rect2.y0 = Rect.y0 + 5;
					Rect2.y1 = Rect.y0 + 50;
					GUI_SetColor(GUI_WHITE);
			
					GUI_DispStringInRect(Lan_map_monitor[3][1], &Rect2, GUI_TA_LEFT | GUI_TA_BOTTOM);

				}
	
			Rect1.x0 = Rect.x0 + 5;
			Rect1.x1 = Rect1.x0 + 100;
			Rect1.y0 = Rect.y0 + 59;
			Rect1.y1 = Rect1.y0 + 32;

			GUI_SetColor(GUI_WHITE);
			GUI_FillRoundedRect(Rect1.x0, Rect1.y0, Rect1.x1, Rect1.y1, 2);

			GUI_SetBkColor(GUI_WHITE);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringInRect(Lan_map_monitor[0][1], &Rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}
}
#define PARAMETER_HUMIDITY 1.0f
static void _cbButton_Humidity(WM_MESSAGE* pMsg)
{

	GUI_RECT Rect, Rect1, Rect2;
	char str[10];
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);
		{
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
			GUI_SetColor(GUI_WHITE);
			GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 8);

			GUI_SetTextMode(GUI_TM_TRANS);
	


				Rect2.x0 = Rect.x0 + 28;
				Rect2.x1 = Rect.x0 + 53;
				Rect2.y0 = Rect.y0 + 5;
				Rect2.y1 = Rect.y0 + 55;
				GUI_SetColor(GUI_WHITE);
				sprintf(str, "%d", PARAMETER_HUMIDITY);
				GUI_DispStringInRect(str, &Rect2, GUI_TA_RIGHT | GUI_TA_BOTTOM);
				Rect2.x0 = Rect.x0 + 56;
				Rect2.x1 = Rect.x1;
				Rect2.y0 = Rect.y0 + 5;
				Rect2.y1 = Rect.y0 + 50;
				GUI_SetColor(GUI_WHITE);
	
				GUI_DispStringInRect(Lan_map_monitor[2][1], &Rect2, GUI_TA_LEFT | GUI_TA_BOTTOM);


			Rect1.x0 = Rect.x0 + 5;
			Rect1.x1 = Rect1.x0 + 100;
			Rect1.y0 = Rect.y0 + 59;
			Rect1.y1 = Rect1.y0 + 32;

			GUI_SetColor(GUI_WHITE);
			GUI_FillRoundedRect(Rect1.x0, Rect1.y0, Rect1.x1, Rect1.y1, 2);

			GUI_SetBkColor(GUI_WHITE);

			GUI_SetColor(GUI_WHITE);
			GUI_DispStringInRect(Lan_map_monitor[1][1], &Rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);

		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}
}



static void _cbButton_arrow(WM_MESSAGE* pMsg)
{
	WM_HWIN hWin;
	GUI_RECT Rect;

	hWin = pMsg->hWin;

	switch (pMsg->MsgId)
	{
	case WM_PAINT:

		WM_GetClientRect(&Rect);
		if (SLIDER_DOWN == slider_infor.bSildUpDownSta)
		{
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
			GUI_SetColor(GUI_WHITE);
			GUI_SetPenSize(4);
			GUI_DrawLine(Rect.x0, Rect.y0 + 15, Rect.x0 + 15, Rect.y0 + 5);
			GUI_DrawLine(Rect.x0 + 15, Rect.y0 + 5, Rect.x0 + 30, Rect.y0 + 15);
		}
		else if ((SLIDER_UP == slider_infor.bSildUpDownSta) || (SLIDER_NULL == slider_infor.bSildUpDownSta))
		{
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
			GUI_SetColor(GUI_WHITE);
			GUI_SetPenSize(4);
			GUI_DrawLine(Rect.x0, Rect.y0 + 5, Rect.x0 + 15, Rect.y0 + 15);
			GUI_DrawLine(Rect.x0 + 15, Rect.y0 + 15, Rect.x0 + 30, Rect.y0 + 5);
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}
}



/*********************************************************************
*
*       _cbDialog
**********************************************************************/
//左边的
TEXT_Handle  Three_Scroll;
TEXT_Handle  TEXT_ARC;


//**********************************************************************************
	//*************************圆环******************************
//**@autor：
//**@说 明： 
//**@形 参：
//**@返 值：
//**********************************************************************************
//#pragma pack(4)
typedef struct
{
	int xCenter;				/* 进度条圆心横坐标 */
	int yCenter;				/* 进度条圆心纵坐标 */
	int BkArc_a0;				/* 背景圆环起始角度 */
	int BkArc_a1;				/* 背景圆环终止角度 */

	float FrontArc_Rad; 		/* 前景圆环的弧度 */

	int FrontArc_a0;			/* 前景圆环起始角度 */
	int FrontArc_a1;			/* 前景圆环终止角度 */
	int EndPoint1_xCenter;		/* 动点圆心横坐标 */
	int EndPoint1_yCenter;		/* 动点圆心纵坐标 */

	int EndPoint_r; 			/* 端点圆半径 */
	int r0; 					/* 进度条半径 */

	char BkArc_PenSize; 		/* 背景圆环宽度 */
	char FrontArc_PenSize;		/* 前景圆环宽度 */
	char reg[2];


}_CircularProgressBarTypeDef;
//#pragma pack()

_CircularProgressBarTypeDef Circular_Progress;

#define ARC 3.1415926f/180//*****************弧度换算单位
float Psetting = 20.0f;
float P_work = 0.0f;
float Pstart = 0.0f;

static void _cbText_Arc(WM_MESSAGE* pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
	{
		WM_GetClientRect(&Rect);
		/* 环形进度条基本参数设置 */
		Circular_Progress.xCenter = Rect.x0 + 101;
		Circular_Progress.yCenter = Rect.y0 + 100;

		if (Circular_Progress.FrontArc_a0 > -45)//圆弧还在填充阶段
		{
			Circular_Progress.BkArc_PenSize = 30;
			Circular_Progress.FrontArc_PenSize = 30;
		}
		if (Psetting != P_work)
		{
			/* 计算定点坐标参数 */

			Circular_Progress.EndPoint_r = Circular_Progress.FrontArc_PenSize / 2;

			/* 设置背景圆环颜色 */
			GUI_SetColor(GUI_GRAY);//灰色圆弧
			/* 设置背景圆环宽度 */
			GUI_SetPenSize(Circular_Progress.BkArc_PenSize);
			/* 绘制背景圆环 */
			GUI_AA_DrawArc(Circular_Progress.xCenter, Circular_Progress.yCenter, Circular_Progress.r0, Circular_Progress.r0, Circular_Progress.BkArc_a0, Circular_Progress.BkArc_a1);

			/* 计算背景首尾坐标 */
			Circular_Progress.FrontArc_Rad = -0.785398126;//(-45) * pi / 180;	//转化为角度				
			Circular_Progress.EndPoint1_xCenter = Circular_Progress.xCenter + Circular_Progress.r0 * cos(Circular_Progress.FrontArc_Rad) + 0.5;//加0.5是干啥 四舍五入吗
			Circular_Progress.EndPoint1_yCenter = Circular_Progress.yCenter - Circular_Progress.r0 * sin(Circular_Progress.FrontArc_Rad) + 0.5;
			/* 绘制背景定点圆 */
			GUI_AA_FillCircle(Circular_Progress.EndPoint1_xCenter, Circular_Progress.EndPoint1_yCenter, Circular_Progress.EndPoint_r);

			//****************************************************起始点绘制				
			/* 计算前景首定点园坐标 */
			Circular_Progress.FrontArc_Rad = 3.9269907;// 3.87463069;//225 * pi / 180;				
			Circular_Progress.EndPoint1_xCenter = Circular_Progress.xCenter + Circular_Progress.r0 * cos(Circular_Progress.FrontArc_Rad) + 0.5;
			Circular_Progress.EndPoint1_yCenter = Circular_Progress.yCenter - Circular_Progress.r0 * sin(Circular_Progress.FrontArc_Rad) + 0.5;

			GUI_SetColor(GUI_BLUE);
			/* 绘制前景定点圆 */
			GUI_AA_FillCircle(Circular_Progress.EndPoint1_xCenter, Circular_Progress.EndPoint1_yCenter, Circular_Progress.EndPoint_r);


			/* 角度转弧度 */
			Circular_Progress.FrontArc_Rad = Circular_Progress.FrontArc_a0 * ARC;
			/* 计算动点坐标 */
			Circular_Progress.EndPoint1_xCenter = Circular_Progress.xCenter + Circular_Progress.r0 * cos(Circular_Progress.FrontArc_Rad) + 0.5;
			Circular_Progress.EndPoint1_yCenter = Circular_Progress.yCenter - Circular_Progress.r0 * sin(Circular_Progress.FrontArc_Rad) + 0.5;


			Circular_Progress.FrontArc_a0 = 225 - (P_work - Pstart) / (Psetting - Pstart) * 270;//进度条角度
			Circular_Progress.FrontArc_a0 = Circular_Progress.FrontArc_a0 < -45 ? -45 : Circular_Progress.FrontArc_a0;

			//****************************************************进度条设置
			GUI_SetColor(GUI_BLUE);
			/* 设置前景圆环宽度 */
			GUI_SetPenSize(Circular_Progress.FrontArc_PenSize);
			/* 绘制前景圆环 */
			GUI_AA_DrawArc(Circular_Progress.xCenter, Circular_Progress.yCenter, Circular_Progress.r0, Circular_Progress.r0, Circular_Progress.FrontArc_a0, Circular_Progress.FrontArc_a1);
			/* 绘制动点圆 */
			GUI_AA_FillCircle(Circular_Progress.EndPoint1_xCenter, Circular_Progress.EndPoint1_yCenter, Circular_Progress.EndPoint_r);
		}
		else if (Psetting == P_work)//*********圆环结束时直接填充定值
		{
			//*********************此部分直接填充固定值减少运算不建议修改
			/* 计算前景首定点园坐标 */
			GUI_SetColor(GUI_BLUE);
			/* 绘制前景定点圆 */
			GUI_AA_FillCircle(42, 159, 15);//**
			//****************************************************起始点绘制				
			GUI_SetPenSize(Circular_Progress.FrontArc_PenSize);
			GUI_AA_DrawArc(101, 100, 83, 83, -45, Circular_Progress.FrontArc_a1);

			GUI_AA_FillCircle(160, 159, 15);
		}

	}
	break;
	default:
		TEXT_Callback(pMsg);
		break;
	}
}

static void _cbDialog_treat(WM_MESSAGE* pMsg) {
	WM_HWIN hItem;
	int NCode;
	int Id;
	static uint8_t Treat_delay;
	static uint8_t leftDone = 0;
	switch (pMsg->MsgId)
	{

		//********空间区域***********
	case WM_INIT_DIALOG: //初始化....
	{
		GUI_UC_SetEncodeUTF8();
		hItem = pMsg->hWin;
		WINDOW_SetBkColor(hItem, GUI_BLACK);
		//日期时间
		WM_hdate = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TIME);
		TEXT_SetBkColor(WM_hdate, GUI_BLACK);
		TEXT_SetTextAlign(WM_hdate, TEXT_CF_LEFT | TEXT_CF_BOTTOM);
		TEXT_SetTextColor(WM_hdate, GUI_WHITE);	//时间展示放在主循环中	


		//*******************************************************创建延时升压并且隐藏

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RAMP);
		WM_SetCallback(hItem, _cbButton_Ramp);
		WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RAMP));

		//*******************************************************创建加温加湿并且隐藏
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Humidity);
		WM_SetCallback(hItem, _cbButton_Humidity);
		WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Humidity));

		//*******************************************************创建加温管路和网络信号的图标
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_Net_PIPE);
		WM_SetCallback(hItem, _cbNetPipe);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Arrow);
		WM_SetCallback(hItem, _cbButton_arrow);
		//*******************************************************创建最下面的四个框及三条竖线
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_Info);
		WM_SetCallback(hItem, _bottom_infor);
		//WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_Info));
	}
	break;

	break;
	case WM_NOTIFY_PARENT:
	{
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_RAMP:
		{
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:

				break;
			}
		}
		break;
		case ID_BUTTON_Humidity:
		{
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
	
				break;
			}
		}
		break;
		case ID_BUTTON_Arrow:
		{
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				if (slider_infor.slider_lock == 1)
				{
					if ((SLIDER_UP == slider_infor.bSildUpDownSta) && (bSildUpDownSta_old == slider_infor.bSildUpDownSta))
					{
						slider_infor.bSildUpDownSta = SLIDER_DOWN;
						slider_infor.slider_lock = 0;
					}
					else if (bSildUpDownSta_old == slider_infor.bSildUpDownSta)
					{
						slider_infor.bSildUpDownSta = SLIDER_UP;
						slider_infor.slider_lock = 0;
					}

				}

				break;
			}
		}
		break;
		}
	}
	break;
	case WM_TIMER:
		Id = WM_GetTimerId(pMsg->Data.v); // The handle of the timer that expired is sent via the data value of the message.
		switch (Id)
		{
		case ID_TIMER_100ms:
			if ((RadVal <= 9) && (!leftDone))
			{
				if (RadVal < 9)
				{
					RadVal++;
				}
				else
				{
					leftDone = 1;
				}
			}
			else
			{
				if ((RadVal > 0) && leftDone)
				{
					RadVal--;
				}
				else
				{
					leftDone = 0;
				}
			}
			WM_SetCallback(WM_GetDialogItem(hWin_Treat_Child1, ID_TEXT_Arc), _cbText_Arc);
			WM_SetCallback(WM_GetDialogItem(hWin_Treat_Child1, ID_TEXT_ThreeGoals), _cbText_ThreeGoals);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Arrow);
			WM_SetCallback(hItem, _cbButton_arrow);

			if (slider_infor.slider_tick == 0)
			{
				Treat_delay++;
				if (!(Treat_delay % 10))
				{
					WM_SetCallback(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Net_PIPE), _cbNetPipe);
				}
				else if (!(Treat_delay % 15))
				{
					WM_SetCallback(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Info), _bottom_infor);
				}
				else if (Treat_delay >= 200)
				{
					ShowDateTimeTitle();//显示时间
					Treat_delay = 0;
				}
			}
			ShowBigTreatPress();//显示压力大字体 
			WM_RestartTimer(hTimer_scroll, 100);

			break;
		case ID_TIMER_xms:

			WM_RestartTimer(hTimer_scroll_1, 100);
			break;
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
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
*       CreateSET
*/


WM_HWIN CreateTREAT(void);
WM_HWIN CreateTREAT(void) {
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog_treat, WM_HBKWIN, 0, 0);
	return hWin;
}


static void _cbDummy_treat(WM_MESSAGE* pMsg)
{
	int  Id;

	WM_HWIN hWin;
	GUI_RECT Rect, Rect1;
	hWin = pMsg->hWin;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
	{
		WM_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_BLACK);
		GUI_Clear();
		GUI_SetColor(GUI_WHITE);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		Rect1.x0 = Rect.x0 + 64;
		Rect1.x1 = Rect.x0 + 64 + 74;
		Rect1.y0 = Rect.y0 + 47;
		Rect1.y1 = Rect.y0 + 47 + 18;
		GUI_DispStringInRect("CPAP", &Rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);
		GUI_SetColor(GUI_WHITE);
		Rect1.x0 = Rect.x0 + 64;
		Rect1.x1 = Rect.x0 + 64 + 74;
		Rect1.y0 = Rect.y0 + 141;
		Rect1.y1 = Rect.y0 + 141 + 18;
		GUI_DispStringInRect("cmH2O", &Rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);
	}
	break;

	case WM_CREATE:
	{

		//大数据 数据展示在主循环
		WM_BigNum = TEXT_CreateEx(50, 67, 100, 70, hWin, WM_CF_SHOW, TEXT_CF_HCENTER | TEXT_CF_VCENTER, ID_TEXT_BigNum, "");
		TEXT_SetBkColor(WM_BigNum, GUI_BLACK);
		TEXT_SetTextColor(WM_BigNum, GUI_WHITE);
		TEXT_SetTextAlign(WM_BigNum, GUI_TA_HCENTER | GUI_TA_VCENTER);



		TEXT_ARC = TEXT_CreateEx(0, 0, 203, 180, hWin, WM_CF_SHOW, TEXT_CF_HCENTER | TEXT_CF_VCENTER, ID_TEXT_Arc, "");
		WM_SetCallback(TEXT_ARC, _cbText_Arc);

		Three_Scroll = TEXT_CreateEx(0, 180, 203, 20, hWin, WM_CF_SHOW, TEXT_CF_HCENTER | TEXT_CF_VCENTER, ID_TEXT_ThreeGoals, "");
		WM_SetCallback(Three_Scroll, _cbText_ThreeGoals);

	}
	break;


	default:
		WM_DefaultProc(pMsg);
		break;
	}
}





//动态数据
static void DynData_Treat(void)
{

	if ((u_alarm_apap_type) && (slider_infor.slider_lock == 1))
	{


		WM_ShowWin(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Remind));
		WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_RAMP));
		WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_Humidity));
		WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Info));
		WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_Arrow));
		WM_HideWin(hWin_Treat_Child1);
		bSildUpDownSta_old = SLIDER_NULL;
	}
	else
	{
		if (bSildUpDownSta_old != slider_infor.bSildUpDownSta)
		{
			if (slider_infor.slider_lock == 1)
			{

				if (SLIDER_DOWN != slider_infor.bSildUpDownSta)
				{
					WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_RAMP));
					WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_Humidity));
					WM_ShowWin(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Info));
				}
				else if (SLIDER_DOWN == slider_infor.bSildUpDownSta)
				{
					WM_ShowWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_RAMP));
					WM_ShowWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_Humidity));
					WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Info));
				}
				bSildUpDownSta_old = slider_infor.bSildUpDownSta;

			}
			else if (slider_infor.slider_lock == 0)
			{
				WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_RAMP));
				WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_Humidity));
				WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Info));

			}

			WM_ShowWin(WM_GetDialogItem(hWin_TREAT, ID_BUTTON_Arrow));
			WM_HideWin(WM_GetDialogItem(hWin_TREAT, ID_TEXT_Remind));
			WM_ShowWin(hWin_Treat_Child1);

		}
	}
}



uint8_t first_in = 0;
uint16_t tou_point = 0;
uint8_t remove_over = 1;

//主页面参数Rst
static void Param_Reset(void) {

	Circular_Progress.FrontArc_a0 = 225;
	Circular_Progress.FrontArc_a1 = 225;
	Circular_Progress.r0 = 83;
	Circular_Progress.BkArc_a0 = -45;
	Circular_Progress.BkArc_a1 = 225;
	slider_infor.bSildUpDownSta = SLIDER_DOWN;
	slider_infor.slider_lock = 1;
	slider_infor.slider_tick = 0;
	bSildUpDownSta_old = SLIDER_NULL;
	tick_disply = 18;
	ShowDateTimeTitle();
}





/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  WM_HWIN _hSwipelist;
  WM_HWIN hWin;
  int     xSize;
  int     ySize;
  int     xPos;
  int     yPos;
  int     i;

#ifdef WIN32
  WM_SetCreateFlags(WM_CF_MEMDEV);
#endif
  WM_MOTION_Enable(1);
  WM_MULTIBUF_Enable(1);
  GUI_Init();

  //
  char  LO_ERROR_FLAG = 0;


  hWin_TREAT = CreateTREAT();
  hWin_Treat_Child1 = WM_CreateWindowAsChild(139, 54, 203, 200, hWin_TREAT, WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW, _cbDummy_treat, 0);//中间的圆圈

  hTimer_scroll = WM_CreateTimer(hWin_TREAT, ID_TIMER_100ms, 120, 0);
  hTimer_scroll_1 = WM_CreateTimer(hWin_TREAT, ID_TIMER_xms, 10, 0);
  Param_Reset();
  //*********************防止界面进入闪烁

  while (1) 
  {
	  DynData_Treat();
    GUI_Exec();
    GUI_Delay(50);
  }
}
