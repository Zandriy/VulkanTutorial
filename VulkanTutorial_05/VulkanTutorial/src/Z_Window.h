/* Z_Window.h
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
 
#ifndef Z_Window_h
#define Z_Window_h

#include <Windows.h>

class Z_Window
{
public:
    Z_Window(const wchar_t* name, int width, int height);
	~Z_Window();

    HINSTANCE module() const { return app_module; }
    HWND handle() const { return hWnd; }
    int width();
    int height();
private:
    bool create_window(const wchar_t* name);

	HINSTANCE app_module;
	HWND hWnd;
    int wnd_width{};
    int wnd_height{};
};

#endif // !Z_Window_h

