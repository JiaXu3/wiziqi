#include "game.h"
#include "touch.h"
#include "led.h"
#include <string.h>
#include "text.h"
#include "delay.h"
#include "rng.h"
#include "usart.h"
#include "exit.h"
#include "beep.h"



//定义棋盘起始坐标
#define CHESSBOARD_START_X		7
#define CHESSBOARD_START_Y		42
#define CHESSBOARD_END_X		450
#define CHESSBOARD_END_Y		500

//棋盘大小
#define CHESSBOARD_SIZE			(CHESSBOARD_END_X-CHESSBOARD_START_X)
//网格大小
#define CHESSGRID_SIZE			(CHESSBOARD_SIZE / 15)
//棋盘颜色
#define CHESSBOARD_COLOR		BROWN

char g_Chess[16][16];		//存放棋子缓存，对应的X,Y坐标上的第几颗棋子
int  g_ChessNum;			//棋子数量
int  g_SameChessNum;		//相连的相同的棋子数
int  g_SpaceFlag = 0;		//空位标志  1为空 0为不空
char g_Mode=0;				//五子棋模式   0为人机对战1为双人对战
u8 voice=0;
u8 flag1=0;
u32 timeCount=10;
int nm;
//绘制棋盘
//15x15
static void DrawChessBoard(void)
{
	int i;
	//填充棋盘背景色
	LCD_Fill(CHESSBOARD_START_X-5, CHESSBOARD_START_Y-5, 
			CHESSBOARD_END_X+5, CHESSBOARD_END_Y+5, CHESSBOARD_COLOR);
	POINT_COLOR = BLACK;
	//绘制边框线
	LCD_DrawRectangle(CHESSBOARD_START_X-3, CHESSBOARD_START_Y-3,
					  CHESSBOARD_END_X+3, CHESSBOARD_END_Y+3);
	LCD_DrawRectangle(CHESSBOARD_START_X-4, CHESSBOARD_START_Y-4,
					  CHESSBOARD_END_X+4, CHESSBOARD_END_Y+4);
	LCD_DrawRectangle(CHESSBOARD_START_X, CHESSBOARD_START_Y, 
					  CHESSBOARD_END_X, CHESSBOARD_END_Y);
	
	POINT_COLOR = BLACK;
	//绘制垂直线
	for (i = 1; i < 16; i++)
	{
		LCD_DrawLine(CHESSBOARD_START_X,
					 CHESSBOARD_START_Y + (CHESSGRID_SIZE*i),
					 CHESSBOARD_END_X,
					 CHESSBOARD_START_Y + (CHESSGRID_SIZE*i));
	}
	
	//绘制水平线
	for (i = 1; i < 16; i++)
	{
		LCD_DrawLine(CHESSBOARD_START_X + (CHESSGRID_SIZE*i),
					 CHESSBOARD_START_Y,
					 CHESSBOARD_START_X + (CHESSGRID_SIZE*i),
					 CHESSBOARD_END_Y);
	}
}

//绘制棋子
//x,y:棋子坐标(0~15)
static void DrawChess(int x, int y, int num, char sta)
{
	uint16_t ChessColor;
	uint16_t xChess;
	uint16_t yChess;
	
	if (x>15 || x<0 || y>15 || y<0)return;	//超范围了
	//判断是下棋还是复盘
	if (sta)
	{
		if (g_Chess[x][y]!=0)return;			//已有棋子
	}
	
	//判断棋子颜色
	(num%2==0) ? (ChessColor=BLACK) : (ChessColor=WHITE);
	
	//计算棋子坐标
	xChess = CHESSBOARD_START_X + (x*CHESSGRID_SIZE);
	yChess = CHESSBOARD_START_Y + (y*CHESSGRID_SIZE);
	
	LcdDrawCircleA(xChess, 
				   yChess, 
				   CHESSGRID_SIZE/2-1, 
				   ChessColor, 
				   1);
	if(voice==1)          //判断声音是否打开 若打开 每次下子蜂鸣器响100ms
		BEEP_Run(100);
	delay_ms(10);
	if (sta == 0) return;
	g_ChessNum++;
	g_Chess[x][y] = g_ChessNum;
}

