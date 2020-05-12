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
static const int circle_wid = 100;
static const int circle_hei = 100;

int window_wid, window_hei;
HDC window_memdc = 0;
HBITMAP window_bitmap = 0;

HANDLE circle_thread, circle_start_event;
HANDLE text_thread, text_start_event, text_end_event;

std::wstring echo;
Font* font = 0;
SolidBrush* text_brush = 0;
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

      font = new Font(FontFamily::GenericMonospace(), 40, FontStyleRegular, UnitPixel);
      text_brush = new SolidBrush(Color::White);

      SetTimer(hWnd, 0, 33, 0);
      break;
    }
	    case WM_SIZE:
    {
      window_wid = LOWORD(lParam);
      window_hei = HIWORD(lParam);

      if (window_memdc) DeleteDC(window_memdc);
      if (window_bitmap) DeleteObject(window_bitmap);

      const HDC dc = GetDC(hWnd);

      window_memdc = CreateCompatibleDC(dc);
      window_bitmap = CreateCompatibleBitmap(dc, window_wid, window_hei);
      SelectObject(window_memdc, window_bitmap);

      ReleaseDC(hWnd, dc);
      break;
    }
	 case WM_CHAR:
    {
      wchar_t c = (wchar_t)wParam;
      echo += c;
      break;
    }
	    case WM_PAINT:
    {
      SetEvent(circle_start_event);
      WaitForSingleObject(text_end_event, INFINITE);

      PAINTSTRUCT ps;
      const HDC hdc = BeginPaint(hWnd, &ps);

	  BitBlt(hdc, 0, 0, window_wid, window_hei, window_memdc, 0, 0, SRCCOPY);

      EndPaint(hWnd, &ps);
      return TRUE;
    }
	 case WM_TIMER:
    {
      InvalidateRect(hWnd, 0, FALSE);
      break;
    }
    case WM_DESTROY:
    {
	  TerminateThread(circle_thread, 0);
      TerminateThread(text_thread, 0);
	  
	  CloseHandle(circle_start_event);
      CloseHandle(text_start_event);
      CloseHandle(text_end_event);

      if (window_memdc) DeleteDC(window_memdc);
      if (window_bitmap) DeleteObject(window_bitmap);

	  if (font) delete font;
      if (text_brush) delete text_brush;

      PostQuitMessage(0);
      break;
    }
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

DWORD WINAPI circle_thread_proc(void*)
{
  int circle_x = 0;
  for (;;)
  {
    WaitForSingleObject(circle_start_event, INFINITE);

	 if (window_memdc)
     {
       if (circle_x + circle_wid > window_wid) circle_x = 0;
	 
       Graphics graphics(window_memdc);
       graphics.Clear(Color(72, 72, 72));
       SolidBrush b(Color(255, 0, 0));
       graphics.FillEllipse(&b, circle_x++, window_hei / 2 - circle_hei / 2, circle_wid, circle_hei);
     }

    SetEvent(text_start_event);
  }
}

DWORD WINAPI text_thread_proc(void*)
{
  for (;;)
  {
    WaitForSingleObject(text_start_event, INFINITE);

    if (window_memdc)
    {
      Graphics graphics(window_memdc);

      const wchar_t* str = echo.c_str();

      RectF box(0, 0, 0, 0);

      const size_t len = echo.length();
        graphics.MeasureString(str, len, font,
          RectF(0, 0, window_wid + 1.f, window_hei + 1.f), &box);

      graphics.DrawString(str, len, font,
        RectF(window_wid / 2 - box.Width / 2, window_hei / 2 - box.Height / 2, box.Width, box.Height),
        StringFormat::GenericDefault(),
        text_brush);
    }

    SetEvent(text_end_event);
  }
}

