#ifndef _GLX_WINDOW_H_
#define _GLX_WINDOW_H_

#define GLX_GLXEXT_LEGACY //mMust be declared so the local gltext.h is taken rather than system one
#include <GL/glx.h>
#include "glx/glxext.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include <ctime>

class ApplicationManager;

class GLX11Window
{
    public:
        GLX11Window();
        virtual ~GLX11Window();

        bool create(int width, int height, int bpp, bool fullscreen);
        void destroy();
        void processEvents();
        void attachApplicationManager(ApplicationManager* game_manager);

        bool isRunning();

        void swapBuffers();

    private:
        ApplicationManager* application_manager_;
        bool is_running_;

        ApplicationManager* getAttachedApplicationManager() { return application_manager_; }

        Display* display_;
        Window x_window_;
        GLXContext gl_context_;
        XF86VidModeModeInfo XF86_desktop_mode_;
        XSetWindowAttributes x_set_attributes_;
        int screen_id_;

        bool full_screen_;
        unsigned int width_;
        unsigned int height_;
        unsigned int bpp_;

        bool gl3_supported_;

};

#endif