//清除棋子
//x,y:棋子坐标(0~14)
static void ClearChess(int x, int y)
{
	uint16_t ChessColor;
	uint16_t xChess;
	uint16_t yChess;
	
	if (x>14 || x<0 || y>14 || y<0)return;	//超范围了
	if (g_Chess[x][y]!=0)return;			//已有棋子
	
	ChessColor = CHESSBOARD_COLOR;
	
	//计算棋子坐标
	xChess = CHESSBOARD_START_X+ (x*CHESSGRID_SIZE);
	yChess = CHESSBOARD_START_Y + (y*CHESSGRID_SIZE);
	
	LcdDrawCircleA(xChess, 
				   yChess, 
				   CHESSGRID_SIZE/2-1, 
				   ChessColor, 
				   1);
	POINT_COLOR=BLACK;
	LCD_DrawLine(xChess-8,yChess,xChess+8,yChess);
	LCD_DrawLine(xChess,yChess-8,xChess,yChess+8);
	POINT_COLOR=WHITE;
}

//获取相同棋子数（一次只能判断）
//x,y:棋子坐标(0~14)
//xStep,yStep:x,y坐标移动步数即数值为-1,0,1
static int GetChessNum(char ChessType, int x, int y, int xStep, int yStep)//参数点x，y为当前点，查下一点是否为同点
{	
	g_SameChessNum = 0;
	while (1)
	{
		x = x + xStep;
		y = y + yStep;//取下一点
		
		//判断是否越界
		if (x < 0 || x > 14 || y < 0 || y > 14)
		{
			g_SpaceFlag = 0;
			return g_SameChessNum;
		}
		
		//没有棋子，返回
		if (g_Chess[x][y]==0)
		{
			g_SpaceFlag = 1;
			return g_SameChessNum;
		}
		
		//计算空点（x，y）周围八个点相同的棋子
		if (g_Chess[x][y] % 2 == ChessType)//判断当前X，y点上的棋子类型是否符合当前的要查找的棋子的类型
		{
			if ((xStep == 1 && yStep == 0) || (xStep == -1 && yStep == 0))//左右
			{
				g_SameChessNum++;
			}
			else if ((xStep == 0 && yStep == 1) || (xStep == 0 && yStep == -1))//上下
			{
				g_SameChessNum++;
			}
			else if ((xStep == 1 && yStep == 1) || (xStep == -1 && yStep == -1))//右对角线
			{
				g_SameChessNum++;
			}
			else if ((xStep == 1 && yStep == -1) || (xStep == -1 && yStep == 1))//左对角线
			{
				g_SameChessNum++;
			}
		}
		else
		{
			g_SpaceFlag = 0;
			return g_SameChessNum;
		}
	}
}

/*=====================================================================*/
//							 智能电脑AI下子相关计算函数                      //
/*=====================================================================*/
//查找棋线状态
//x, y:坐标
//xStep, yStep:移动步数
static int Search(int color, int x, int y, int xStep, int yStep)
{
	int count = 1;
	int alive1 = 0, alive2 = 0;
	
	count += GetChessNum(color, x, y, xStep, yStep);//判断下一个棋子与当前棋子是否同棋子
	if (g_SpaceFlag) alive1 = 1;//下一棋子为空
	count += GetChessNum(color, x, y, 0-xStep, 0-yStep);//取对线的点判断是否为同棋子
	if (g_SpaceFlag) alive2 = 1;//下一棋子为空
	
	if (count>=5) return 5;
	else if (alive1&&alive2) return count;//count=2
	else if ((alive1||alive2)&&count!=1) return count+4;//count=2或3
	else return 0;
}

