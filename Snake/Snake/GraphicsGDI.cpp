#include "Graphics.hpp"
#include <Windows.h>
#include <vector>

using namespace std;

namespace
{
	HWND window;
	int windowWidth;
	int windowHeight;

	HDC backbufferDC;
	HBITMAP backbufferBitmap;

	HDC bitmapDC;

	struct GDIBitmap
	{
		HBITMAP handle;
		int width;
		int height;

		GDIBitmap()
			: handle(0)
		{
		}
	};

	vector<GDIBitmap> bitmaps;
}

void InitializeGraphics(void* window)
{
	// spara undan f�nsterhandtaget. anledningen att den �r void* �r f�r att
	// slippa #include <Windows.h> i headern
	::window = (HWND)window;

	// skapa en DC f�r backbuffern som har samma format som f�nstret
	HDC hdc = GetDC(::window);
	backbufferDC = CreateCompatibleDC(hdc);

	// ta reda p� hur stort f�nstret �r
	RECT windowSize = { 0 };
	GetClientRect(::window, &windowSize);

	windowWidth = windowSize.right - windowSize.left;
	windowHeight = windowSize.bottom - windowSize.top;

	// skapa en minnesbuffer f�r dubbelbuffring
	backbufferBitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
	SelectObject(backbufferDC, backbufferBitmap);

	// s�tt textbakgrunden genomskinlig
	SetBkMode(backbufferDC, TRANSPARENT);

	// skapa en DC f�r utritning av bilder
	bitmapDC = CreateCompatibleDC(hdc);
}

bool LoadBitmapFromFile(const char* filename, Bitmap& bitmap)
{
	// ladda bitmapen
	GDIBitmap gdi;
	gdi.handle = (HBITMAP)LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (gdi.handle == 0)
	{
		return false;
	}

	// ta reda p� storleken
	BITMAP header;
	GetObject(gdi.handle, sizeof(header), &header);
	gdi.width = header.bmWidth;
	gdi.height = header.bmHeight;

	// spara undan bitmap-informationen och returnera ett index
	// det "skyddar" spelet/applikationen fr�n implementationen, vilket
	// g�r det enkelt att byta till en annan API som OpenGL eller Direct3D
	bitmaps.push_back(gdi);

	bitmap.index = bitmaps.size() - 1;
	return true;
}

void BeginGraphics()
{
	// t�m inneh�llet i minnes-buffern
	RECT rectangle = { 0, 0, windowWidth, windowHeight };
	FillRect(backbufferDC, &rectangle, (HBRUSH)COLOR_BACKGROUND);
}

void DrawBitmap(Bitmap bitmap, int x, int y)
{
	// rita ut bitmappen p� backbuffern
	GDIBitmap& gdi = bitmaps[bitmap.index];
	SelectObject(bitmapDC, gdi.handle);
	BitBlt(backbufferDC, x, y, gdi.width, gdi.height, bitmapDC, 0, 0, SRCCOPY);
}

void DrawString(const char* text, int x, int y)
{
	// rita ut text p� backbuffern
	TextOut(backbufferDC, x, y, text, strlen(text));
}

void EndGraphics()
{
	// kopiera inneh�llet i minnesbuffern till sk�rmen
	HDC windowDC = GetDC(::window);
	BitBlt(windowDC, 0, 0, windowWidth, windowHeight, backbufferDC, 0, 0, SRCCOPY);
	ReleaseDC(::window, windowDC);
}

void FreeBitmap(Bitmap bitmap)
{
	// fria minnet som upptas av en bitmap
	if (bitmaps[bitmap.index].handle != 0)
	{
		DeleteObject(bitmaps[bitmap.index].handle);
		bitmaps[bitmap.index].handle = 0;
	}
}

void ShutdownGraphics()
{
	// fria alla resurser som anv�nds av grafiksystemet
	DeleteDC(bitmapDC);

	DeleteObject(backbufferBitmap);
	DeleteDC(backbufferDC);
}

