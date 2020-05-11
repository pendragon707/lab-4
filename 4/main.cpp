#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>
#include <string>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static const TCHAR classname[] = TEXT("Lab4");
static const TCHAR title[] = TEXT("Lab4");

HBRUSH bg_brush = 0;
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nShowCmd;

  GdiplusStartupInput gdiplusStartupInput;
  gdiplusStartupInput.GdiplusVersion = 1;

  ULONG_PTR gdiplusToken;
  if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, 0) != Ok) return 1;

  bg_brush = (HBRUSH)CreateSolidBrush(RGB(72, 72, 72));

  WNDCLASS wc;
  // Определение класса окна
  wc.hInstance = hInst;
  wc.lpszClassName = classname; 				// Имя класса окна
  wc.lpfnWndProc = WndProc; 					// Функция окна
  wc.style = CS_HREDRAW | CS_VREDRAW; 			// Стиль окна
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); 		// Стандартная иконка
  wc.hCursor = LoadCursor(NULL, IDC_ARROW); 		// Стандартный курсор
  wc.hbrBackground = bg_brush;
  wc.lpszMenuName = 0; 					// Нет меню
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  if (!RegisterClass(&wc))
  {
    DeleteObject(wc.hbrBackground);
    GdiplusShutdown(gdiplusToken);
    return 1;
  }

  HWND hwnd = CreateWindowEx(0, classname, title,
    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 480, 0, 0, 0, 0);
  if (!hwnd)
  {
    DeleteObject(wc.hbrBackground);
    GdiplusShutdown(gdiplusToken);
    UnregisterClass(classname, hInst);
    return 1;
  }
  ShowWindow(hwnd, SW_SHOWNORMAL);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DeleteObject(wc.hbrBackground);
  GdiplusShutdown(gdiplusToken);
  UnregisterClass(classname, hInst);
  return 0;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}