//获取分数
//i,j:坐标
//color:0白色  1黑色
//返回: 0//（x，y）两个是不同色
//		2,活二//两个是空
//		3,活三
//		4,活四
//		5,五
//		6,死二//一个是空 一个同色
//		7,死三
//		8,死四
static unsigned int GetScore(int i, int j, int color)
{
	int deadfour=0, deadthree=0, deadtwo=0;
	int five=0, alivefour=0, alivethree=0, alivetwo=0;
	int status[4];
	uint16_t score;
	int n;
	
//	(color==0) ? (g_ChessNum++) : (g_ChessNum+=2);
//	g_Chess[i][j] = g_ChessNum;
	status[0] = Search(color, i, j, 0, 1);  //上下
	status[1] = Search(color, i, j, 1, 0);   //左右
	status[2] = Search(color, i, j, 1, 1);   //右上对角线
	status[3] = Search(color, i, j, 1, -1);   //右下对角线
//	(color==0) ? (g_ChessNum--) : (g_ChessNum-=2);
//	g_Chess[i][j] = 0;
	
	//统计各种情况的数目
	for (n = 0; n < 4; n++)
	{
		switch (status[n])
		{
			case 8:
				deadfour++;
				break;
			case 7:
				deadthree++;
				break;
			case 6:
				deadtwo++;
				break;
			case 5:
				five = 1;
				break;
			case 4:
				alivefour = 1;
				break;
			case 3:
				alivethree++;
				break;
			case 2:
				alivetwo++;
				break;
			default:
				break;
		}
	}
	
	if (five) score = 20000;
	else if (alivefour) score = 15000;
	else if (deadfour>=2) score = 15000;
	else if (deadfour&&alivethree) score = 15000;
	else if (alivethree>=2) score = 15000;
	else if (alivefour&&(alivetwo>=2)) score = 5000;
	else if (alivethree&&deadthree) score = 1000;
	else if (1==deadfour) score = 500;
	else if (1==alivethree) score = 200;
	else if (alivetwo>=2) score = 100;
	else if (deadthree>=2) score = 50;
	else if (1==alivetwo) score = 10;
	else if (1==deadthree) score = 5;
	else score = 1;
	
	return score;
}

//查找最佳下子位置
static int SearchValue(int *best_i, int *best_j, int color)
{
	char x, y;
	uint16_t  score[15][15];
	uint16_t  MaxScore = 0;
	
	memset(score, 0, sizeof(score));
	//白棋
	if (0 == color)
	{
		//计算整个棋盘  
		for (x = 0; x < 15; x++)
		{
			for (y = 0; y < 15; y++)
			{
				if (g_Chess[x][y] != 0) continue;  //找没有下子的地方
				score[x][y] = GetScore(x, y, color);
			}
		}
		
		//遍历寻找最佳下子坐标
		for (x = 0; x < 15; x++)
		{
			for (y = 0; y < 15; y++)
			{
				if (score[x][y] > MaxScore)
				{
					MaxScore = score[x][y];
					*best_i = x;
					*best_j = y;
				}
				else if (score[x][y] == MaxScore)
				{
					char randrom = RNG_Get_RandomRange(0, 9);
					if (randrom%2)
					{
						MaxScore = score[x][y];
						*best_i = x;
						*best_j = y;
					}
				}
			}
		}
		return MaxScore;
	}
	//黑棋
	if (1 == color)
	{
		//计算整个棋盘
		for (x = 0; x < 15; x++)
		{
			for (y = 0; y < 15; y++)
			{
				if (g_Chess[x][y] != 0) continue;
				score[x][y] = GetScore(x, y, color);
			}
		}
		
		//遍历寻找最佳下子坐标
		for (x = 0; x < 15; x++)
		{
			for (y = 0; y < 15; y++)
			{
				if ((score[x][y]) > MaxScore)
				{
					MaxScore = score[x][y];
					*best_i = x;
					*best_j = y;
				}
				else if (score[x][y] == MaxScore)
				{
					char randrom = RNG_Get_RandomRange(0, 9);
					if (randrom&2)
					{
						MaxScore = score[x][y];
						*best_i = x;
						*best_j = y;
					}
				}
			}
		}
		return MaxScore;
	}
	
	return 0;
}

