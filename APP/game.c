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



//����������ʼ����
#define CHESSBOARD_START_X		7
#define CHESSBOARD_START_Y		42
#define CHESSBOARD_END_X		450
#define CHESSBOARD_END_Y		500

//���̴�С
#define CHESSBOARD_SIZE			(CHESSBOARD_END_X-CHESSBOARD_START_X)
//�����С
#define CHESSGRID_SIZE			(CHESSBOARD_SIZE / 15)
//������ɫ
#define CHESSBOARD_COLOR		BROWN

char g_Chess[16][16];		//������ӻ��棬��Ӧ��X,Y�����ϵĵڼ�������
int  g_ChessNum;			//��������
int  g_SameChessNum;		//��������ͬ��������
int  g_SpaceFlag = 0;		//��λ��־  1Ϊ�� 0Ϊ����
char g_Mode=0;				//������ģʽ   0Ϊ�˻���ս1Ϊ˫�˶�ս
u8 voice=0;
u8 flag1=0;
u32 timeCount=10;
int nm;
//��������
//15x15
static void DrawChessBoard(void)
{
	int i;
	//������̱���ɫ
	LCD_Fill(CHESSBOARD_START_X-5, CHESSBOARD_START_Y-5, 
			CHESSBOARD_END_X+5, CHESSBOARD_END_Y+5, CHESSBOARD_COLOR);
	POINT_COLOR = BLACK;
	//���Ʊ߿���
	LCD_DrawRectangle(CHESSBOARD_START_X-3, CHESSBOARD_START_Y-3,
					  CHESSBOARD_END_X+3, CHESSBOARD_END_Y+3);
	LCD_DrawRectangle(CHESSBOARD_START_X-4, CHESSBOARD_START_Y-4,
					  CHESSBOARD_END_X+4, CHESSBOARD_END_Y+4);
	LCD_DrawRectangle(CHESSBOARD_START_X, CHESSBOARD_START_Y, 
					  CHESSBOARD_END_X, CHESSBOARD_END_Y);
	
	POINT_COLOR = BLACK;
	//���ƴ�ֱ��
	for (i = 1; i < 16; i++)
	{
		LCD_DrawLine(CHESSBOARD_START_X,
					 CHESSBOARD_START_Y + (CHESSGRID_SIZE*i),
					 CHESSBOARD_END_X,
					 CHESSBOARD_START_Y + (CHESSGRID_SIZE*i));
	}
	
	//����ˮƽ��
	for (i = 1; i < 16; i++)
	{
		LCD_DrawLine(CHESSBOARD_START_X + (CHESSGRID_SIZE*i),
					 CHESSBOARD_START_Y,
					 CHESSBOARD_START_X + (CHESSGRID_SIZE*i),
					 CHESSBOARD_END_Y);
	}
}

//��������
//x,y:��������(0~15)
static void DrawChess(int x, int y, int num, char sta)
{
	uint16_t ChessColor;
	uint16_t xChess;
	uint16_t yChess;
	
	if (x>15 || x<0 || y>15 || y<0)return;	//����Χ��
	//�ж������廹�Ǹ���
	if (sta)
	{
		if (g_Chess[x][y]!=0)return;			//��������
	}
	
	//�ж�������ɫ
	(num%2==0) ? (ChessColor=BLACK) : (ChessColor=WHITE);
	
	//������������
	xChess = CHESSBOARD_START_X + (x*CHESSGRID_SIZE);
	yChess = CHESSBOARD_START_Y + (y*CHESSGRID_SIZE);
	
	LcdDrawCircleA(xChess, 
				   yChess, 
				   CHESSGRID_SIZE/2-1, 
				   ChessColor, 
				   1);
	if(voice==1)          //�ж������Ƿ�� ���� ÿ�����ӷ�������100ms
		BEEP_Run(100);
	delay_ms(10);
	if (sta == 0) return;
	g_ChessNum++;
	g_Chess[x][y] = g_ChessNum;
}

