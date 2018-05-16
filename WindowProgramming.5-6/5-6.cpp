#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <atlimage.h>
#include <math.h>

#define windowX 750
#define windowY 750

HINSTANCE g_hInst;
LPCTSTR lpszClass = (L"Window Class Name");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

struct map {
	int x;
	int y;
	int index = -1;
	BOOL check = FALSE;
};

struct Block {
	int x;
	int y;
	int xpos;
	int ypos;
	int rotation;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //메인
{
	srand((unsigned int)time(NULL));
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	hWnd = CreateWindow(lpszClass, L"window program", WS_OVERLAPPEDWINDOW, 0, 0, windowX, windowY, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

const int bs = 50;
POINT connected[100] = { 0 };
static map Map[10][14];
static int connectedLine = 0;
BOOL stuckWall = FALSE;

BOOL isConnected(int dir,int x,int y) // 0왼 1위 2우 3아래 // rotation 
{
	BOOL c;
	connectedLine++;
	switch (dir)
	{
	case 0:
		if (x > 0 && Map[x - 1][y].check && (Map[x - 1][y].index == 1 || Map[x - 1][y].index == 3 || Map[x - 1][y].index == 4))
		{
			connected[connectedLine].x = x - 1;
			connected[connectedLine].y = y;
			if (connected[0].x == connected[connectedLine].x && connected[0].y == connected[connectedLine].y)
			{
				return TRUE;
			}
			switch (Map[x - 1][y].index) //imageIndex
			{
			case 1:
				c = isConnected(0, x - 1, y);
				break;
			case 3:
				c = isConnected(1, x - 1, y);
				break;
			case 4:
				c = isConnected(3, x - 1, y);
				break;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 1:
		if (y > 0 && Map[x][y-1].check && (Map[x][y-1].index == 2 || Map[x][y-1].index == 4 || Map[x][y-1].index == 5))
		{
			connected[connectedLine].x = x;
			connected[connectedLine].y = y - 1;
			if (connected[0].x == connected[connectedLine].x && connected[0].y == connected[connectedLine].y)
			{
				return TRUE;
			}
			switch (Map[x][y-1].index) //imageIndex
			{
			case 2:
				c = isConnected(1, x, y - 1);
				break;
			case 4:
				c = isConnected(2, x, y - 1);
				break;
			case 5:
				c = isConnected(0, x, y - 1);
				break;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 2:
		if (x < 9 && Map[x + 1][y].check && (Map[x + 1][y].index == 1 || Map[x + 1][y].index == 0 || Map[x + 1][y].index == 5))
		{
			connected[connectedLine].x = x + 1;
			connected[connectedLine].y = y;
			if (connected[0].x == connected[connectedLine].x && connected[0].y == connected[connectedLine].y)
			{
				return TRUE;
			}
			switch (Map[x + 1][y].index) //imageIndex
			{
			case 1:
				c = isConnected(2, x + 1, y);
				break;
			case 0:
				c = isConnected(1, x + 1, y);
				break;
			case 5:
				c = isConnected(3, x + 1, y);
				break;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	case 3:
		if (y < 13 && Map[x][y + 1].check && (Map[x][y +1].index == 2 || Map[x][y +1].index == 0 || Map[x][y + 1].index == 3))
		{
			connected[connectedLine].x = x;
			connected[connectedLine].y = y + 1;
			if (connected[0].x == connected[connectedLine].x && connected[0].y == connected[connectedLine].y)
			{
				return TRUE;
			}
			switch (Map[x][y + 1].index) //imageIndex
			{
			case 2:
				c = isConnected(3, x, y + 1);
				break;
			case 0:
				c = isConnected(0, x, y + 1);
				break;
			case 3:
				c = isConnected(2, x, y + 1);
				break;
			}
		}
		else
		{
			return FALSE;
		}
		break;
	}

	return c;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, memDC, mem2DC;
	static HBITMAP hBitmap, h2Bitmap, oldBit1, oldBit2;
	static RECT clientRECT;

	HBITMAP blocks[6];
	static int x = 367;
	static Block dropBlock;
	static int xpos = 5;
	static int ypos = 0;
	static int y = 0;
	static int count = 0;
	static int nextBlock;
	static int Mapcount[20] = { 0 };
	switch (iMessage)
	{
	case WM_CREATE:
		dropBlock.x = x;
		dropBlock.y = y;
		dropBlock.xpos = 5;
		dropBlock.ypos = 0;
		dropBlock.rotation = rand() % 6;
		nextBlock = rand() % 6;
		for (int i = 0; i < 14; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				Map[j][i].x = 100 + (i * bs);
				Map[j][i].y = 100 + (j * bs);
			}
		}

		GetClientRect(hWnd, &clientRECT);
		SetTimer(hWnd, 1, 500, NULL);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hDC);
		mem2DC = CreateCompatibleDC(memDC);
		hBitmap = CreateCompatibleBitmap(hDC, clientRECT.right, clientRECT.bottom);

		blocks[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(101));
		blocks[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(102));
		blocks[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(103));
		blocks[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(104));
		blocks[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(105));
		blocks[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(106));

		oldBit1 = (HBITMAP)SelectObject(memDC, hBitmap);
		oldBit2 = (HBITMAP)SelectObject(mem2DC, NULL);
		//---------------------------------------
		Rectangle(memDC, clientRECT.right / 2 - 250, 0, clientRECT.right / 2 + 250, clientRECT.bottom);
		Rectangle(memDC, clientRECT.right / 2 + 280, 30, clientRECT.right / 2 + 330, 80);
		for (int i = 0; i < 10; i++)
		{
			MoveToEx(memDC, clientRECT.right / 2 - 250 + bs * (i + 1), 0, NULL);
			LineTo(memDC, clientRECT.right / 2 - 250 + bs * (i + 1), clientRECT.bottom);
		}
		for (int i = 0; i < 14; i++)
		{
			MoveToEx(memDC, clientRECT.right / 2 - 250, bs * (i + 1), NULL);
			LineTo(memDC, clientRECT.right / 2 + 250, bs * (i + 1));
		}
		//---------------------------------------
		(HBITMAP)SelectObject(mem2DC, blocks[dropBlock.rotation]);
		TransparentBlt(memDC, dropBlock.x, dropBlock.y, bs, bs, mem2DC, 0, 0, bs, bs, RGB(255, 255, 255));
		SelectObject(mem2DC, NULL);
		(HBITMAP)SelectObject(mem2DC, blocks[nextBlock]);
		TransparentBlt(memDC, clientRECT.right / 2 + 280, 30, bs, bs, mem2DC, 0, 0, bs, bs, RGB(255, 255, 255));
		SelectObject(mem2DC, NULL);
		//----------------
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 14; ++j)
			{
				if (Map[i][j].check == 1)
				{
					(HBITMAP)SelectObject(mem2DC, blocks[Map[i][j].index]);
					TransparentBlt(memDC, clientRECT.right / 2 - 250 + bs * i, bs * j, bs, bs, mem2DC, 0, 0, bs, bs, RGB(255, 255, 255));
					SelectObject(mem2DC, NULL);
				}
			}
		}
		BitBlt(hDC, 0, 0, clientRECT.right, clientRECT.bottom, memDC, 0, 0, SRCCOPY);

		SelectObject(memDC, oldBit1);
		for (int i = 0; i < 6; ++i)
		{
			DeleteObject(blocks[i]);
		}
		DeleteObject(hBitmap);
		DeleteDC(memDC);
		DeleteDC(mem2DC);
		DeleteObject(h2Bitmap);

		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			if (Map[dropBlock.xpos - 1][dropBlock.ypos].check)
				break;
			if (dropBlock.xpos > 0)
			{
				dropBlock.x -= bs;
				dropBlock.xpos -= 1;
			}
			InvalidateRect(hWnd, NULL, false);
			break;
		case VK_RIGHT:
			if (Map[dropBlock.xpos + 1][dropBlock.ypos].check)
				break;
			if (dropBlock.xpos <= 8)
			{
				dropBlock.x += bs;
				dropBlock.xpos += 1;
				InvalidateRect(hWnd, NULL, false);
			}
			break;
		case VK_DOWN:
			if (dropBlock.ypos < 12 && Map[dropBlock.xpos][dropBlock.ypos+2].check != 1)
			{
				dropBlock.y += bs;
				dropBlock.ypos += 1;
				InvalidateRect(hWnd, NULL, false);
			}
			break;
		case VK_SPACE:
		{
			switch (dropBlock.rotation)
			{
			case 0:
				dropBlock.rotation = 3;
				break;
			case 1:
				dropBlock.rotation = 2;
				break;
			case 2:
				dropBlock.rotation = 1;
				break;
			case 3:
				dropBlock.rotation = 4;
				break;
			case 4:
				dropBlock.rotation = 5;
				break;
			case 5:
				dropBlock.rotation = 0;
				break;
			}
		}

		break;
		}
	case WM_CHAR:

		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			dropBlock.y += 50;
			dropBlock.ypos += 1;
			if (Map[dropBlock.xpos][dropBlock.ypos+1].check)
			{
				Map[dropBlock.xpos][dropBlock.ypos].check = TRUE;
				Map[dropBlock.xpos][dropBlock.ypos].index = dropBlock.rotation;
				switch (dropBlock.rotation)
				{
				case 0:
				case 5:
					for (int i = 0; i < 100; i++)
					{
						connected[i].x = 0;
						connected[i].y = 0;
						connectedLine = 0;
					}
					connected[0].x = dropBlock.xpos;
					connected[0].y = dropBlock.ypos;
					if (isConnected(0, dropBlock.xpos, dropBlock.ypos))
					{
						for (int i = 0; i < connectedLine; i++)
						{
							Map[connected[i].x][connected[i].y].check = 0;
							Map[connected[i].x][connected[i].y].index = 0;
						}
						for (int i = 9; i >= 0; i--)
						{
							for (int j = 12; j >= 0; j--)
							{
								while (j < 13 && Map[i][j].check == 1 && Map[i][j + 1].check == 0)
								{
									Map[i][j + 1].check = Map[i][j].check;
									Map[i][j].check = 0;
									Map[i][j + 1].index = Map[i][j].index;
									Map[i][j+1].x = Map[i][j].x;
									Map[i][j + 1].y = Map[i][j].y;
									j++;
									Sleep(10);
								}
							}
						}
					}
					break;
				case 1:
				case 3:
				case 4:
					for (int i = 0; i < 100; i++)
					{
						connected[i].x = 0;
						connected[i].y = 0;
						connectedLine = 0;
					}
					connected[0].x = dropBlock.xpos;
					connected[0].y = dropBlock.ypos;
					if (isConnected(2, dropBlock.xpos, dropBlock.ypos))
					{
						for (int i = 0; i < connectedLine; i++)
						{
							Map[connected[i].x][connected[i].y].check = 0;
							Map[connected[i].x][connected[i].y].index = 0;
						}
						for (int i = 9; i >= 0; i--)
						{
							for (int j = 12; j >= 0; j--)
							{
								while (j < 13 && Map[i][j].check == 1 && Map[i][j + 1].check == 0)
								{
									Map[i][j + 1].check = Map[i][j].check;
									Map[i][j].check = 0;
									Map[i][j + 1].index = Map[i][j].index;
									Map[i][j + 1].x = Map[i][j].x;
									Map[i][j + 1].y = Map[i][j].y;
									j++;
									Sleep(10);
								}
							}
						}
					}
					break;
				}
				dropBlock.x = x;
				dropBlock.y = y;
				dropBlock.xpos = 5;
				dropBlock.ypos = 0;
				dropBlock.rotation = nextBlock;
				nextBlock = rand() % 6;
			}
			if (dropBlock.ypos == 13)
			{
				Map[dropBlock.xpos][13].check = TRUE;
				Map[dropBlock.xpos][13].index = dropBlock.rotation;
				switch (dropBlock.rotation)
				{
				case 0:
				case 5:
					connected[0].x = dropBlock.xpos;
					connected[0].y = dropBlock.ypos;
					if (isConnected(0, dropBlock.xpos, dropBlock.ypos))
					{
						for (int i = 0; i < connectedLine; i++)
						{
							Map[connected[i].x][connected[i].y].check = 0;
							Map[connected[i].x][connected[i].y].index = 0;
						}
						connectedLine = 0;
					}
					break;
				case 1:
				case 3:
				case 4:
					connected[0].x = dropBlock.xpos;
					connected[0].y = dropBlock.ypos;
					if (isConnected(2, dropBlock.xpos, dropBlock.ypos))
					{
						for (int i = 0; i < connectedLine; i++)
						{
							Map[connected[i].x][connected[i].y].check = 0;
							Map[connected[i].x][connected[i].y].index = 0;
						}
						connectedLine = 0;
					}
					break;
				}
				dropBlock.x = x;
				dropBlock.y = y;
				dropBlock.xpos = 5;
				dropBlock.ypos = 0;
				dropBlock.rotation = nextBlock;
				nextBlock = rand() % 6;
			}

			InvalidateRect(hWnd, NULL, false);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		
	default:
		return (DefWindowProc(hWnd, iMessage, wParam, lParam));
		//case에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달
	}

}