//获取AI下子坐标
//xChess,yChess:棋子坐标
//判断棋盘当前的局势
static void GetAIPoint(int *xChess, int *yChess)  //把计算出来的坐标放任参数中
{
	unsigned int iWhiteValue, iBlackValue;
	int wi, wj, bi, bj;
	
	//得到分数和有利位置
	iWhiteValue = SearchValue(&wi, &wj, 1);//得到下黑棋最好位置
	iBlackValue = SearchValue(&bi, &bj, 0);//得到下白旗的最好位置
	
	//白棋有利,电脑进攻
	if (iWhiteValue >= iBlackValue)
	{
		*xChess = wi;
		*yChess = wj;
		printf("\r\nx:%d  y:%d\r\n",wi,wj);
	}
	else if (iWhiteValue < iBlackValue)	//黑棋有利,电脑防守
	{
		*xChess = bi;
		*yChess = bj;
		printf("\r\nx:%d  y:%d\r\n",bi,bj);
	}
}


/*=====================================================================*/
//						电脑下子相关计算函数结束                      //
/*=====================================================================*/

//判断是否五子相连
//x,y:棋子坐标(0~14)
static char IsFiveChessContant(int x, int y)
{
	int ChessType = 0;
	int ChessNum = 0;

	ChessType = g_Chess[x][y] % 2;
	
	//横向判断
	ChessNum = GetChessNum(ChessType, x, y, -1, 0) + GetChessNum(ChessType, x, y, 1, 0) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	//纵向判断
	ChessNum = GetChessNum(ChessType, x, y, 0, 1) + GetChessNum(ChessType, x, y, 0, -1) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	//斜向判断
	ChessNum = GetChessNum(ChessType, x, y, -1, 1) + GetChessNum(ChessType, x, y, 1, -1) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	//反斜向判断
	ChessNum = GetChessNum(ChessType, x, y, 1, 1) + GetChessNum(ChessType, x, y, -1, -1) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	return 0;
}

//游戏结束
//获胜结束
//平局结束
//x,y:棋子坐标(0~14)

static uint8_t IsGameOver(uint8_t x, uint8_t y)
{
	//五子相连
	if (IsFiveChessContant(x, y)) return 2;
	
	//平局结束
	if (g_ChessNum == 225) return 1;
	
	return 0;
}

//悔棋
static void RetractChess(void)
{
	int x, y;
	int xChess, yChess;
	
	//如果没有棋子则退出
	if (g_ChessNum == 0) return;
	
	//查询最后一个棋子并记录
	for (x = 0; x < 15; x++)
	{
		for (y = 0; y < 15; y++)
		{
			if (g_Chess[x][y] == g_ChessNum)
			{
				xChess = x;
				yChess = y;
				break;
			}
		}
	}
	
	//清除最后一个棋子
	g_Chess[xChess][yChess] = 0;
	g_ChessNum -= 1;
	ClearChess(xChess, yChess);
}

//重新开始
static void ResStart(void)
{
	g_ChessNum = 0;	//棋子数量
	//清空棋子数据
	memset(g_Chess, 0, sizeof(g_Chess));
	
	//绘制棋盘
	DrawChessBoard();
}

//绘制按钮
//x,y:坐标
//width,height:宽度,高度
//*Text:文本
//sta:按钮状态(1弹起 0按下)
static void DrawButton(int x, int y, int width, int height, u8* Text, char sta)
{
	int iLen = strlen((char*)Text);
	
	if (x > 478 || x < 0 || y < 0 || y > 638) return;
	
	POINT_COLOR = WHITE;
	LCD_DrawRectangle(x, y, x+width, y+height);
	if (sta) 
	{
		LCD_Fill(x+1, y+1, x+width-1, y+height-1, BLUE);
		BACK_COLOR = RED;
//		LCD_ShowString(x+((width-iLen*8)/2), y+((height-16)/2), 200, 16, 16, Text);
		Show_Str(x+((width-iLen*8)/2), y+((height-16)/2), 200, 16, Text, 16, 0);
	}
	else 
	{
		LCD_Fill(x+1, y+1, x+width-1, y+height-1, GRAY);
		BACK_COLOR = GRAY;
//		LCD_ShowString(x+((width-iLen*8)/2)+2, y+((height-16)/2)+2, 200, 16, 16, Text);
		Show_Str(x+((width-iLen*8)/2)+2, y+((height-16)/2)+2, 200, 16, Text, 16, 0);
	}
}

