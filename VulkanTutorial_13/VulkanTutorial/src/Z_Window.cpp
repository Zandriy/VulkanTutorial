/* Z_Window.cpp
 * VulkanTutorial project
 *
 * Created by Andriy Zhabura on 08-Oct-2016.
 * Last modified on 08-Oct-2016.
 */

/*
 * Copyright © 2016 Andriy Zhabura
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */
 
#include "Z_Window.h"
#include <stdexcept>

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        return 0;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

Z_Window::Z_Window(const wchar_t* name, int width, int height)
    : app_module(NULL)
    , hWnd(NULL)
    , wnd_width(width)
    , wnd_height(height)
{
    if (!create_window(name))
    {
        throw std::domain_error{"Window cannot be created"};
    }
}

Z_Window::~Z_Window()
{

}

bool Z_Window::create_window(const wchar_t* name)
{
    if (wnd_width <= 0 || wnd_height <= 0)
    {
        return false;
    }

    app_module = GetModuleHandle(NULL);
    if (app_module == NULL)
    {
        return false;
    }

    // Initialize the window class structure:
    WNDCLASSEX win_class;
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = app_module;
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = LPCWSTR(name);
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        return false;
    }
    // Create window with the registered class:
    RECT wr = { 0, 0, wnd_width, wnd_height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    hWnd = CreateWindowEx(0,
        LPCWSTR(name),              // class name
        LPCWSTR(name),              // app name
        WS_OVERLAPPEDWINDOW |       // window style
        WS_VISIBLE | WS_SYSMENU,
        150, 150,                   // x/y coords
        wr.right - wr.left,         // width
        wr.bottom - wr.top,         // height
        NULL,                       // handle to parent
        NULL,                       // handle to menu
        app_module,                 // hInstance
        NULL);                      // no extra parameters

    return hWnd != NULL;
}

int Z_Window::width()
{
    return wnd_width;
}

int Z_Window::height()
{
    return wnd_height;
}