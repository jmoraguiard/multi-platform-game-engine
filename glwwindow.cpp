#include "glwwindow.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "wglext.h"

#include "applicationManager.h"

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

GLWindow::GLWindow(HINSTANCE instance_handler) {
	instance_handler_ = instance_handler;
}

bool GLWindow::create(int width, int height, int bpp, bool fullscreen) {
	const char CLASS_NAME[]  = "Windows Window Class";

	window_rectangle_.left			= (long) 0;
	window_rectangle_.right			= (long) width;
	window_rectangle_.top			= (long) 0;
	window_rectangle_.bottom		= (long) height;

	window_class.cbSize				= sizeof(WNDCLASSEX);						// The size, in bytes, of this structure.
	window_class.style				= CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc		= GLWindow::StaticWindowProcess;	// A pointer to an application-defined function called the window procedure or "window proc." 
	window_class.cbClsExtra			= 0;
    window_class.cbWndExtra			= 0;
    window_class.hInstance			= instance_handler_;							// A handle to the instance that contains the window procedure for the class. 
    window_class.lpszClassName		= CLASS_NAME;										// A pointer to a null-terminated string or is an atom.
	window_class.hCursor			= LoadCursor(NULL, IDC_ARROW);		// A handle to the class cursor.
	window_class.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);		// A handle to a small icon that is associated with the window class.
	window_class.hIcon				= LoadIcon(NULL, IDI_APPLICATION);  // default icon
    window_class.hbrBackground		= NULL;                             // don't need background
    window_class.lpszMenuName		= NULL; 

    if(!RegisterClassEx(&window_class))// Registers a window class with the operating system for subsequent use in calls to the CreateWindow or CreateWindowEx function.
		return false;

	if(MessageBox(NULL, "Do you want to launch application at full screnn?", "Full Screen", MB_ICONINFORMATION | MB_OKCANCEL) != IDCANCEL){
		full_screen_ = true;

		DEVMODE device_mode_settings;

		memset(&device_mode_settings, 0, sizeof(device_mode_settings));
		device_mode_settings.dmSize = sizeof(device_mode_settings);

		device_mode_settings.dmPelsWidth	= width;	// Setting device mode width to specified width
		device_mode_settings.dmPelsHeight	= height;	// Setting device mode height to specified height
		device_mode_settings.dmBitsPerPel	= bpp;		// Setting device bits per pixel
		device_mode_settings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&device_mode_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) { 
			// Setting device mode for full screen has failed, switch to windowed
			MessageBox(NULL, "Display mode failed", "Error", MB_ICONERROR | MB_OK);
			full_screen_ = false;
		}
		// ChangeDisplaySettings(0, 0) -> for back to normal windowed screen
	}else{
		full_screen_ = false;
	}

	if(full_screen_) {
		window_extended_style_ = WS_EX_APPWINDOW;
		window_style_ = WS_POPUP;
		ShowCursor(false);
	} else {
		window_extended_style_ = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		window_style_ = WS_OVERLAPPEDWINDOW;
	}

	if(!AdjustWindowRectEx(&window_rectangle_, window_style_, false, window_extended_style_))
		return false;
    
    HWND window_handler = CreateWindowEx(						// Creates an overlapped, pop-up, or child window with an extended window style.
        NULL,													// Optional window styles.
        CLASS_NAME,												// Window class
        "Learn to Program Windows",								// Window text
		window_style_,											// Window style
        0, 0,													// Size and position
		window_rectangle_.right - window_rectangle_.left,
		window_rectangle_.bottom - window_rectangle_.top,
        NULL,													// Parent window    
        NULL,													// Menu
        instance_handler_,										// Instance handle
        this													// Additional application data
        );

    if (window_handler == NULL)
		return false;

	ShowWindow(window_handler, SW_SHOW);
	UpdateWindow(window_handler_); // update the window

	return true;
}

void GLWindow::destroy() 
{
	if (full_screen_) {
        ChangeDisplaySettings(NULL, 0);         // If So Switch Back To The Desktop
        ShowCursor(true);                       // Show Mouse Pointer
    }
}

bool GLWindow::isRunning()
{
    return is_running_;
}