//关于对话框界面
static char About(void)
{
	static char ckey = 0;
	
	POINT_COLOR = WHITE;
	//绘制关于窗口
	LCD_DrawRectangle(20, 110, 220, 220);
	LCD_DrawRectangle(21, 111, 219, 219);
	LCD_Fill(22, 112, 218, 218, BLACK);
	BACK_COLOR = BLACK;
	//文本
	Show_Str(104, 115, 32, 16, (u8*)"关于", 16, 0);
	Show_Str(40, 135, 136, 16, (u8*)"平台:STM32探索者开发板", 16, 0);
	Show_Str(40, 155, 136, 16, (u8*)"作者:李俊潮", 16, 0);
	Show_Str(40, 175, 136, 16, (u8*)"游戏名:欢乐五子棋", 16, 0);
	//绘制按钮
	DrawButton(90, 195, 60, 20, (u8*)"退出", 1);
	
	while (1)
	{
		//判断触摸屏是否被按下
		if (tp_dev.sta&TP_PRES_DOWN)
		{
			int x = tp_dev.x[0];
			int y = tp_dev.y[0];
			
			if (x > 90 && x < 150 && y > 195 && y < 215)
			{
				if (ckey == 0)
				{
					ckey = 1;
					DrawButton(90, 195, 60, 20, (u8*)"退出", 0);
				}	
			}
		}
		else if (ckey)
		{
			DrawButton(90, 195, 60, 20, (u8*)"退出", 1);
			ckey = 0;
			g_Mode = 0;
			return 1;
		}
	}
}




//界面初始化
static char UIStart(void)
{
	LCD_Clear(GBLUE);
	BACK_COLOR = GBLUE;
	POINT_COLOR = BLACK;
	Show_Str((240 - 24*3)-30, 30, 200, 48, (u8*)"欢乐五子棋", 24, 0);
	
	//创建按钮
	DrawButton(120, 100, 160, 80, (u8*)"人机对战", 1);
	DrawButton(120, 150, 160, 80, (u8*)"双人对战", 1);
	DrawButton(120, 200, 160, 80, (u8*)"关于", 1);
	DrawButton(120, 250, 160, 80, (u8*)"设置", 1);
	
	while (1)
	{
		static char ckey = 0;
		
		//判断触摸屏是否被按下
		if (tp_dev.sta&TP_PRES_DOWN)
		{
			int x = tp_dev.x[0];
			int y = tp_dev.y[0];
			
			if (x > 120 && x < 280 && y > 100 && y < 180)
			{
				if (ckey == 0)
				{
					ckey = 1;
					DrawButton(120, 100, 160, 80, (u8*)"人机对战", 0);
				}
				
			}
			if (x > 120 && x < 280 && y > 150 && y < 230)
			{
				if (ckey == 0)
				{
					ckey = 2;
					DrawButton(120, 150, 160, 80, (u8*)"双人对战", 0);
				}
				
			}
			if (x > 120&& x < 280 && y > 200 && y < 280)
			{
				if (ckey == 0)
				{
					ckey = 3;
					DrawButton(120, 200, 160, 80, (u8*)"关于", 0);
				}
				
			}
			if (x > 120 && x < 280 && y > 250 && y < 330)
			{
				if (ckey == 0)
				{
					ckey = 4;
					DrawButton(120, 250, 160, 80, (u8*)"设置", 1);
				}
				
			}
		}
		else if (ckey)
		{
			switch (ckey)
			{
				case 1:
					DrawButton(120, 100, 160, 80, (u8*)"人机对战", 1);
					ckey = 0;
					return 1;
				case 2:
					DrawButton(120, 150, 160, 80, (u8*)"双人对战", 1);
					ckey = 0;
					return 2;
				case 3:
					DrawButton(120, 200, 160, 80, (u8*)"关于", 1);
					ckey = 0;
					return 3;
				case 4:
					DrawButton(120, 250, 160, 80, (u8*)"设置", 1);
					ckey=0;
					return 4;
				default:
					break;
			}
		}
	}
}

