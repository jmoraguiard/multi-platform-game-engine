#ifdef _WIN32
#include <windows.h>
#include "glwwindow.h"
#endif

#include "ApplicationManager.h"
#include "clock.h"

//wWinMain is the conventional and most recent name used for the application entry point. (http://msdn.microsoft.com/en-us/library/windows/desktop/ff381406%28v=vs.85%29.aspx)
#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE instance_handler, //instance_handler is something called a "handle to an instance" or "handle to a module." The operating system uses this value to identify the executable (EXE) when it is loaded in memory. The instance handle is needed for certain Windows functions — for example, to load icons or bitmaps.
										HINSTANCE instance_previous_handler, //instance_previous_handler has no meaning. It was used in 16-bit Windows, but is now always zero.
										PWSTR pCmdLine, //pCmdLine contains the command-line arguments as a Unicode string.
										int nCmdShow) { //nCmdShow is a flag that says whether the main application window will be minimized, maximized, or shown normally.
#endif

	const int window_width			= 1024;
	const int window_height			= 768;
	const int window_bpp			= 16;
	const bool window_fullscreen	= false;

#ifdef _WIN32
	GLWindow program_window(instance_handler);
#endif

	ApplicationManager application_manager;

	Clock application_clock;

	program_window.attachApplicationManager(&application_manager);

	if (!program_window.create(window_width, window_height, window_bpp, window_fullscreen)) {
#ifdef _WIN32
		MessageBox(NULL, "Window Creation Failed!", "An error ocurrer", MB_ICONERROR | MB_OK);
#endif
		program_window.destroy();
		return 0;
	}

	if (!application_manager.init()) {
#ifdef _WIN32
		MessageBox(NULL, "Application Creation Failed!", "An error ocurrer", MB_ICONERROR | MB_OK);
#endif
		program_window.destroy();
		return 0;
	}

	while (program_window.isRunning()) {
		program_window.processEvents(); //Process any window events

		float elapsedTime = application_clock.calculateDeltaSeconds();

		application_manager.update(elapsedTime);
		application_manager.draw();

		program_window.swapBuffers();
    }

    program_window.destroy(); //Destroy the program window

	return 0; //The return value is not used by the operating system, but you can use the return value to convey a status code to some other program that you write.
}
