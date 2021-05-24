#include "X11Util.h"
X11Util::X11Util(/* args */)
{
}

X11Util::~X11Util()
{
}
bool X11Util::CreateWindow(std::string title){
     Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    EGLConfig ecfg;
    EGLint num_config;
    Window win;
    /*
     * X11 native display initialization
     */

    m_xdisplay = XOpenDisplay(NULL);
    if ( m_xdisplay == NULL )
    {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(m_xdisplay);

    swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(
               m_xdisplay, root,
               0, 0, m_gles_context->m_width, m_gles_context->m_height, 0,
               CopyFromParent, InputOutput,
               CopyFromParent, CWEventMask,
               &swa );

    xattr.override_redirect = false;
    XChangeWindowAttributes ( m_xdisplay, win, CWOverrideRedirect, &xattr );

    hints.input = true;
    hints.flags = InputHint;
    XSetWMHints(m_xdisplay, win, &hints);

    // make the window visible on the screen
    XMapWindow (m_xdisplay, win);
    XStoreName (m_xdisplay, win, title.c_str());

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (m_xdisplay, "_NET_WM_STATE", false);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = false;
    XSendEvent (
       m_xdisplay,
       DefaultRootWindow ( m_xdisplay ),
       false,
       SubstructureNotifyMask,
       &xev );

    m_gles_context->m_egl_native_window = (EGLNativeWindowType) win;
    m_gles_context->m_egl_native_display = (EGLNativeDisplayType) m_xdisplay;
    return EGL_TRUE;
}

bool X11Util::UserInterrupt()
{
    XEvent xev;
    KeySym key;
    GLboolean userinterrupt = GL_FALSE;
    char text;

    // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
    while ( XPending ( m_xdisplay ) )
    {
        XNextEvent( m_xdisplay, &xev );
        if ( xev.type == DestroyNotify )
            userinterrupt = GL_TRUE;
    }
    return userinterrupt;
}