//�������
//x,y:��������(0~14)
static void ClearChess(int x, int y)
{
	uint16_t ChessColor;
	uint16_t xChess;
	uint16_t yChess;
	
	if (x>14 || x<0 || y>14 || y<0)return;	//����Χ��
	if (g_Chess[x][y]!=0)return;			//��������
	
	ChessColor = CHESSBOARD_COLOR;
	
	//������������
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

//��ȡ��ͬ��������һ��ֻ���жϣ�
//x,y:��������(0~14)
//xStep,yStep:x,y�����ƶ���������ֵΪ-1,0,1
static int GetChessNum(char ChessType, int x, int y, int xStep, int yStep)//������x��yΪ��ǰ�㣬����һ���Ƿ�Ϊͬ��
{	
	g_SameChessNum = 0;
	while (1)
	{
		x = x + xStep;
		y = y + yStep;//ȡ��һ��
		
		//�ж��Ƿ�Խ��
		if (x < 0 || x > 14 || y < 0 || y > 14)
		{
			g_SpaceFlag = 0;
			return g_SameChessNum;
		}
		
		//û�����ӣ�����
		if (g_Chess[x][y]==0)
		{
			g_SpaceFlag = 1;
			return g_SameChessNum;
		}
		
		//����յ㣨x��y����Χ�˸�����ͬ������
		if (g_Chess[x][y] % 2 == ChessType)//�жϵ�ǰX��y���ϵ����������Ƿ���ϵ�ǰ��Ҫ���ҵ����ӵ�����
		{
			if ((xStep == 1 && yStep == 0) || (xStep == -1 && yStep == 0))//����
			{
				g_SameChessNum++;
			}
			else if ((xStep == 0 && yStep == 1) || (xStep == 0 && yStep == -1))//����
			{
				g_SameChessNum++;
			}
			else if ((xStep == 1 && yStep == 1) || (xStep == -1 && yStep == -1))//�ҶԽ���
			{
				g_SameChessNum++;
			}
			else if ((xStep == 1 && yStep == -1) || (xStep == -1 && yStep == 1))//��Խ���
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
//							 ���ܵ���AI������ؼ��㺯��                      //
/*=====================================================================*/
//��������״̬
//x, y:����
//xStep, yStep:�ƶ�����
static int Search(int color, int x, int y, int xStep, int yStep)
{
	int count = 1;
	int alive1 = 0, alive2 = 0;
	
	count += GetChessNum(color, x, y, xStep, yStep);//�ж���һ�������뵱ǰ�����Ƿ�ͬ����
	if (g_SpaceFlag) alive1 = 1;//��һ����Ϊ��
	count += GetChessNum(color, x, y, 0-xStep, 0-yStep);//ȡ���ߵĵ��ж��Ƿ�Ϊͬ����
	if (g_SpaceFlag) alive2 = 1;//��һ����Ϊ��
	
	if (count>=5) return 5;
	else if (alive1&&alive2) return count;//count=2
	else if ((alive1||alive2)&&count!=1) return count+4;//count=2��3
	else return 0;
}

//��ȡ����
//i,j:����
//color:0��ɫ  1��ɫ
//����: 0//��x��y�������ǲ�ͬɫ
//		2,���//�����ǿ�
//		3,����
//		4,����
//		5,��
//		6,����//һ���ǿ� һ��ͬɫ
//		7,����
//		8,����
static unsigned int GetScore(int i, int j, int color)
{
	int deadfour=0, deadthree=0, deadtwo=0;
	int five=0, alivefour=0, alivethree=0, alivetwo=0;
	int status[4];
	uint16_t score;
	int n;
	
//	(color==0) ? (g_ChessNum++) : (g_ChessNum+=2);
//	g_Chess[i][j] = g_ChessNum;
	status[0] = Search(color, i, j, 0, 1);  //����
	status[1] = Search(color, i, j, 1, 0);   //����
	status[2] = Search(color, i, j, 1, 1);   //���϶Խ���
	status[3] = Search(color, i, j, 1, -1);   //���¶Խ���
//	(color==0) ? (g_ChessNum--) : (g_ChessNum-=2);
//	g_Chess[i][j] = 0;
	
	//ͳ�Ƹ����������Ŀ
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

//�����������λ��
static int SearchValue(int *best_i, int *best_j, int color)
{
	char x, y;
	uint16_t  score[15][15];
	uint16_t  MaxScore = 0;
	
	memset(score, 0, sizeof(score));
	//����
	if (0 == color)
	{
		//������������  
		for (x = 0; x < 15; x++)
		{
			for (y = 0; y < 15; y++)
			{
				if (g_Chess[x][y] != 0) continue;  //��û�����ӵĵط�
				score[x][y] = GetScore(x, y, color);
			}
		}
		
		//����Ѱ�������������
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
	//����
	if (1 == color)
	{
		//������������
		for (x = 0; x < 15; x++)
		{
			for (y = 0; y < 15; y++)
			{
				if (g_Chess[x][y] != 0) continue;
				score[x][y] = GetScore(x, y, color);
			}
		}
		
		//����Ѱ�������������
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

//��ȡAI��������
//xChess,yChess:��������
//�ж����̵�ǰ�ľ���
static void GetAIPoint(int *xChess, int *yChess)  //�Ѽ��������������β�����
{
	unsigned int iWhiteValue, iBlackValue;
	int wi, wj, bi, bj;
	
	//�õ�����������λ��
	iWhiteValue = SearchValue(&wi, &wj, 1);//�õ��º������λ��
	iBlackValue = SearchValue(&bi, &bj, 0);//�õ��°�������λ��
	
	//��������,���Խ���
	if (iWhiteValue >= iBlackValue)
	{
		*xChess = wi;
		*yChess = wj;
		printf("\r\nx:%d  y:%d\r\n",wi,wj);
	}
	else if (iWhiteValue < iBlackValue)	//��������,���Է���
	{
		*xChess = bi;
		*yChess = bj;
		printf("\r\nx:%d  y:%d\r\n",bi,bj);
	}
}


/*=====================================================================*/
//						����������ؼ��㺯������                      //
/*=====================================================================*/

//�ж��Ƿ���������
//x,y:��������(0~14)
static char IsFiveChessContant(int x, int y)
{
	int ChessType = 0;
	int ChessNum = 0;

	ChessType = g_Chess[x][y] % 2;
	
	//�����ж�
	ChessNum = GetChessNum(ChessType, x, y, -1, 0) + GetChessNum(ChessType, x, y, 1, 0) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	//�����ж�
	ChessNum = GetChessNum(ChessType, x, y, 0, 1) + GetChessNum(ChessType, x, y, 0, -1) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	//б���ж�
	ChessNum = GetChessNum(ChessType, x, y, -1, 1) + GetChessNum(ChessType, x, y, 1, -1) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	//��б���ж�
	ChessNum = GetChessNum(ChessType, x, y, 1, 1) + GetChessNum(ChessType, x, y, -1, -1) + 1;
	
	if (ChessNum >= 5)
	{
		return 1;
	}
	
	return 0;
}

//��Ϸ����
//��ʤ����
//ƽ�ֽ���
//x,y:��������(0~14)

static uint8_t IsGameOver(uint8_t x, uint8_t y)
{
	//��������
	if (IsFiveChessContant(x, y)) return 2;
	
	//ƽ�ֽ���
	if (g_ChessNum == 225) return 1;
	
	return 0;
}

//����
static void RetractChess(void)
{
	int x, y;
	int xChess, yChess;
	
	//���û���������˳�
	if (g_ChessNum == 0) return;
	
	//��ѯ���һ�����Ӳ���¼
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
	
	//������һ������
	g_Chess[xChess][yChess] = 0;
	g_ChessNum -= 1;
	ClearChess(xChess, yChess);
}

//���¿�ʼ
static void ResStart(void)
{
	g_ChessNum = 0;	//��������
	//�����������
	memset(g_Chess, 0, sizeof(g_Chess));
	
	//��������
	DrawChessBoard();
}

//���ư�ť
//x,y:����
//width,height:���,�߶�
//*Text:�ı�
//sta:��ť״̬(1���� 0����)
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

//���ڶԻ������
static char About(void)
{
	static char ckey = 0;
	
	POINT_COLOR = WHITE;
	//���ƹ��ڴ���
	LCD_DrawRectangle(20, 110, 220, 220);
	LCD_DrawRectangle(21, 111, 219, 219);
	LCD_Fill(22, 112, 218, 218, BLACK);
	BACK_COLOR = BLACK;
	//�ı�
	Show_Str(104, 115, 32, 16, (u8*)"����", 16, 0);
	Show_Str(40, 135, 136, 16, (u8*)"ƽ̨:STM32̽���߿�����", 16, 0);
	Show_Str(40, 155, 136, 16, (u8*)"����:���", 16, 0);
	Show_Str(40, 175, 136, 16, (u8*)"��Ϸ��:����������", 16, 0);
	//���ư�ť
	DrawButton(90, 195, 60, 20, (u8*)"�˳�", 1);
	
	while (1)
	{
		//�жϴ������Ƿ񱻰���
		if (tp_dev.sta&TP_PRES_DOWN)
		{
			int x = tp_dev.x[0];
			int y = tp_dev.y[0];
			
			if (x > 90 && x < 150 && y > 195 && y < 215)
			{
				if (ckey == 0)
				{
					ckey = 1;
					DrawButton(90, 195, 60, 20, (u8*)"�˳�", 0);
				}	
			}
		}
		else if (ckey)
		{
			DrawButton(90, 195, 60, 20, (u8*)"�˳�", 1);
			ckey = 0;
			g_Mode = 0;
			return 1;
		}
	}
}




//�����ʼ��
static char UIStart(void)
{
	LCD_Clear(GBLUE);
	BACK_COLOR = GBLUE;
	POINT_COLOR = BLACK;
	Show_Str((240 - 24*3)-30, 30, 200, 48, (u8*)"����������", 24, 0);
	
	//������ť
	DrawButton(120, 100, 160, 80, (u8*)"�˻���ս", 1);
	DrawButton(120, 150, 160, 80, (u8*)"˫�˶�ս", 1);
	DrawButton(120, 200, 160, 80, (u8*)"����", 1);
	DrawButton(120, 250, 160, 80, (u8*)"����", 1);
	
	while (1)
	{
		static char ckey = 0;
		
		//�жϴ������Ƿ񱻰���
		if (tp_dev.sta&TP_PRES_DOWN)
		{
			int x = tp_dev.x[0];
			int y = tp_dev.y[0];
			
			if (x > 120 && x < 280 && y > 100 && y < 180)
			{
				if (ckey == 0)
				{
					ckey = 1;
					DrawButton(120, 100, 160, 80, (u8*)"�˻���ս", 0);
				}
				
			}
			if (x > 120 && x < 280 && y > 150 && y < 230)
			{
				if (ckey == 0)
				{
					ckey = 2;
					DrawButton(120, 150, 160, 80, (u8*)"˫�˶�ս", 0);
				}
				
			}
			if (x > 120&& x < 280 && y > 200 && y < 280)
			{
				if (ckey == 0)
				{
					ckey = 3;
					DrawButton(120, 200, 160, 80, (u8*)"����", 0);
				}
				
			}
			if (x > 120 && x < 280 && y > 250 && y < 330)
			{
				if (ckey == 0)
				{
					ckey = 4;
					DrawButton(120, 250, 160, 80, (u8*)"����", 1);
				}
				
			}
		}
		else if (ckey)
		{
			switch (ckey)
			{
				case 1:
					DrawButton(120, 100, 160, 80, (u8*)"�˻���ս", 1);
					ckey = 0;
					return 1;
				case 2:
					DrawButton(120, 150, 160, 80, (u8*)"˫�˶�ս", 1);
					ckey = 0;
					return 2;
				case 3:
					DrawButton(120, 200, 160, 80, (u8*)"����", 1);
					ckey = 0;
					return 3;
				case 4:
					DrawButton(120, 250, 160, 80, (u8*)"����", 1);
					ckey=0;
					return 4;
				default:
					break;
			}
		}
	}
}

//����ʾ��������˸
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

//���˻���ս�� �������
void help(void)
{
	int help_x,help_y;
	//�˻���սģʽ,������û�б����£�
	if ((g_Mode==1) && !(tp_dev.sta&TP_PRES_DOWN))
	{	
		if ((g_ChessNum>2)&&(g_ChessNum%2+1))	
		{	
			//����AI��������
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
	
	//���ƹ��ڴ���
	LCD_DrawRectangle(30, 110, 210, 220);
	LCD_DrawRectangle(31, 111, 209, 219);
	LCD_Fill(32, 112, 208, 218, BLACK);
	BACK_COLOR = BLACK;
	
	//�ı�
	Show_Str(104, 115, 32, 16, (u8*)"����", 16, 0);

	
	//���ư�ť
	DrawButton(90, 140, 60, 20, (u8*)"����", 1);
	DrawButton(90, 165, 60, 20, (u8*)"У׼", 1);
	DrawButton(90, 190, 60, 20, (u8*)"����", 1);
	
	while (1)
	{
		//�жϴ������Ƿ񱻰���
		if (tp_dev.sta&TP_PRES_DOWN)
		{
			int x = tp_dev.x[0];
			int y = tp_dev.y[0];
			
			if (x > 90 && x < 150 && y > 140 && y < 160)
			{
				if (ckey == 0)
				{
					ckey = 1;
					DrawButton(90, 140, 60, 20, (u8*)"����", 0);
				}	
			}
			else if (x > 90 && x < 150 && y > 165 && y < 185)
			{
				if (ckey == 0)
				{
					ckey = 2;
					DrawButton(90, 165, 60, 20, (u8*)"У׼", 0);
				}	
			}
			else if (x > 90 && x < 150 && y > 190 && y < 210)
			{
				if (ckey == 0)
				{
					ckey = 3;
					DrawButton(90, 190, 60, 20, (u8*)"����", 0);
				}	
			}
			
		}
		else if (ckey==1)
		{
			DrawButton(90, 140, 60, 20, (u8*)"����", 1);
			voice=!voice;
			if(voice) Show_Str(104, 80, 32, 16, (u8*)"��", 16, 0);
			else Show_Str(104, 80, 32, 16, (u8*)"�ر�", 16, 0);
			ckey = 0;
			g_Mode = 0;
			//return 0;
		}
		else if (ckey==2)
		{
			DrawButton(90, 165, 60, 20, (u8*)"У׼", 1);
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
			DrawButton(90, 195, 60, 20, (u8*)"����", 1);
			ckey = 0;
			g_Mode = 0;
			return 1;
		}
	}
}

//��Ϸ��ʼ��
//��ʼ��ȫ�ֱ���
//��ʼ������
void GameInit(void)
{
	g_Mode = UIStart(); //UIStart��ð��µļ�λ
	
	if (g_Mode == 1 || g_Mode == 2)
	{
		g_ChessNum = 0;	//��������
		
		//��ջ���
		memset(g_Chess, 0, sizeof(g_Chess));
		
		LCD_Clear(BLACK);
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		//���Ʊ߿�
		LCD_DrawRectangle(0, 0, 239, 319);
		LCD_DrawRectangle(1, 1, 238, 318);
		LCD_DrawLine(2, CHESSBOARD_START_Y-8, 237, CHESSBOARD_START_Y-8);
		LCD_DrawLine(2, CHESSBOARD_END_Y+8, 237, CHESSBOARD_END_Y+8);
		LCD_DrawLine(2, CHESSBOARD_START_Y-9, 237, CHESSBOARD_START_Y-9);
		LCD_DrawLine(2, CHESSBOARD_END_Y+9, 237, CHESSBOARD_END_Y+9);
		POINT_COLOR = RED;
		if (g_Mode == 1) Show_Str((240 - 24*7), 5, 200, 24, (u8*)"�˻���ս������", 24, 0);
		else Show_Str((240 - 24*7), 5, 200, 24, (u8*)"˫�˶�ս������", 24, 0);
		
		//��������
		DrawChessBoard();
		//���ư���
		DrawButton(5, 570, 80, 25, (u8*)"���¿�ʼ", 1);
		DrawButton(87, 570, 48, 25, (u8*)"����", 1);
		DrawButton(137, 570, 48, 25, (u8*)"����", 1);
		DrawButton(187, 570, 49, 25, (u8*)"����", 1);
		Show_Str(240, 570, 120,25, (u8*)"����ʱ:", 24, 0);
		
		
		
		
		
	}
	else if (g_Mode == 3) //�������ڶԻ���
	{
		while (About())
		{
			GameInit();	//����
			break;
		}
	}
	else if(g_Mode==4)
	{
		while(g_Set())
		{
			GameInit();	//����
			break;
		}
	}
}

//��ʼ��Ϸ
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
	//�жϴ������Ƿ񱻰���
	if (tp_dev.sta&TP_PRES_DOWN)
	{
		//��ȡ����ֵ
		x = tp_dev.x[0];
		y = tp_dev.y[0];
		
		//�ж��Ƿ���������
		if (x > CHESSBOARD_START_X && x < CHESSBOARD_END_X &&
		    y > CHESSBOARD_START_Y && y < CHESSBOARD_END_Y)
		{
			//����˻���ս��AI����
			if (g_Mode == 1 && (g_ChessNum%2 == 1)) return;
			//������������
			xChess = (x - CHESSBOARD_START_X ) / CHESSGRID_SIZE;
			yChess = (y - CHESSBOARD_START_Y ) / CHESSGRID_SIZE;
		}
		else
		{
			//�жϰ����Ƿ���
			if (x > 5 && x < 85 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 1;
					DrawButton(5, 570, 80, 25, (u8*)"���¿�ʼ", 0);
				}
			}
			if (x > 87 && x < 135 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 2;
					DrawButton(87, 570, 48, 25, (u8*)"����", 0);
				}	
			}
			if (x > 137 && x < 185 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 3;
					DrawButton(137, 570, 48, 25, (u8*)"����", 0);
				}
			}
			if (x > 187 && x < 236 && y > 570 && y < 595)
			{
				if (skey == 0)
				{
					skey = 4;
					DrawButton(187, 570, 49, 25, (u8*)"����", 0);
				}
			}
		}
	}
	else if (skey) //��Ļû�б�����ʱ������Ƿǰ��¹��ĸ������������Ļ�ִ�а�������
 	{
		switch (skey)
		{
			case 1:
				DrawButton(5, 570, 80, 25, (u8*)"���¿�ʼ", 1);
				ResStart();		//���¿�ʼ��Ϸ
			  timeCount=10;
				skey = 0;
				State = 0;   //���¿�ʼ���������
				break;
			case 2:
				DrawButton(87, 570, 48, 25, (u8*)"����", 1);
				if (State == 0)
				{
					if ((g_ChessNum%2==0) && (g_Mode==1))
					{
						RetractChess();		//����,�˻���ս�Ļ���������
					}
					RetractChess();
				}
				skey = 0;
				break;
			case 3:
				DrawButton(137, 570, 48, 25, (u8*)"����", 1);
				if(g_Mode==1)
				{
					help();			
				}
				skey = 0;
				break;
			case 4:
				DrawButton(187, 570, 49, 25, (u8*)"����", 1);
				skey = 0;
				State = 0;
				GameInit();
				break;
			default:
				break;
		}
	}

	//�ж��Ƿ��������
	if (!State) 
	{
		//�˻���սģʽ,������û�б�����
		if ((g_Mode==1) && !(tp_dev.sta&TP_PRES_DOWN))
		{
			if ((g_ChessNum>0)&&(g_ChessNum%2))  //���������ֵ�AI����
			{
				//����AI��������
				GetAIPoint(&xChess, &yChess);
			}
			
		}
		//����ı�
		if (xChess!=xPre || yChess!=yPre)
		{	
			if (g_ChessNum%2) delay_ms(300);
			DrawChess(xChess, yChess, g_ChessNum, 1);
			flag1=1;
			xPre = xChess;
			yPre = yChess;
			State = IsGameOver(xChess, yChess);  //isGameOver(),1��ƽ�֣�2������������3��������ķ������
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
					Show_Str(20 + (200 - 16*2)/2, 150, 200, 16, (u8*)"����", 16, 0);
					State = 3;
					break;
				case 2:
					if (g_ChessNum % 2)
					{
						if (g_Mode == 1)
						{
							Show_Str((240 - 16*4)/2, 150, 200, 16, (u8*)"��Ӯ��!!", 16, 0);
		          BEEP_Run(100);  //������
						}
						else
						{
							Show_Str((240 - 16*3)/2, 150, 200, 16, (u8*)"����Ӯ", 16, 0);
							BEEP_Run(100);  //������
						}	
					}
					else
					{
						if (g_Mode == 1)
						{
							Show_Str((240 - 16*4)/2, 150, 200, 16, (u8*)"������!!", 16, 0);
							BEEP_Run(100);  //������
						}
						else
						{
							Show_Str((240 - 16*3)/2, 150, 200, 16, (u8*)"����Ӯ", 16, 0);
							BEEP_Run(100);  //������
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
			//ÿ500����
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
	
	Show_Str((240 - 16*4)/2, 150, 200, 16, (u8*)"��ʱ��!!", 16, 0);
		State=3;
		timeCount=10;
	}
}				
}