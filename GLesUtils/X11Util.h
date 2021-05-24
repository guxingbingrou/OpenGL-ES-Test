#ifndef _X11_UTIL_H__
#define _X11_UTIL_H__
#include "GlesUtil.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cstring>
class X11Util : public GlesUtil
{
private:
    /* data */
public:
    X11Util(/* args */);
    ~X11Util();
    bool CreateWindow(std::string title) override;
    bool UserInterrupt() override;
private:
    Display* m_xdisplay = nullptr;
};

#endif

