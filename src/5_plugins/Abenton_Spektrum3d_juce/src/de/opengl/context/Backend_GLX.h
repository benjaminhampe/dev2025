#pragma once
#include "IOpenGLBackend.h"

#ifndef _WIN32
#include <X11/Xlib.h>
#include <GL/glx.h>

class Backend_GLX : public IOpenGLBackend
{
public:
    Display* dpy = nullptr;
    Window win = 0;
    GLXContext ctx = nullptr;

    bool createWindow(void* parentHandle, int x, int y, int w, int h) override
    {
        dpy = XOpenDisplay(nullptr);
        if (!dpy) return false;

        Window parent = (Window) parentHandle;

        int attribs[] = {
            GLX_RGBA,
            GLX_DOUBLEBUFFER,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            None
        };

        XVisualInfo* vi = glXChooseVisual(dpy, DefaultScreen(dpy), attribs);
        if (!vi) return false;

        XSetWindowAttributes swa;
        swa.colormap = XCreateColormap(dpy, parent, vi->visual, AllocNone);
        swa.event_mask = ExposureMask | StructureNotifyMask;

        win = XCreateWindow(
            dpy, parent,
            x, y, w, h,
            0, vi->depth, InputOutput, vi->visual,
            CWColormap | CWEventMask, &swa);

        XMapWindow(dpy, win);

        ctx = glXCreateContext(dpy, vi, nullptr, GL_TRUE);
        return ctx != nullptr;
    }

    void destroy() override
    {
        if (ctx) { glXDestroyContext(dpy, ctx); ctx = nullptr; }
        if (win) { XDestroyWindow(dpy, win); win = 0; }
        if (dpy) { XCloseDisplay(dpy); dpy = nullptr; }
    }

    void resize(int x, int y, int w, int h) override
    {
        XMoveResizeWindow(dpy, win, x, y, w, h);
    }

    void makeCurrent() override
    {
        glXMakeCurrent(dpy, win, ctx);
    }

    void doneCurrent() override
    {
        glXMakeCurrent(dpy, None, nullptr);
    }

    void swapBuffers() override
    {
        glXSwapBuffers(dpy, win);
    }
/*
    void render() override
    {
        makeCurrent();

        glViewport(0, 0, 800, 600); // TODO: echte Größe holen
        glClearColor(0.1f, 0.1f, 0.15f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Dein Rendering

        swapBuffers();
        doneCurrent();
    }
*/
};

#endif
