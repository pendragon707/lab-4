#pragma once

#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>
#include <string>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI circle_thread_proc(void*);
DWORD WINAPI text_thread_proc(void*);

static const TCHAR classname[] = TEXT("Lab4");
static const TCHAR title[] = TEXT("Lab4");

int window_wid, window_hei;

HANDLE circle_thread, circle_start_event;
HANDLE text_thread, text_start_event, text_end_event;

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
	  case WM_CREATE:
    {
      circle_start_event = CreateEvent(0, FALSE, FALSE, TEXT("CircleStartEvent"));
      circle_thread = CreateThread(0, 0, circle_thread_proc, 0, 0, 0);

      text_start_event = CreateEvent(0, FALSE, FALSE, TEXT("TextStartEvent"));
      text_end_event = CreateEvent(0, FALSE, FALSE, TEXT("TextEndEvent"));
      text_thread = CreateThread(0, 0, text_thread_proc, 0, 0, 0);

      SetTimer(hWnd, 0, 33, 0);
      break;
    }
	 case WM_TIMER:
    {
      InvalidateRect(hWnd, 0, FALSE);
      break;
    }
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

DWORD WINAPI circle_thread_proc(void*)
{
  for (;;)
  {
    WaitForSingleObject(circle_start_event, INFINITE);

    SetEvent(text_start_event);
  }
}

DWORD WINAPI text_thread_proc(void*)
{
  for (;;)
  {
    WaitForSingleObject(text_start_event, INFINITE);

    SetEvent(text_end_event);
  }
}

