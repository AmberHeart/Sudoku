#include<stdio.h>
#include<easyx.h>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#define GAP 25 //最外边框宽度
#define S_WID 50//每一个数独小框宽度
#define SGAP_WID 5	//一个中型框内小框之间的间隔
#define M_WID (3*S_WID + 4*SGAP_WID)	//中型框宽度
#define MGAP_WID 10 //中型框之间的距离
#define L_WID (3*M_WID + 4*MGAP_WID)//大型框宽度
/*常文正的大作业——PB21111706*/
int log = 0;//判断是否可解
int soduku[9][9];//数独数组
int origin[9][9];//原始数据数组
int answer[9][9];//答案数组
int tip[9][9];//提示数组
int times[9][9];// 检查是否死循环数组
//记录数独哪些元素是初始的
void initsoduku()
{
	int i, j;
	for (i = 0; i <= 8; i++)
	{
		for (j = 0; j <= 8; j++)
		{
			if (soduku[i][j] != 0)
			{
				origin[i][j] = 1;
			}
			answer[i][j] = soduku[i][j];
		}
	}
}
//玩家坐标
struct player
{
	int x = GAP + MGAP_WID + SGAP_WID;//选定框坐标
	int y = GAP + MGAP_WID + SGAP_WID;
	int i = 0;//选定框对应的数独位置
	int j = 0;
}player;
//判定输入数据是否合法
void valid(HWND hnd)
{
	int i, j;
	int row, column;
	int isPossible = 1;
	int num;
	//遍历第i行查询是否有重复的值
	for (i = 0; i <= 8; i++)
	{
		for (j = 0; j <= 8; j++)
		{
			num = soduku[i][j];
			if (num != 0)
			{
				for (column = 0; column < 9; column++)
				{
					if (num == soduku[i][column] && j != column) isPossible *= 0;
					else isPossible *= 1;
				}
				//遍历第j列查询是否有重复的值
				for (row = 0; row < 9; row++)
				{
					if (num == soduku[row][j] && row != i) isPossible *= 0;
					else isPossible *= 1;
				}
				//遍历x[i][i]所在宫(box)查询是否有重复的值
				int box_i, box_j;
				//定位宫的位置
				int offset_x, offset_y;
				//计算x方向(即j序号增加方向)偏移量
				offset_x = j - j % 3;
				//计算y方向(即i序号增加方向)偏移量
				offset_y = i - i % 3;
				for (box_i = 0; box_i < 3; box_i++)
				{
					for (box_j = 0; box_j < 3; box_j++)
					{
						//将box_i,box_j修正到正确的九宫格
						if (num == soduku[box_i + offset_y][box_j + offset_x] && box_i + offset_y != i && box_j + offset_x != j) isPossible *= 0;
						else isPossible *= 1;
						if (!isPossible)
							break;
					}
					if (!isPossible)
						break;
				}
			}
			if (!isPossible)
				break;
		}
		if (!isPossible)
			break;
	}
	if (!isPossible)
		MessageBox(hnd, "当前数独不合法", "检查", MB_OK);
	else
		MessageBox(hnd, "当前数独合法", "检查", MB_OK);
}
//提示函数
void tips()
{
	int i, j, flag = 0;
	for (i = 0; i <= 8; i++)
	{
		for (j = 0; j <= 8; j++)
		{
			if (soduku[i][j] != answer[i][j])
			{
				soduku[i][j] = answer[i][j];
				tip[i][j] = 1;
				flag = 1;
				break;
			}
		}
		if (flag)
			break;
	}
}
//validate函数负责检查下x[i][j]是否为可行的值
int validate(int x[9][9], int i, int j, int num)
{
	int row, column;
	int isPossible = 1;
	//遍历第i行查询是否有重复的值
	for (column = 0; column < 9; column++)
	{
		if (num == x[i][column]) isPossible *= 0;
		else isPossible *= 1;
	}
	//遍历第j列查询是否有重复的值
	for (row = 0; row < 9; row++)
	{
		if (num == x[row][j]) isPossible *= 0;
		else isPossible *= 1;
	}
	//遍历x[i][i]所在宫(box)查询是否有重复的值
	int box_i, box_j;
	//定位宫的位置
	int offset_x, offset_y;
	//计算x方向(即j序号增加方向)偏移量
	offset_x = j - j % 3;
	//计算y方向(即i序号增加方向)偏移量
	offset_y = i - i % 3;
	for (box_i = 0; box_i < 3; box_i++)
	{
		for (box_j = 0; box_j < 3; box_j++)
		{
			//将box_i,box_j修正到正确的九宫格
			if (num == x[box_i + offset_y][box_j + offset_x]) isPossible *= 0;
			else isPossible *= 1;
		}
	}
	//验证无误返回1，否则返回0
	return isPossible;
}
//数独是否可解判断
int  judgeanswer(HWND hnd, int i, int j)
{
	if (times[i][j] > 20)
	{
		MessageBox(hnd, "此数独无解", "警告", MB_OK);
		return 1;
	}
	else return 0;
}
//solve_soduku用来解出唯一解
void solve_soduku(int x[9][9], HWND hnd)
{
	int i, j, num, flag = 0, m, n;
	for (i = 0; i < 9; i++)	//先找到第一个需要填的数 方便之后判断数独无解
	{
		for (j = 0; j < 9; j++)
		{
			if (origin[i][j] == 0)
			{
				flag = 1;
				break;
			}
		}
		if (flag)
			break;
	}
	flag = 0;//flag 初始化
	m = i;	
	n = j;
	//初始化i和j
	i = 0;
	j = 0;
	//遍历九宫格，从x[0][0]开始尝试
	while (i < 9)
	{
		if (log)
			break;
		while (j < 9)
		{
			//无解退出
			if (log)
				break;
			//判断是否为上一行试错跳转而来
		start:if (flag == 1 && j == -1)
		{
			i--;
			j = 8;
			goto start;
		}
		//判断是否为试错跳转而来且当前值不可修改
		else if (flag == 1 && origin[i][j] == 1)
		{
			//继续返回上一层
			j--;
			continue;
		}
		//判断当前格子是否不可修改
		else if (origin[i][j] == 1)
		{
			j++;
			continue;
		}
		//开始尝试
		else
		{
			if (i == m && j == n)
			{
				times[m][n]++;
				if (judgeanswer(hnd, m, n))
					log = 1;
			}
			num = x[i][j];
			//若当前格子还未赋值，即不是由上一次试错跳转而来，从1开始尝试
			if (num == 0) num = 1;
			//若flag==1即是由上一次试错而来，从上一次错误的值+1继续尝试
			else if (flag == 1) num = x[i][j] + 1;
			//初始化跳转标识的值
			flag = 0;
			//从num开始向后逐个尝试
			for (; num <= 10; num++)
			{
				//用validate函数判断尝试的值是否可行
				//如果可行将当前格子的值变为num
				if (validate(x, i, j, num) == 1 && num < 10)
				{
					x[i][j] = num;
					j++;
					break;
				}
				//如果到9还没有可行的值填入，说明前面有错，初始当前值为0，返回上一次尝试
				else if (num == 10)
				{
					x[i][j] = 0;
					j--;
					//打上跳转标识flag=1
					flag = 1;
					break;
				}
			}
		}
		}
		//一行填满从下一行第一个继续尝试
		j = 0;
		i++;
	}
}
//创造按钮
void button(int x, int y, int w, int h, const char str[])
{
	//绘制按钮
	setfillcolor(RGB(142, 123, 99));
	fillroundrect(x, y, x + w, y + h, 5, 5);
	settextcolor(RGB(248, 242, 243));
	settextstyle(30, 0, "微软雅黑");
	setbkmode(0);
	outtextxy(x + (w - textwidth(str)) / 2, y + (h - textheight(str)) / 2, str);
}
//绘制小框
void draww(int x, int y)
{
	int i, j;
	for (i = 1; i <= 3; i++)//行
	{
		for (j = 1; j <= 3; j++)	//列
		{
			setfillcolor(RGB(187, 172, 160));
			fillrectangle(x + SGAP_WID * j + S_WID * (j - 1), y + SGAP_WID * i + S_WID * (i - 1), x + SGAP_WID * j + S_WID * j, y + SGAP_WID * i + S_WID * i);
		}
	}
}
//画出棋盘
void build()
{
	//绘制底框
	setfillcolor(RGB(189, 172, 160));
	fillrectangle(GAP, GAP, GAP + L_WID, GAP + L_WID);
	//绘制中框
	int i, j;
	for (i = 1; i <= 3; i++)//行
	{
		for (j = 1; j <= 3; j++)	//列
		{
			setfillcolor(RGB(205, 193, 181));
			fillrectangle(GAP + MGAP_WID * j + M_WID * (j - 1), GAP + MGAP_WID * i + M_WID * (i - 1), GAP + MGAP_WID * j + M_WID * j, GAP + MGAP_WID * i + M_WID * i);
			draww(GAP + MGAP_WID * j + M_WID * (j - 1), GAP + MGAP_WID * i + M_WID * (i - 1));
		}
	}
}
//选择框移动函数
void move(int n)
{
	switch (n)
	{
	case 72://向上
		if (player.y > GAP + MGAP_WID + SGAP_WID)
		{
			player.i--;
			player.y -= S_WID + SGAP_WID;
			if (player.y == GAP + L_WID - (MGAP_WID + 4 * (S_WID + SGAP_WID))
				|| player.y == GAP + L_WID - (2 * MGAP_WID + SGAP_WID + 7 * (S_WID + SGAP_WID)))
				player.y -= MGAP_WID + SGAP_WID;
		}
		break;
	case 80://向下
	{
		if (player.y < GAP + L_WID - MGAP_WID - (S_WID + SGAP_WID))
		{
			player.i++;
			player.y += S_WID + SGAP_WID;
			if (player.y == GAP + MGAP_WID + SGAP_WID + 3 * (S_WID + SGAP_WID)
				|| player.y == GAP + 2 * MGAP_WID + 2 * SGAP_WID + 6 * (S_WID + SGAP_WID))
				player.y += MGAP_WID + SGAP_WID;
		}
		break;
	}
	case 75://向左
	{
		if (player.x > GAP + MGAP_WID + SGAP_WID)
		{
			player.j--;
			player.x -= S_WID + SGAP_WID;
			if (player.x == GAP + L_WID - (MGAP_WID + 4 * (S_WID + SGAP_WID))
				|| player.x == GAP + L_WID - (2 * MGAP_WID + SGAP_WID + 7 * (S_WID + SGAP_WID)))
				player.x -= MGAP_WID + SGAP_WID;
		}
		break;
	}
	case 77://向右
	{
		if (player.x < GAP + L_WID - MGAP_WID - (S_WID + SGAP_WID))
		{
			player.j++;
			player.x += S_WID + SGAP_WID;
			if (player.x == GAP + MGAP_WID + SGAP_WID + 3 * (S_WID + SGAP_WID)
				|| player.x == GAP + 2 * MGAP_WID + 2 * SGAP_WID + 6 * (S_WID + SGAP_WID))
				player.x += MGAP_WID + SGAP_WID;
		}
		break;
	}
	}
}
//像框内填数字函数
void fillnum()
{
	char temp[20];
	int i, j;//i 行 j 列
	int x, y;
	for (i = 0; i <= 8; i++)
	{
		for (j = 0; j <= 8; j++)
		{
			if (soduku[i][j] != 0)//如果是0则不显示
			{
				x = GAP + MGAP_WID + SGAP_WID + j * (S_WID + SGAP_WID);
				y = GAP + MGAP_WID + SGAP_WID + i * (S_WID + SGAP_WID);
				if (i >= 3 && i < 6)
					y += (MGAP_WID + SGAP_WID);
				if (i >= 6)
					y += 2 * (MGAP_WID + SGAP_WID);
				if (j >= 3 && j < 6)
					x += (MGAP_WID + SGAP_WID);
				if (j >= 6)
					x += 2 * (MGAP_WID + SGAP_WID);
				if (origin[i][j] == 1)
					settextcolor(RGB(119, 109, 101));	//黑色标出原始数组
				else if (tip[i][j] == 1)
					settextcolor(RGB(236, 38, 114));	//红色标出提示的字符
				else
					settextcolor(WHITE);
				setbkmode(TRANSPARENT);
				settextstyle(40, 0, "Arial");
				sprintf_s(temp, "%d", soduku[i][j]);	//能打印出数字所以要转化一下，否则会按照ASCII码
				outtextxy(x + (S_WID - textwidth(temp)) / 2, y + 5, temp);
			}
		}
	}
}
//绘图函数
void draw()
{
	//双缓冲绘图
	BeginBatchDraw();
	//设置背景色
	setbkcolor(RGB(251, 248, 240));
	cleardevice();
	//绘制棋盘	
	build();
	//绘制按钮
	button(GAP + L_WID + 5 * GAP, L_WID / 3, M_WID, M_WID / 3, "提示");
	button(GAP + L_WID + 5 * GAP, M_WID + L_WID / 3, M_WID, M_WID / 3, "检查");
	//绘制选定框
	setfillcolor(RGB(246, 124, 94));
	fillrectangle(player.x, player.y, player.x + S_WID, player.y + S_WID);
	//绘制数字
	fillnum();
	//绘制文本提示
	settextstyle(30, 0, "微软雅黑");
	setbkmode(TRANSPARENT);
	settextcolor(RGB(116, 103, 94));
	outtextxy(GAP + L_WID + S_WID, GAP, "通过键盘↑↓←→控制选定框移动");
	outtextxy(GAP + L_WID + S_WID, GAP + 40, "输入数字改变框内数字 0是空缺");
	outtextxy(GAP + L_WID + S_WID, GAP + 80, "鼠标点击按钮可提示或检查");
	outtextxy(GAP + L_WID + S_WID, GAP + 120, "或者按R检查 T提示");
	//结束缓冲绘图
	EndBatchDraw();
}
//插入数字
void insertnum(int n)
{
	int i = player.i;
	int j = player.j;
	if (origin[i][j] == 0 && tip[i][j] == 0)
	{
		switch (n)
		{
		case 48:
		case 96://0
			soduku[i][j] = 0;
			break;
		case 49:
		case 97://1
			soduku[i][j] = 1;
			break;
		case 50:
		case 98://2
			soduku[i][j] = 2;
			break;
		case 51:
		case 99://3
			soduku[i][j] = 3;
			break;
		case 52:
		case 100://4
			soduku[i][j] = 4;
			break;
		case 53:
		case 101:
			soduku[i][j] = 5;
			break;
		case 54:
		case 102:
			soduku[i][j] = 6;
			break;
		case 55:
		case 103:
			soduku[i][j] = 7;
			break;
		case 56:
		case 104:
			soduku[i][j] = 8;
			break;
		case 57:
		case 105:
			soduku[i][j] = 9;
			break;
		}
	}
}
//把数独解出的判断
int judgewin(HWND hnd)
{
	int i, j;
	int flag = 1;
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			if (soduku[i][j] != answer[i][j])
			{
				flag = 0;
			}
			if (!flag)
			{
				break;
			}
		}
		if (!flag)
			break;
	}
	if (flag)
	{
		MessageBoxA(hnd, "恭喜你解出来了数独", "成功", MB_OK);
	}
	return flag;
}
//主函数
int main()
{
	int i, j;
	char temp;
	HWND hnd = GetHWnd();	//获取句柄
	ExMessage msg;	//用来获取鼠标消息
	printf("请输入您要求解的数独:\n");
	printf("注意数独空缺应该为0\n");
	printf("  123456789\n");
	//输入数独
	for (i = 0; i < 9; i++)
	{
		printf("%d:", i + 1);
		for (j = 0; j < 10; j++)
		{
			temp = getchar();
			if (j < 9) soduku[i][j] = temp - '0';
		}
	}
	initsoduku();//记录初始数独
	solve_soduku(answer, hnd);//把数独解出来 判断输入数独是否有解
	if (!log)//数独有解才能进行下面的游戏
	{
		//视图初始化
		initgraph(960, 2 * GAP + L_WID, 0);
		hnd = GetHWnd();//获取窗口句柄
		while (1)
		{
			//绘图
			draw();
			//记录键盘信息
			if (_kbhit())
			{
				int kb = _getch();
				move(kb);
				insertnum(kb);
				if(kb == 116)
					if (MessageBox(hnd, "确定要使用提示吗？", "提示", MB_OKCANCEL) == 1)//ok返回值为1 cancel则为2
					{
						tips();
					}
				if(kb == 114)
					if(MessageBox(hnd, "确定要使用检查吗？", "检查", MB_OKCANCEL) == 1)
				{
					valid(hnd);
				}
			}
			//记录鼠标信息
			if (peekmessage(&msg, EM_MOUSE))//点按钮
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					if (msg.x >= GAP + L_WID + 5 * GAP && msg.x <= GAP + L_WID + 5 * GAP + M_WID
						&& msg.y >= L_WID / 3 && msg.y <= L_WID / 3 + M_WID / 3)//按下了提示按钮
					{
						if (MessageBox(hnd, "确定要使用提示吗？", "提示", MB_OKCANCEL) == 1)//ok返回值为1 cancel则为2
						{
							tips();
						}
					}
					if (msg.x >= GAP + L_WID + 5 * GAP && msg.x <= GAP + L_WID + 5 * GAP + M_WID
						&& msg.y >= M_WID + L_WID / 3 && msg.y <= M_WID + L_WID / 3 + M_WID / 3)//按下了检查按钮
					{
						if (MessageBox(hnd, "确定要使用检查吗？", "检查", MB_OKCANCEL) == 1)
						{
							valid(hnd);
						}
					}
				}
			}
			//判断是否成功
			if (judgewin(hnd))
				break;
		}
	}
	return 0;
}