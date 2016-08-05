#include "Snake.h"



LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM	lParam)
{
	HDC hdc = { 0 };
	PAINTSTRUCT ps = { 0 };

	switch (msg)
	{
	case WM_PAINT:
	{

		switch (mode)
		{
		case 1:
			drawsplash(hwnd);
		break;
		case 2:
			drawgame(hwnd);
		break;
		}
		EndGraphics();
		EndPaint(hwnd, &ps);
	}
	break;

	case WM_TIMER:
		gametick(hwnd);
	break;
	case WM_KEYDOWN:
	{
		InvalidateRect(hwnd, 0, false);
		switch (wParam)
		{
		case VK_UP:
			next_move = ID_UP;
			return 0;
		case VK_DOWN:
			next_move = ID_DOWN;
			return 0;
		case VK_LEFT:
			next_move = ID_LEFT;
			return 0;
		case VK_RIGHT:
			next_move = ID_RIGHT;
			return 0;
		case VK_SPACE:
			snakelen += 1;
			return 0; 
		case VK_RETURN:
			if (mode == 1)
				startgame(hwnd);
			return 0;
		default:
			break;
		}
		break;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		
		case ID_QUIT:
			FreeBitmap(map1);
			FreeBitmap(snakebm);
			FreeBitmap(food);
			FreeBitmap(splash);
			ShutdownGraphics();
			PostQuitMessage(0);
			return 0;
		case ID_NEW:
			startgame(hwnd);
			return 0;
		case ID_HISCORE:
		{
			printhiscore(hwnd);
			return 0;
		}
		}
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;


	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK WindowProc2(HWND hwndInput, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case MyButtonID:
		{
			char text[256] = { 0 };
			GetWindowText(myEditBox, text, 256);
			newplayer.name = text;
			SendMessage(hwndInput, WM_CLOSE, 0, 0);
		}
		return 0;
		}
		break;
	}

	return DefWindowProc(hwndInput, msg, wParam, lParam);
}

