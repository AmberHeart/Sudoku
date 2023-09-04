#include<stdio.h>
#include<easyx.h>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#define GAP 25 //����߿���
#define S_WID 50//ÿһ������С����
#define SGAP_WID 5	//һ�����Ϳ���С��֮��ļ��
#define M_WID (3*S_WID + 4*SGAP_WID)	//���Ϳ���
#define MGAP_WID 10 //���Ϳ�֮��ľ���
#define L_WID (3*M_WID + 4*MGAP_WID)//���Ϳ���
/*�������Ĵ���ҵ����PB21111706*/
int log = 0;//�ж��Ƿ�ɽ�
int soduku[9][9];//��������
int origin[9][9];//ԭʼ��������
int answer[9][9];//������
int tip[9][9];//��ʾ����
int times[9][9];// ����Ƿ���ѭ������
//��¼������ЩԪ���ǳ�ʼ��
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
//�������
struct player
{
	int x = GAP + MGAP_WID + SGAP_WID;//ѡ��������
	int y = GAP + MGAP_WID + SGAP_WID;
	int i = 0;//ѡ�����Ӧ������λ��
	int j = 0;
}player;
//�ж����������Ƿ�Ϸ�
void valid(HWND hnd)
{
	int i, j;
	int row, column;
	int isPossible = 1;
	int num;
	//������i�в�ѯ�Ƿ����ظ���ֵ
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
				//������j�в�ѯ�Ƿ����ظ���ֵ
				for (row = 0; row < 9; row++)
				{
					if (num == soduku[row][j] && row != i) isPossible *= 0;
					else isPossible *= 1;
				}
				//����x[i][i]���ڹ�(box)��ѯ�Ƿ����ظ���ֵ
				int box_i, box_j;
				//��λ����λ��
				int offset_x, offset_y;
				//����x����(��j������ӷ���)ƫ����
				offset_x = j - j % 3;
				//����y����(��i������ӷ���)ƫ����
				offset_y = i - i % 3;
				for (box_i = 0; box_i < 3; box_i++)
				{
					for (box_j = 0; box_j < 3; box_j++)
					{
						//��box_i,box_j��������ȷ�ľŹ���
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
		MessageBox(hnd, "��ǰ�������Ϸ�", "���", MB_OK);
	else
		MessageBox(hnd, "��ǰ�����Ϸ�", "���", MB_OK);
}
//��ʾ����
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
//validate������������x[i][j]�Ƿ�Ϊ���е�ֵ
int validate(int x[9][9], int i, int j, int num)
{
	int row, column;
	int isPossible = 1;
	//������i�в�ѯ�Ƿ����ظ���ֵ
	for (column = 0; column < 9; column++)
	{
		if (num == x[i][column]) isPossible *= 0;
		else isPossible *= 1;
	}
	//������j�в�ѯ�Ƿ����ظ���ֵ
	for (row = 0; row < 9; row++)
	{
		if (num == x[row][j]) isPossible *= 0;
		else isPossible *= 1;
	}
	//����x[i][i]���ڹ�(box)��ѯ�Ƿ����ظ���ֵ
	int box_i, box_j;
	//��λ����λ��
	int offset_x, offset_y;
	//����x����(��j������ӷ���)ƫ����
	offset_x = j - j % 3;
	//����y����(��i������ӷ���)ƫ����
	offset_y = i - i % 3;
	for (box_i = 0; box_i < 3; box_i++)
	{
		for (box_j = 0; box_j < 3; box_j++)
		{
			//��box_i,box_j��������ȷ�ľŹ���
			if (num == x[box_i + offset_y][box_j + offset_x]) isPossible *= 0;
			else isPossible *= 1;
		}
	}
	//��֤���󷵻�1�����򷵻�0
	return isPossible;
}
//�����Ƿ�ɽ��ж�
int  judgeanswer(HWND hnd, int i, int j)
{
	if (times[i][j] > 20)
	{
		MessageBox(hnd, "�������޽�", "����", MB_OK);
		return 1;
	}
	else return 0;
}
//solve_soduku�������Ψһ��
void solve_soduku(int x[9][9], HWND hnd)
{
	int i, j, num, flag = 0, m, n;
	for (i = 0; i < 9; i++)	//���ҵ���һ����Ҫ����� ����֮���ж������޽�
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
	flag = 0;//flag ��ʼ��
	m = i;	
	n = j;
	//��ʼ��i��j
	i = 0;
	j = 0;
	//�����Ź��񣬴�x[0][0]��ʼ����
	while (i < 9)
	{
		if (log)
			break;
		while (j < 9)
		{
			//�޽��˳�
			if (log)
				break;
			//�ж��Ƿ�Ϊ��һ���Դ���ת����
		start:if (flag == 1 && j == -1)
		{
			i--;
			j = 8;
			goto start;
		}
		//�ж��Ƿ�Ϊ�Դ���ת�����ҵ�ǰֵ�����޸�
		else if (flag == 1 && origin[i][j] == 1)
		{
			//����������һ��
			j--;
			continue;
		}
		//�жϵ�ǰ�����Ƿ񲻿��޸�
		else if (origin[i][j] == 1)
		{
			j++;
			continue;
		}
		//��ʼ����
		else
		{
			if (i == m && j == n)
			{
				times[m][n]++;
				if (judgeanswer(hnd, m, n))
					log = 1;
			}
			num = x[i][j];
			//����ǰ���ӻ�δ��ֵ������������һ���Դ���ת��������1��ʼ����
			if (num == 0) num = 1;
			//��flag==1��������һ���Դ����������һ�δ����ֵ+1��������
			else if (flag == 1) num = x[i][j] + 1;
			//��ʼ����ת��ʶ��ֵ
			flag = 0;
			//��num��ʼ����������
			for (; num <= 10; num++)
			{
				//��validate�����жϳ��Ե�ֵ�Ƿ����
				//������н���ǰ���ӵ�ֵ��Ϊnum
				if (validate(x, i, j, num) == 1 && num < 10)
				{
					x[i][j] = num;
					j++;
					break;
				}
				//�����9��û�п��е�ֵ���룬˵��ǰ���д���ʼ��ǰֵΪ0��������һ�γ���
				else if (num == 10)
				{
					x[i][j] = 0;
					j--;
					//������ת��ʶflag=1
					flag = 1;
					break;
				}
			}
		}
		}
		//һ����������һ�е�һ����������
		j = 0;
		i++;
	}
}
//���찴ť
void button(int x, int y, int w, int h, const char str[])
{
	//���ư�ť
	setfillcolor(RGB(142, 123, 99));
	fillroundrect(x, y, x + w, y + h, 5, 5);
	settextcolor(RGB(248, 242, 243));
	settextstyle(30, 0, "΢���ź�");
	setbkmode(0);
	outtextxy(x + (w - textwidth(str)) / 2, y + (h - textheight(str)) / 2, str);
}
//����С��
void draww(int x, int y)
{
	int i, j;
	for (i = 1; i <= 3; i++)//��
	{
		for (j = 1; j <= 3; j++)	//��
		{
			setfillcolor(RGB(187, 172, 160));
			fillrectangle(x + SGAP_WID * j + S_WID * (j - 1), y + SGAP_WID * i + S_WID * (i - 1), x + SGAP_WID * j + S_WID * j, y + SGAP_WID * i + S_WID * i);
		}
	}
}
//��������
void build()
{
	//���Ƶ׿�
	setfillcolor(RGB(189, 172, 160));
	fillrectangle(GAP, GAP, GAP + L_WID, GAP + L_WID);
	//�����п�
	int i, j;
	for (i = 1; i <= 3; i++)//��
	{
		for (j = 1; j <= 3; j++)	//��
		{
			setfillcolor(RGB(205, 193, 181));
			fillrectangle(GAP + MGAP_WID * j + M_WID * (j - 1), GAP + MGAP_WID * i + M_WID * (i - 1), GAP + MGAP_WID * j + M_WID * j, GAP + MGAP_WID * i + M_WID * i);
			draww(GAP + MGAP_WID * j + M_WID * (j - 1), GAP + MGAP_WID * i + M_WID * (i - 1));
		}
	}
}
//ѡ����ƶ�����
void move(int n)
{
	switch (n)
	{
	case 72://����
		if (player.y > GAP + MGAP_WID + SGAP_WID)
		{
			player.i--;
			player.y -= S_WID + SGAP_WID;
			if (player.y == GAP + L_WID - (MGAP_WID + 4 * (S_WID + SGAP_WID))
				|| player.y == GAP + L_WID - (2 * MGAP_WID + SGAP_WID + 7 * (S_WID + SGAP_WID)))
				player.y -= MGAP_WID + SGAP_WID;
		}
		break;
	case 80://����
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
	case 75://����
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
	case 77://����
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
//����������ֺ���
void fillnum()
{
	char temp[20];
	int i, j;//i �� j ��
	int x, y;
	for (i = 0; i <= 8; i++)
	{
		for (j = 0; j <= 8; j++)
		{
			if (soduku[i][j] != 0)//�����0����ʾ
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
					settextcolor(RGB(119, 109, 101));	//��ɫ���ԭʼ����
				else if (tip[i][j] == 1)
					settextcolor(RGB(236, 38, 114));	//��ɫ�����ʾ���ַ�
				else
					settextcolor(WHITE);
				setbkmode(TRANSPARENT);
				settextstyle(40, 0, "Arial");
				sprintf_s(temp, "%d", soduku[i][j]);	//�ܴ�ӡ����������Ҫת��һ�£�����ᰴ��ASCII��
				outtextxy(x + (S_WID - textwidth(temp)) / 2, y + 5, temp);
			}
		}
	}
}
//��ͼ����
void draw()
{
	//˫�����ͼ
	BeginBatchDraw();
	//���ñ���ɫ
	setbkcolor(RGB(251, 248, 240));
	cleardevice();
	//��������	
	build();
	//���ư�ť
	button(GAP + L_WID + 5 * GAP, L_WID / 3, M_WID, M_WID / 3, "��ʾ");
	button(GAP + L_WID + 5 * GAP, M_WID + L_WID / 3, M_WID, M_WID / 3, "���");
	//����ѡ����
	setfillcolor(RGB(246, 124, 94));
	fillrectangle(player.x, player.y, player.x + S_WID, player.y + S_WID);
	//��������
	fillnum();
	//�����ı���ʾ
	settextstyle(30, 0, "΢���ź�");
	setbkmode(TRANSPARENT);
	settextcolor(RGB(116, 103, 94));
	outtextxy(GAP + L_WID + S_WID, GAP, "ͨ�����̡�����������ѡ�����ƶ�");
	outtextxy(GAP + L_WID + S_WID, GAP + 40, "�������ָı�������� 0�ǿ�ȱ");
	outtextxy(GAP + L_WID + S_WID, GAP + 80, "�������ť����ʾ����");
	outtextxy(GAP + L_WID + S_WID, GAP + 120, "���߰�R��� T��ʾ");
	//���������ͼ
	EndBatchDraw();
}
//��������
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
//������������ж�
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
		MessageBoxA(hnd, "��ϲ������������", "�ɹ�", MB_OK);
	}
	return flag;
}
//������
int main()
{
	int i, j;
	char temp;
	HWND hnd = GetHWnd();	//��ȡ���
	ExMessage msg;	//������ȡ�����Ϣ
	printf("��������Ҫ��������:\n");
	printf("ע��������ȱӦ��Ϊ0\n");
	printf("  123456789\n");
	//��������
	for (i = 0; i < 9; i++)
	{
		printf("%d:", i + 1);
		for (j = 0; j < 10; j++)
		{
			temp = getchar();
			if (j < 9) soduku[i][j] = temp - '0';
		}
	}
	initsoduku();//��¼��ʼ����
	solve_soduku(answer, hnd);//����������� �ж����������Ƿ��н�
	if (!log)//�����н���ܽ����������Ϸ
	{
		//��ͼ��ʼ��
		initgraph(960, 2 * GAP + L_WID, 0);
		hnd = GetHWnd();//��ȡ���ھ��
		while (1)
		{
			//��ͼ
			draw();
			//��¼������Ϣ
			if (_kbhit())
			{
				int kb = _getch();
				move(kb);
				insertnum(kb);
				if(kb == 116)
					if (MessageBox(hnd, "ȷ��Ҫʹ����ʾ��", "��ʾ", MB_OKCANCEL) == 1)//ok����ֵΪ1 cancel��Ϊ2
					{
						tips();
					}
				if(kb == 114)
					if(MessageBox(hnd, "ȷ��Ҫʹ�ü����", "���", MB_OKCANCEL) == 1)
				{
					valid(hnd);
				}
			}
			//��¼�����Ϣ
			if (peekmessage(&msg, EM_MOUSE))//�㰴ť
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					if (msg.x >= GAP + L_WID + 5 * GAP && msg.x <= GAP + L_WID + 5 * GAP + M_WID
						&& msg.y >= L_WID / 3 && msg.y <= L_WID / 3 + M_WID / 3)//��������ʾ��ť
					{
						if (MessageBox(hnd, "ȷ��Ҫʹ����ʾ��", "��ʾ", MB_OKCANCEL) == 1)//ok����ֵΪ1 cancel��Ϊ2
						{
							tips();
						}
					}
					if (msg.x >= GAP + L_WID + 5 * GAP && msg.x <= GAP + L_WID + 5 * GAP + M_WID
						&& msg.y >= M_WID + L_WID / 3 && msg.y <= M_WID + L_WID / 3 + M_WID / 3)//�����˼�鰴ť
					{
						if (MessageBox(hnd, "ȷ��Ҫʹ�ü����", "���", MB_OKCANCEL) == 1)
						{
							valid(hnd);
						}
					}
				}
			}
			//�ж��Ƿ�ɹ�
			if (judgewin(hnd))
				break;
		}
	}
	return 0;
}