//在提示点绿子闪烁
void CHESS_Blink(int chess_x,int chess_y)
{
	int xChess,yChess;
	xChess = CHESSBOARD_START_X + (chess_x*CHESSGRID_SIZE);
	yChess = CHESSBOARD_START_Y + (chess_y*CHESSGRID_SIZE);
	LcdDrawCircleA(xChess, yChess,  CHESSGRID_SIZE/2-1,GREEN , 1);
	delay_ms(200);
	LcdDrawCircleA(xChess, yChess, CHESSGRID_SIZE/2-1, CHESSBOARD_COLOR, 1);
	POINT_COLOR=BLACK;
	LCD_DrawLine(xChess-8,yChess,xChess+8,yChess);
	LCD_DrawLine(xChess,yChess-8,xChess,yChess+8);
	delay_ms(200);
	LcdDrawCircleA(xChess, yChess,  CHESSGRID_SIZE/2-1,GREEN , 1);
	delay_ms(200);
	LcdDrawCircleA(xChess, yChess, CHESSGRID_SIZE/2-1, CHESSBOARD_COLOR, 1);
	POINT_COLOR=BLACK;
	LCD_DrawLine(xChess-8,yChess,xChess+8,yChess);
	LCD_DrawLine(xChess,yChess-8,xChess,yChess+8);
	delay_ms(200);				    				  
}

//在人机对战下 请求帮助
void help(void)
{
	int help_x,help_y;
	//人机对战模式,触摸屏没有被按下，
	if ((g_Mode==1) && !(tp_dev.sta&TP_PRES_DOWN))
	{	
		if ((g_ChessNum>2)&&(g_ChessNum%2+1))	
		{	
			//计算AI下子坐标
			 GetAIPoint(&help_x, &help_y);	
		}
		else
		{
			return;
		}
	}		
	CHESS_Blink(help_x,help_y);	
}

static u8 g_Set(void)
{
	static char ckey = 0;
	
	POINT_COLOR = WHITE;
	
	//绘制关于窗口
	LCD_DrawRectangle(30, 110, 210, 220);
	LCD_DrawRectangle(31, 111, 209, 219);
	LCD_Fill(32, 112, 208, 218, BLACK);
	BACK_COLOR = BLACK;
	
	//文本
	Show_Str(104, 115, 32, 16, (u8*)"设置", 16, 0);

	
	//绘制按钮
	DrawButton(90, 140, 60, 20, (u8*)"声音", 1);
	DrawButton(90, 165, 60, 20, (u8*)"校准", 1);
	DrawButton(90, 190, 60, 20, (u8*)"返回", 1);
	
	while (1)
	{
		//判断触摸屏是否被按下
		if (tp_dev.sta&TP_PRES_DOWN)
		{
			int x = tp_dev.x[0];
			int y = tp_dev.y[0];
			
			if (x > 90 && x < 150 && y > 140 && y < 160)
			{
				if (ckey == 0)
				{
					ckey = 1;
					DrawButton(90, 140, 60, 20, (u8*)"声音", 0);
				}	
			}
			else if (x > 90 && x < 150 && y > 165 && y < 185)
			{
				if (ckey == 0)
				{
					ckey = 2;
					DrawButton(90, 165, 60, 20, (u8*)"校准", 0);
				}	
			}
			else if (x > 90 && x < 150 && y > 190 && y < 210)
			{
				if (ckey == 0)
				{
					ckey = 3;
					DrawButton(90, 190, 60, 20, (u8*)"返回", 0);
				}	
			}
			
		}
		else if (ckey==1)
		{
			DrawButton(90, 140, 60, 20, (u8*)"声音", 1);
			voice=!voice;
			if(voice) Show_Str(104, 80, 32, 16, (u8*)"打开", 16, 0);
			else Show_Str(104, 80, 32, 16, (u8*)"关闭", 16, 0);
			ckey = 0;
			g_Mode = 0;
			//return 0;
		}
		else if (ckey==2)
		{
			DrawButton(90, 165, 60, 20, (u8*)"校准", 1);
			PC0=1;
			ckey = 0;
			g_Mode = 0;		
			LED1=0;
			delay_ms(500);
			LED1=1;
			GameInit();
			return 1;
		}
		else if (ckey==3)
		{
			DrawButton(90, 195, 60, 20, (u8*)"返回", 1);
			ckey = 0;
			g_Mode = 0;
			return 1;
		}
	}
}