void GLWindow::processEvents()
{
    MSG msg;

    //While there are messages in the queue, store them in msg
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        //Process the messages one-by-one
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void GLWindow::attachApplicationManager(ApplicationManager* game_manager)
{
	game_manager_ = game_manager;
}

void GLWindow::OnResize(int width, int height) {
	getAttachedApplicationManager()->onResize(width, height);
}

void GLWindow::setupPixelFormat(void) {
    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd =
    {   
        sizeof(PIXELFORMATDESCRIPTOR),  // size
            1,                          // version
            PFD_SUPPORT_OPENGL |        // OpenGL window
            PFD_DRAW_TO_WINDOW |        // render to window
            PFD_DOUBLEBUFFER,           // support double-buffering
            PFD_TYPE_RGBA,              // color type
            32,                         // prefered color depth
            0, 0, 0, 0, 0, 0,           // color bits (ignored)
            0,                          // no alpha buffer
            0,                          // alpha bits (ignored)
            0,                          // no accumulation buffer
            0, 0, 0, 0,                 // accum bits (ignored)
            16,                         // depth buffer
            0,                          // no stencil buffer
            0,                          // no auxiliary buffers
            PFD_MAIN_PLANE,             // main layer
            0,                          // reserved
            0, 0, 0,                    // no layer, visible, damage masks
    };

    pixelFormat = ChoosePixelFormat(device_context_handler_, &pfd);
    SetPixelFormat(device_context_handler_, pixelFormat, &pfd);
}

void GLWindow::swapBuffers() { 
	SwapBuffers(device_context_handler_); 
}

LRESULT GLWindow::WindowProcess(HWND window_handler, UINT message_code, WPARAM w_additional_data, LPARAM l_additional_data) {
    switch (message_code) {
		case WM_CREATE:
		{
			device_context_handler_ = GetDC(window_handler);
			setupPixelFormat();

			//Set the version that we want, in this case 3.0
			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			0}; //zero indicates the end of the array

			//Create temporary context so we can get a pointer to the function
			HGLRC tmpContext = wglCreateContext(device_context_handler_);
			//Make it current
			wglMakeCurrent(device_context_handler_, tmpContext);

			//Get the function pointer
			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

			//If this is NULL then OpenGL 3.0 is not supported
			if (!wglCreateContextAttribsARB)
			{
				MessageBox(window_handler, "OpenGL 3.0 is not supported, falling back to GL 2.1", "An error occurred", MB_ICONERROR|MB_OK);
				rendering_context_handler_ = tmpContext;
			} else {
				// Create an OpenGL 3.0 context using the new function
				rendering_context_handler_ = wglCreateContextAttribsARB(device_context_handler_, 0, attribs);
				//Delete the temporary context
				wglDeleteContext(tmpContext);
			}

			//Make the GL3 context current
			wglMakeCurrent(device_context_handler_, rendering_context_handler_);

			is_running_ = true; //Mark our window as running
		}
		break;
		case WM_CLOSE:
		{
			if (MessageBox(window_handler, "Really quit?", "My application", MB_OKCANCEL) == IDOK)
				DestroyWindow(window_handler);
			else
				return 0;
		}
		break;
		case WM_DESTROY:
		{
			wglMakeCurrent(device_context_handler_, NULL);
			wglDeleteContext(rendering_context_handler_);

			is_running_ = false;

			PostQuitMessage(0);
			return 0;
		}
		break;
		case WM_SIZE:
		{
			int width = LOWORD(l_additional_data);  // Macro to get the low-order word.
			int height = HIWORD(l_additional_data); // Macro to get the high-order word.

			// Respond to the message:
			OnResize(width, height);
		}
		break;
		case WM_KEYDOWN:
			switch(w_additional_data) {
				case(VK_ESCAPE): //If the escape key was pressed
					if (MessageBox(window_handler, "Really quit?", "My application", MB_OKCANCEL) == IDOK)
						DestroyWindow(window_handler);
					else
						return 0;
					break;
				default:
					this->getAttachedApplicationManager()->processKeyboardEvent(w_additional_data);
					break;
			}
		default:
			break;
    }

    return DefWindowProc(window_handler, message_code, w_additional_data, l_additional_data);
}

LRESULT GLWindow::StaticWindowProcess(HWND window_handler, UINT message_code, WPARAM w_additional_data, LPARAM l_additional_data) {
	GLWindow* window = NULL;

    //If this is the create message
    switch(message_code) {
		case WM_CREATE:
		{
			//Get the pointer we stored during create
			window = (GLWindow*)((LPCREATESTRUCT)l_additional_data)->lpCreateParams;

			//Associate the window pointer with the hwnd for the other events to access
			SetWindowLongPtr(window_handler, GWL_USERDATA, (LONG_PTR)window);
		}
		break;
		default:
		{
			//If this is not a creation event, then we should have stored a pointer to the window
			window = (GLWindow*)GetWindowLongPtr(window_handler, GWL_USERDATA);

			if(!window) 
				return DefWindowProc(window_handler, message_code, w_additional_data, l_additional_data);    
		}
		break;
	}

    //Call our window's member WndProc (allows us to access member variables)
	return window->WindowProcess(window_handler, message_code, w_additional_data, l_additional_data);
}