HMENU CreateMainMenu()
{
	HMENU main = CreateMenu();

	HMENU file = CreateMenu();
	AppendMenu(file, MF_STRING, 1, "&Quit");
	AppendMenu(file, MF_STRING, 2, "&New game");
	AppendMenu(file, MF_STRING, 3, "&Hi-Score");
	AppendMenu(main, MF_POPUP, (UINT_PTR)file, "&Menu");

	return main;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char className[] = { "MyWindowClass" };
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = 0;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = (HICON)LoadIcon(0, IDI_APPLICATION);
	windowClass.hIconSm = 0;
	windowClass.hCursor = (HCURSOR)LoadCursor(0, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;

	RegisterClassEx(&windowClass);

	HWND hwnd = CreateWindow(
		"MyWindowClass",
		"Snake",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		650, 560,
		0,
		CreateMainMenu(),
		hInstance, 0);

	srand(time(NULL));
	UpdateWindow(hwnd);
	ShowWindow(hwnd, nCmdShow);

	ifstream file("hiscore.txt");
	string temp;
	for (int i = 0; i < 5; i++)
	{
		getline(file, temp);
		getline(file, hiscore[i].name);
		hiscore[i].score = atof(temp.c_str());
	}

	HMENU	hMenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	

	WNDCLASS wcInput = { 0 };
	wcInput.lpfnWndProc = WindowProc2;
	wcInput.hInstance = hInstance;
	wcInput.lpszClassName = "MyInputClass";

	RegisterClass(&wcInput);

	HWND hwndInput = CreateWindow(
		"MyInputClass",
		"Controls",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		250, 120,
		0,
		0,
		hInstance, 0);

	HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	myButton = CreateWindow(
		"BUTTON",
		"Ok",
		WS_CHILD | WS_VISIBLE,
		80, 50,
		50, 28,
		hwndInput,
		(HMENU)MyButtonID,
		hInstance, 0);

	SendMessage(myButton, WM_SETFONT, (WPARAM)font, 0);

	myEditBox = CreateWindow(
		"EDIT",
		"Enter name...",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		60, 10,
		100, 28,
		hwndInput,
		(HMENU)MyEditBoxID,
		hInstance, 0);

	SendMessage(myEditBox, WM_SETFONT, (WPARAM)font, 0);

	UpdateWindow(hwndInput);
	ShowWindow(hwndInput, nCmdShow);
	hMenu = GetSystemMenu(hwndInput, FALSE);
	EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	


	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


void gameover(HWND hwnd)
{
	KillTimer(hwnd, 0);
	PlaySound("fail.wav", NULL, SND_FILENAME | SND_ASYNC);
	MessageBox(hwnd, "Game Over!", "Fail", MB_OK);
	mode = 1;
	InvalidateRect(hwnd, 0, false);
}

void levelup(HWND hwnd)
{
	if (stage == 1)
	{
		KillTimer(hwnd, 0);
		PlaySound("win.wav", NULL, SND_FILENAME | SND_ASYNC);
		MessageBox(hwnd, "Level Completed!", "Win!", MB_OK);
		snakelen = 3;
		SetTimer(hwnd, 0, 100, 0);
		fill(begin(posX), end(posX), 0);
		fill(begin(posY), end(posY), 0);
		posX[0] = 1;
		posY[0] = 1;
		next_move = ID_RIGHT;
		ifstream file("level2.txt");
		for (int j = 0; j < 25; j++)
		{
			for (int i = 0; i < 25; i++) {
				file >> level[j][i];
			}
		}
		file.close();
		stage += 1;
	}
	else
	{
		KillTimer(hwnd, 0);
		newplayer.score = gametime;
		PlaySound("win.wav", NULL, SND_FILENAME | SND_ASYNC);
		string temp = "Victory! Your time: " + to_string(round(gametime * 100) / 100.0) + "s";
		MessageBox(hwnd, temp.c_str(), "Win!", MB_OK);
		if (gametime < hiscore[4].score)
		{
			MessageBox(hwnd, "Hiscore!", "Win!", MB_OK);
			hiscore[4].name = newplayer.name;
			hiscore[4].score = newplayer.score;
			bubbleSort(hiscore);
			printhiscore(hwnd);
			ofstream file("hiscore.txt");
			for (int i = 0; i < 5; i++)
			{
				file << hiscore[i].strscore() + "\n";
				file << hiscore[i].name + "\n";
			}

		}
		mode = 1;
		InvalidateRect(hwnd, 0, false);
	}
}

void startgame(HWND hwnd)
{
	mode = 2;
	SetTimer(hwnd, 0, 100, 0);
	snakelen = 3;
	fill(begin(posX), end(posX), 0);
	fill(begin(posY), end(posY), 0);
	posY[0] = 1;
	posX[0] = 1;
	next_move = ID_RIGHT;

	ifstream file("level1.txt");
	for (int j = 0; j < 25; j++)
	{
		for (int i = 0; i < 25; i++) {
			file >> level[j][i];
		}
	}
	stage = 1;
	gametime = 0;

}
void gametick(HWND hwnd)
{
	gametime = gametime + 0.1;
	for (int i = snakelen - 1; i > 0; i--)
	{
		posY[i] = posY[i - 1];
		posX[i] = posX[i - 1];
	}
	switch (next_move)
	{
	case ID_UP:
		posY[0] -= 1;
		break;
	case ID_DOWN:
		posY[0] += 1;
		break;
	case ID_LEFT:
		posX[0] -= 1;
		break;
	case ID_RIGHT:
		posX[0] += 1;
		break;
	}

	for (int i = 1; i < snakelen; i++)		//krock? 
	{
		if ((posX[0] == posX[i]) && (posY[0] == posY[i]))
		{
			gameover(hwnd);
		}
	}

	if (level[posY[0]][posX[0]] == '1')
	{
		gameover(hwnd);
	}

	if (level[posY[0]][posX[0]] == 'f')
	{
		PlaySound("food.wav", NULL, SND_FILENAME | SND_ASYNC);
		level[posY[0]][posX[0]] = '0';
		int randx = rand() % 23 + 1;
		int randy = rand() % 23 + 1;
		while (level[randy][randx] == '1')
		{
			randx = rand() % 23 + 1;
			randy = rand() % 23 + 1;
		}
		level[randy][randx] = 'f';
		snakelen += 1;
	}

	if (snakelen == 33)
	{
		levelup(hwnd);
	}
	InvalidateRect(hwnd, 0, false);
}
void drawsplash(HWND hwnd)
{
	InitializeGraphics(hwnd);
	LoadBitmapFromFile("splash.bmp", splash);
	LoadBitmapFromFile("1.bmp", map1);
	LoadBitmapFromFile("snake.bmp", snakebm);
	LoadBitmapFromFile("food.bmp", food);
	BeginGraphics();
	DrawBitmap(splash, 0, 0);
}
void drawgame(HWND hwnd)
{
	BeginGraphics();
	DrawBitmap(map1, 0, 0);
	for (int i = 0; i < snakelen; i++)
	{
		DrawBitmap(snakebm, 20 * posX[i], 20 * posY[i]);
	}
	for (int j = 0; j < 25; j++)
	{
		for (int i = 0; i < 25; i++) {
			if (level[j][i] == '1')
			{
				DrawBitmap(snakebm, 20 * i, 20 * j);
			}
			else if (level[j][i] == 'f')
			{
				DrawBitmap(food, 20 * i, 20 * j);
			}
		}
	}
	string temp = "Food: " + to_string(snakelen - 3) + "/ 30";
	DrawString(temp.c_str(), 550, 10);
	DrawString(to_string(round(gametime * 100) / 100.0).c_str(), 550, 30);
}
void bubbleSort(player hiscore[5])
{
	player temp;
	bool done = true;

	do {
		done = true;
		for (int i = 0; i < 4; i++)
		{
			if (hiscore[i].score > hiscore[i + 1].score)
			{
				done = false;
				temp = hiscore[i];
				hiscore[i] = hiscore[i + 1];
				hiscore[i + 1] = temp;
			}
		}
	} while (done == false);
}
void printhiscore(HWND hwnd)
{
	string temp;
	for (int i = 0; i < 5; i++)
	{
		temp = temp + to_string(i + 1) + ". " + hiscore[i].strscore() + "         " + hiscore[i].name + "\n";
	}
	MessageBox(hwnd, temp.c_str(), "Hi-Score", MB_OK);
}