//游戏初始化
//初始化全局变量
//初始化棋盘
void GameInit(void)
{
	g_Mode = UIStart(); //UIStart获得按下的键位
	
	if (g_Mode == 1 || g_Mode == 2)
	{
		g_ChessNum = 0;	//棋子数量
		
		//清空缓存
		memset(g_Chess, 0, sizeof(g_Chess));
		
		LCD_Clear(BLACK);
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		//绘制边框
		LCD_DrawRectangle(0, 0, 239, 319);
		LCD_DrawRectangle(1, 1, 238, 318);
		LCD_DrawLine(2, CHESSBOARD_START_Y-8, 237, CHESSBOARD_START_Y-8);
		LCD_DrawLine(2, CHESSBOARD_END_Y+8, 237, CHESSBOARD_END_Y+8);
		LCD_DrawLine(2, CHESSBOARD_START_Y-9, 237, CHESSBOARD_START_Y-9);
		LCD_DrawLine(2, CHESSBOARD_END_Y+9, 237, CHESSBOARD_END_Y+9);
		POINT_COLOR = RED;
		if (g_Mode == 1) Show_Str((240 - 24*7), 5, 200, 24, (u8*)"人机对战五子棋", 24, 0);
		else Show_Str((240 - 24*7), 5, 200, 24, (u8*)"双人对战五子棋", 24, 0);
		
		//绘制棋盘
		DrawChessBoard();
		//绘制按键
		DrawButton(5, 570, 80, 25, (u8*)"重新开始", 1);
		DrawButton(87, 570, 48, 25, (u8*)"悔棋", 1);
		DrawButton(137, 570, 48, 25, (u8*)"帮助", 1);
		DrawButton(187, 570, 49, 25, (u8*)"返回", 1);
		Show_Str(240, 570, 120,25, (u8*)"倒计时:", 24, 0);
		
		
		
		
		
	}
	else if (g_Mode == 3) //弹出关于对话框
	{
		while (About())
		{
			GameInit();	//返回
			break;
		}
	}
	else if(g_Mode==4)
	{
		while(g_Set())
		{
			GameInit();	//返回
			break;
		}
	}
}

