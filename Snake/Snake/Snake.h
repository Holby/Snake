#pragma once
#include "Graphics.hpp"
#include <Windows.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>     
#include <stdlib.h>   
#include <time.h> 
#include <string>
#pragma comment(lib, "winmm.lib")

using namespace std;

struct player
{
	float score = 0;
	string name;

	string strscore()
	{
		return to_string(score);
	}
};

player hiscore[5];
player newplayer;
char level[25][25];
int posY[250];
int posX[250];
int mode = 1;

int snakelen = 3;

const int ID_QUIT = 1;
const int ID_NEW = 2;
const int ID_HISCORE = 3;

const int ID_UP = 1;
const int ID_RIGHT = 2;
const int ID_DOWN = 3;
const int ID_LEFT = 4;

int next_move = 2;
float gametime = 0;
int stage = 1;

Bitmap map1;
Bitmap snakebm;
Bitmap food;
Bitmap splash;

HWND myButton = 0;
HWND myEditBox = 0;

const int MyButtonID = 1;
const int MyEditBoxID = 2;


void gameover(HWND);
void levelup(HWND);
void startgame(HWND);
void gametick(HWND);
void drawsplash(HWND);
void drawgame(HWND);
void printhiscore(HWND);

void bubbleSort(player hiscore[5]);