#include <iostream>
#include <string>

#include "glxwindow.h"
#include "applicationManager.h"

using std::string;

typedef GLXContext (* PFNGLXCREATECONTEXTATTRIBSARBPROC) (Display *dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list);

GLX11Window::GLX11Window():
application_manager_(NULL),
is_running_(true),
display_(NULL),
x_window_(0),
gl_context_(0),
screen_id_(0),
full_screen_(false),
width_(0),
height_(0),
bpp_(0),
gl3_supported_(false) {

}

GLX11Window::~GLX11Window() {

}

bool GLX11Window::create(int width, int height, int bpp, bool fullscreen) {
    display_ = XOpenDisplay(0);  //Open default display
    if (display_ == NULL)
    {
        std::cerr << "Could not open the display" << std::endl;
        return false;
    }

    screen_id_ = DefaultScreen(display_); //Get the default screen id

    int n = 0, modeNum = 0;
    //Get a framebuffer config using the default attributes
    GLXFBConfig framebufferConfig = (*glXChooseFBConfig(display_, DefaultScreen(display_), 0, &n));

    XF86VidModeModeInfo **modes;
    if (!XF86VidModeGetAllModeLines(display_, screen_id_, &modeNum, &modes))
    {
        std::cerr << "Could not query the video modes" << std::endl;
        return false;
    }

    XF86_desktop_mode_ = *modes[0];

    int bestMode = -1;
    for (int i = 0; i < modeNum; i++)
    {
        if ((modes[i]->hdisplay == width) &&
            (modes[i]->vdisplay == height))
        {
            bestMode = i;
        }
    }

    if (bestMode == -1)
    {
        std::cerr << "Could not find a suitable graphics mode" << std::endl;
        return false;
    }


    int doubleBufferedAttribList [] = {
        GLX_RGBA, GLX_DOUBLEBUFFER,
        GLX_RED_SIZE, 4,
        GLX_GREEN_SIZE, 4,
        GLX_BLUE_SIZE, 4,
        GLX_DEPTH_SIZE, 16,
        None
    };

    XVisualInfo* vi = NULL;
    //Attempt to create a double buffered window
    vi = glXChooseVisual(display_, screen_id_, doubleBufferedAttribList);

    if (vi == NULL)
    {
        std::cerr << "Could not create a double buffered window" << std::endl;
        return false;
    }

    //Create a GL 2.1 context
    GLXContext gl2Context = glXCreateContext(display_, vi, 0, GL_TRUE);

    if (gl2Context == NULL)
    {
        std::cerr << "Could not create a GL 2.1 context, please check your graphics drivers" << std::endl;
        return false;
    }

    //Get a pointer to the GL 3.0 context creation
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");

    if (glXCreateContextAttribs == NULL)
    {
        std::cerr << "OpenGL 3.0 is not supported, falling back to 2.1" << std::endl;
        gl_context_ = gl2Context;
        gl3_supported_ = false;
    }
    else
    {
        //Create a GL 3.0 context

        int attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,//we want a 3.0 context
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            0 //zero indicates the end of the array
        };

        gl_context_ = glXCreateContextAttribs(display_, framebufferConfig, 0, true, &attribs[0]);
        glXDestroyContext(display_, gl2Context); //We can destroy the GL 2.0 context once the 3.0 one has bene created
        gl3_supported_ = true;
    }

    Colormap cmap = XCreateColormap(display_, RootWindow(display_, vi->screen),vi->visual, AllocNone);
    x_set_attributes_.colormap = cmap;
    x_set_attributes_.border_pixel = 0;
    x_set_attributes_.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
                                StructureNotifyMask;

    x_set_attributes_.override_redirect = False;

    unsigned long windowAttributes = CWBorderPixel | CWColormap | CWEventMask;

    if (fullscreen)
    {
        windowAttributes = CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

        XF86VidModeSwitchToMode(display_, screen_id_, modes[bestMode]);
        XF86VidModeSetViewPort(display_, screen_id_, 0, 0);
        x_set_attributes_.override_redirect = True;
    }

    x_window_ = XCreateWindow(display_, RootWindow(display_, vi->screen),
                                  0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
                                  CWBorderPixel | CWColormap | CWEventMask, &x_set_attributes_);

    string title = "BOGLGP - Chapter 2 - OpenGL Application";

    if (fullscreen)
    {
        XWarpPointer(display_, None, x_window_, 0, 0, 0, 0, 0, 0);
        XMapRaised(display_, x_window_);
        XGrabKeyboard(display_, x_window_, True, GrabModeAsync, GrabModeAsync, CurrentTime);
        XGrabPointer(display_, x_window_, True, ButtonPressMask,
                     GrabModeAsync, GrabModeAsync, x_window_, None, CurrentTime);

        full_screen_ = true;
    }
    else
    {
        Atom wmDelete = XInternAtom(display_, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(display_, x_window_, &wmDelete, 1);
        XSetStandardProperties(display_, x_window_, title.c_str(), None, NULL, NULL, 0, NULL);
        XMapRaised(display_, x_window_);
    }


    XFree(modes);

    //Make the new context current
    glXMakeCurrent(display_, x_window_, gl_context_);

    int posx = 0;
    int posy = 0;
    Window winDummy;
    unsigned int borderDummy;

    width_ = (unsigned) width;
    height_ = (unsigned) height;
    bpp_ = (unsigned) bpp;

    XGetGeometry(display_, x_window_, &winDummy,
                 &posx, &posy, &width_, &height_,
                 &borderDummy, &bpp_);

    return true;
}

void GLX11Window::destroy() {
    if (gl_context_)
    {
        glXMakeCurrent(display_, None, NULL);
        glXDestroyContext(display_, gl_context_);
        gl_context_ = NULL;
    }

    if (full_screen_)
    {
        XF86VidModeSwitchToMode(display_, screen_id_, &XF86_desktop_mode_);
        XF86VidModeSetViewPort(display_, screen_id_, 0, 0);
    }

    XCloseDisplay(display_);
}

void GLX11Window::processEvents() {
    XEvent event;

    while (XPending(display_) > 0)
    {
        XNextEvent(display_, &event);
        switch (event.type)
        {
        case Expose:
            if (event.xexpose.count != 0)
                break;
            break;
        case ConfigureNotify:
        {
            int width = event.xconfigure.width;
            int height = event.xconfigure.height;
            getAttachedApplicationManager()->onResize(width, height);
        }
        break;
        case KeyPress:
        {
            if (XLookupKeysym(&event.xkey,0) == XK_Escape) {
                is_running_ = false;
            }

            //Register the key press with the keyboard interface
        }
        break;
        case KeyRelease:
        {

        }
        break;

        case ClientMessage:
            if (string(XGetAtomName(display_, event.xclient.message_type)) == string("WM_PROTOCOLS"))
            {
                is_running_ = false;
            }
            break;
        default:
            break;
        }
    }

}

bool GLX11Window::isRunning() {
    return is_running_;
}

void GLX11Window::attachApplicationManager(ApplicationManager* application_manager) {
    application_manager_ = application_manager;
}

void GLX11Window::swapBuffers() {
    glXSwapBuffers(display_, x_window_);
}