//开始游戏
void GameStart(void)
{

	
	
	u16 x, y;
	u32 timeCount=10;
	static int xChess = 0;
	static int yChess = 0;
	static int xPre = 0;
	static int yPre = 0;
	static uint8_t State = 0;
	static char skey = 0;
		for(nm=0;nm<100;nm++)
	{
		LCD_ShowxNum(330,570,timeCount,2,24,0);
	    delay_ms(10);
			if(nm==99){timeCount--;nm=0;}
	//判断触摸屏是否被按下
	if (tp_dev.sta&TP_PRES_DOWN)
	{
		//获取坐标值
		x = tp_dev.x[0];
		y = tp_dev.y[0];
		
		//判断是否在棋盘上
		if (x > CHESSBOARD_START_X && x < CHESSBOARD_END_X &&
		    y > CHESSBOARD_START_Y && y < CHESSBOARD_END_Y)
		{
			//如果人机对战且AI下棋
			if (g_Mode == 1 && (g_ChessNum%2 == 1)) return;
			//计算棋子坐标
			xChess = (x - CHESSBOARD_START_X ) / CHESSGRID_SIZE;
			yChess = (y - CHESSBOARD_START_Y ) / CHESSGRID_SIZE;
		}
		else
		{
			//判断按键是否按下
			if (x > 5 && x < 85 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 1;
					DrawButton(5, 570, 80, 25, (u8*)"重新开始", 0);
				}
			}
			if (x > 87 && x < 135 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 2;
					DrawButton(87, 570, 48, 25, (u8*)"悔棋", 0);
				}	
			}
			if (x > 137 && x < 185 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 3;
					DrawButton(137, 570, 48, 25, (u8*)"帮助", 0);
				}
			}
			if (x > 187 && x < 236 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 4;
					DrawButton(187, 570, 49, 25, (u8*)"返回", 0);
				}
			}
		}
	}
	else if (skey) //屏幕没有被按下时，检查是非按下过四个按键，按过的话执行按键操作
 	{
		switch (skey)
		{
			case 1:
				DrawButton(5, 570, 80, 25, (u8*)"重新开始", 1);
				ResStart();		//重新开始游戏
			  timeCount=10;
				skey = 0;
				State = 0;   //重新开始后可以下子
				break;
			case 2:
				DrawButton(87, 570, 48, 25, (u8*)"悔棋", 1);
				if (State == 0)
				{
					if ((g_ChessNum%2==0) && (g_Mode==1))
					{
						RetractChess();		//悔棋,人机对战的话悔棋两颗
					}
					RetractChess();
				}
				skey = 0;
				break;
			case 3:
				DrawButton(137, 570, 48, 25, (u8*)"帮助", 1);
				if(g_Mode==1)
				{
					help();			
				}
				skey = 0;
				break;
			case 4:
				DrawButton(187, 570, 49, 25, (u8*)"返回", 1);
				skey = 0;
				State = 0;
				GameInit();
				break;
			default:
				break;
		}
	}

	//判断是否可以下子
	if (!State) 
	{
		//人机对战模式,触摸屏没有被按下
		if ((g_Mode==1) && !(tp_dev.sta&TP_PRES_DOWN))
		{
			if ((g_ChessNum>0)&&(g_ChessNum%2))  //有棋子且轮到AI下子
			{
				//计算AI下子坐标
				GetAIPoint(&xChess, &yChess);
			}
			
		}
		//坐标改变
		if (xChess!=xPre || yChess!=yPre)
		{	
			if (g_ChessNum%2) delay_ms(300);
			DrawChess(xChess, yChess, g_ChessNum, 1);
			flag1=1;
			xPre = xChess;
			yPre = yChess;
			State = IsGameOver(xChess, yChess);  //isGameOver(),1是平局，2是五子相连，3是输出最后的反馈结果
		}
	}
	else if (State)
	{

		if (State==1 || State==2)
		{
			POINT_COLOR = WHITE;
			LCD_DrawRectangle(18, 138, 222, 178);
			LCD_DrawRectangle(19, 139, 221, 177);
			LCD_Fill(20, 140, 220, 176, BLACK);
			BACK_COLOR = BLACK;
			POINT_COLOR = RED;
			
			switch (State)
			{
				case 1:
					Show_Str(20 + (200 - 16*2)/2, 150, 200, 16, (u8*)"和棋", 16, 0);
					State = 3;
					break;
				case 2:
					if (g_ChessNum % 2)
					{
						if (g_Mode == 1)
						{
							Show_Str((240 - 16*4)/2, 150, 200, 16, (u8*)"你赢了!!", 16, 0);
		          BEEP_Run(100);  //凤鸣响
						}
						else
						{
							Show_Str((240 - 16*3)/2, 150, 200, 16, (u8*)"黑棋赢", 16, 0);
							BEEP_Run(100);  //凤鸣响
						}	
					}
					else
					{
						if (g_Mode == 1)
						{
							Show_Str((240 - 16*4)/2, 150, 200, 16, (u8*)"你输了!!", 16, 0);
							BEEP_Run(100);  //凤鸣响
						}
						else
						{
							Show_Str((240 - 16*3)/2, 150, 200, 16, (u8*)"白棋赢", 16, 0);
							BEEP_Run(100);  //凤鸣响
						}
					}
					State = 3;
					break;
				default:
					break;
			}
		}
		else if (State == 3)
		{
			char x, y;
			static int t = 0;
			delay_ms(10);
			t++;
			if (t == 100)
			{
				DrawChessBoard();
				for (x = 0; x < 15; x++)
				{
					for (y = 0; y < 15; y++)
					{
						if (g_Chess[x][y] > 0)
						{
							int num = g_Chess[x][y];
							DrawChess(x, y, num-1, 0);
						}
					}
				}
			}
			//每500毫秒
			if (t >= 200)
			{
				(g_ChessNum == 225) ? (State = 1) : (State = 2);
				t = 0;
			}	
		}
	}
	if(flag1){timeCount=10;flag1=0;};
	if(timeCount==0)
	{
	
	Show_Str((240 - 16*4)/2, 150, 200, 16, (u8*)"超时了!!", 16, 0);
		State=3;
		timeCount=10;
	}
}				
}