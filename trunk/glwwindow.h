#ifndef _GLWINDOW_H
#define _GLWINDOW_H

#include <windows.h>
#include <ctime>

class ApplicationManager;

class GLWindow 
{
	public:
		GLWindow(HINSTANCE instance_handler); //default constructor
				
		bool create(int width, int height, int bpp, bool fullscreen);
		void destroy();
		void processEvents();
		void attachApplicationManager(ApplicationManager* game_manager);

		float getElapsedSeconds();
    
		bool isRunning(); //Is the window running?

		void swapBuffers();

		static LRESULT CALLBACK StaticWindowProcess(HWND window_handler, UINT message_code, WPARAM w_additional_data, LPARAM l_additional_data);
		LRESULT CALLBACK WindowProcess(HWND window_handler, UINT message_code, WPARAM w_additional_data, LPARAM l_additional_data);

	private:
		void OnResize(int width, int height);
		void setupPixelFormat(void);
		ApplicationManager* getAttachedApplicationManager() { return game_manager_; }

		ApplicationManager* game_manager_;
		bool is_running_;
		bool full_screen_;
		float last_time_;

		HWND window_handler_;
		HDC device_context_handler_;
		HGLRC rendering_context_handler_;
		HINSTANCE instance_handler_;
		WNDCLASSEX window_class; //Contains window class information. (http://msdn.microsoft.com/en-us/library/windows/desktop/ms633577%28v=vs.85%29.aspx)
		RECT window_rectangle_; //Windows size
		DWORD window_extended_style_; //Window Extended Style
		DWORD window_style_; //Window Style
};

#endif
