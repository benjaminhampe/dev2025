// main.cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>

// #include <FL/Fl.H>
// #include <FL/Fl_Window.H>
// #include <FL/x.H>

// #include <windows.h>
// #include <GL/gl.h>
// #include <GL/wgl.h>

class GLWindow : public Fl_Window {
public:
    GLWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr),
          m_resizing(false)
    {
        box(FL_NO_BOX);
        resizable(this);

        Fl::add_timeout(1.0 / 60.0, timer_cb, this);
    }

    ~GLWindow() override {
        destroy_wgl();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_SHOW:
            init_wgl();
            install_winproc_hook();
            return 1;

        default:
            return Fl_Window::handle(e);
        }
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);
        redraw();
    }

    void draw() override
    {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    // ---------------- TIMER ----------------
    static void timer_cb(void* userdata)
    {
        GLWindow* self = (GLWindow*)userdata;

        if (!self->m_resizing)
            self->redraw();

        Fl::repeat_timeout(1.0 / 60.0, timer_cb, userdata);
    }

    // ---------------- WGL INIT ----------------
    void init_wgl()
    {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl()
    {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
        }
    }

    // ---------------- WIN32 RESIZE HOOK ----------------
    static LRESULT CALLBACK HookProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
    {
        GLWindow* self = (GLWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (self)
        {
            if (msg == WM_ENTERSIZEMOVE)
                self->m_resizing = true;

            if (msg == WM_EXITSIZEMOVE)
                self->m_resizing = false;
        }

        return CallWindowProc(self->m_oldProc, hwnd, msg, w, l);
    }

    void install_winproc_hook()
    {
        HWND hwnd = (HWND)fl_xid(this);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
        m_oldProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookProc);
    }

    HDC     m_hdc;
    HGLRC   m_hglrc;
    bool    m_resizing;
    WNDPROC m_oldProc;
};

int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    GLWindow win(800, 600, "Raw OpenGL FLTK Window (NO LAG RESIZE)");
    win.show(argc, argv);

    return Fl::run();
}


#if 0

class GLWindow : public Fl_Window {
public:
    GLWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr)
    {
        box(FL_NO_BOX);
        resizable(this);
        Fl::add_timeout(0.0, timer_cb, this); // start render loop
    }

    ~GLWindow() override {
        destroy_wgl();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_SHOW:
            init_wgl();
            return Fl_Window::handle(e);

        default:
            return Fl_Window::handle(e);
        }
    }

    void resize(int X, int Y, int W, int H) override
    {
        //Fl_Window::resize(X, Y, W, H);
        redraw(); // let FLTK schedule a repaint; no direct draw() here
    }

    void draw() override
    {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    static void timer_cb(void* userdata)
    {
        GLWindow* self = (GLWindow*)userdata;
        self->redraw();                         // ask FLTK to repaint
        Fl::repeat_timeout(1.0 / 60.0, timer_cb, userdata); // ~60 FPS
    }

    void init_wgl()
    {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl()
    {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

    HDC   m_hdc;
    HGLRC m_hglrc;
};

int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    GLWindow win(800, 600, "Raw OpenGL FLTK Window (proper loop)");
    win.show(argc, argv);

    return Fl::run();
}
#endif

#if 0
class GLWindow : public Fl_Window {
public:
    GLWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr)
    {
        box(FL_NO_BOX);
        resizable(this);
    }

    ~GLWindow() override {
        destroy_wgl();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_SHOW:
            init_wgl();
            return 1;

        case FL_HIDE:
            return 1;

        default:
            return Fl_Window::handle(e);
        }
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);
        // resize event triggers immediate redraw
        draw();
    }

    void draw() override
    {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    void init_wgl()
    {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl()
    {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
        }
    }

    HDC   m_hdc;
    HGLRC m_hglrc;
};

// ---------------- MAIN LOOP ----------------
int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    GLWindow win(800, 600, "Raw OpenGL FLTK Window");
    win.show(argc, argv);

    // REAL RENDER LOOP
    while (win.shown())
    {
        Fl::wait(0);   // process events without blocking
        win.draw();    // continuous rendering
    }

    return 0;
}

#endif
#if 0
class GLWindow : public Fl_Window {
public:
    GLWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr)
    {
        box(FL_NO_BOX);
        resizable(this);
    }

    ~GLWindow() override {
        destroy_wgl();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_SHOW:
            init_wgl();
            return 1;

        default:
            return Fl_Window::handle(e);
        }
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);
        redraw(); // only redraw when size actually changes
    }

    void draw() override
    {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    void init_wgl()
    {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl()
    {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

    HDC   m_hdc;
    HGLRC m_hglrc;
};

int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    GLWindow win(800, 600, "Raw OpenGL FLTK Window (no timer)");
    win.show(argc, argv);

    return Fl::run();
}
#endif

#if 0
class GLWindow : public Fl_Window {
public:
    GLWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr)
    {
        box(FL_NO_BOX);
        resizable(this);

        // 60 FPS redraw loop
        Fl::add_timeout(1.0 / 60.0, timer_cb, this);
    }

    ~GLWindow() override {
        destroy_wgl();
    }

    // ---------------- Resize ----------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Window::resize(X, Y, W, H);
        printf("resize: %d x %d\n", W, H);
        redraw();
    }

    // ---------------- FLTK Events ----------------
    int handle(int e) override {
        switch (e)
        {
        case FL_SHOW:
            init_wgl();
            return 1;

        default:
            return Fl_Window::handle(e);
        }
    }

    // ---------------- Render ----------------
    void draw() override {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    // ---------------- Timer ----------------
    static void timer_cb(void* userdata) {
        GLWindow* self = (GLWindow*)userdata;
        self->redraw();
        Fl::repeat_timeout(1.0 / 60.0, timer_cb, userdata);
    }

    // ---------------- WGL Init ----------------
    void init_wgl() {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    // ---------------- WGL Destroy ----------------
    void destroy_wgl() {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

    HDC   m_hdc;
    HGLRC m_hglrc;
};

// ---------------- Main ----------------
int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    GLWindow win(800, 600, "Raw OpenGL FLTK Window");
    win.show(argc, argv);

    return Fl::run();
}
#endif


#if 0
class GLWindow : public Fl_Window {
public:
    GLWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr)
    {
        box(FL_NO_BOX);
        resizable(this);
        Fl::add_timeout(1.0 / 60.0, &GLWindow::timer_cb, this);
    }

    ~GLWindow() override {
        destroy_wgl();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_SHOW:
            init_wgl();
            return 1;

        default:
            return Fl_Window::handle(e);
        }
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);

        // THIS is your resize event
        printf("resize: %d x %d\n", W, H);

        // Force redraw so GL viewport updates immediately
        redraw();
    }

    void draw() override
    {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    static void timer_cb(void* userdata)
    {
        GLWindow* self = (GLWindow*)userdata;
        self->redraw();
        Fl::repeat_timeout(1.0 / 60.0, &GLWindow::timer_cb, userdata);
    }

    void init_wgl()
    {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl()
    {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
        }
    }

    HDC   m_hdc;
    HGLRC m_hglrc;
};
#endif

#if 0
// =========================================================
// Shader helpers
// =========================================================
GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        printf("Shader error:\n%s\n", log);
    }
    return s;
}

GLuint createProgram(const char* vs, const char* fs) {
    GLuint v = compileShader(GL_VERTEX_SHADER, vs);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    GLint ok;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(p, sizeof(log), nullptr, log);
        printf("Link error:\n%s\n", log);
    }
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/x.H>

#include <windows.h>
#include <GL/gl.h>
#include <GL/wgl.h>

class MainWindow : public Fl_Window {

    HDC       m_hdc;
    HGLRC     m_hglrc;
    //Fl_Button* m_overlay;
public:
    MainWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title)
        , m_hdc(nullptr)
        , m_hglrc(nullptr)
        // , m_overlay(nullptr)
    {
        // begin();

        // m_overlay = new Fl_Button(w - 80, 10, 70, 25, "Close");
        // m_overlay->callback(&MainWindow::close_cb, this);

        // end();

        resizable(this);

        Fl::add_timeout(1.0 / 60.0, &MainWindow::timer_cb, this);
    }

    ~MainWindow() override {
        destroy_wgl();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_PUSH:
            printf("Mouse down: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_DRAG:
            printf("Mouse drag: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_RELEASE:
            printf("Mouse up\n");
            return 1;

        case FL_KEYDOWN:
            printf("Key down: %d\n", Fl::event_key());
            return 1;

        case FL_KEYUP:
            printf("Key up: %d\n", Fl::event_key());
            return 1;

        case FL_DND_ENTER:
            printf("FL_DND_ENTER: %s\n", Fl::event_text());
            return 1;

        case FL_DND_DRAG:
            printf("FL_DND_DRAG: %s\n", Fl::event_text());
            return 1;

        case FL_DND_RELEASE:
            printf("FL_DND_RELEASE: %s\n", Fl::event_text());
            return 1;

        case FL_PASTE: {
            int mx = Fl::event_x();
            int my = Fl::event_y();
            const char* data = Fl::event_text();
            printf("DROP at %d,%d: %s\n", mx, my, data);
            return 1;
        }

        case FL_SHOW:
            init_wgl();
            return Fl_Window::handle(e);

        case FL_HIDE:
            return Fl_Window::handle(e);
        }
        return Fl_Window::handle(e);
    }

    void draw() override {
        // Fl_Window::draw();

        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

    void resize(int x, int y, int w, int h) override {
        //Fl_Window::resize(x, y, w, h);
        // if (m_overlay)
        //     m_overlay->resize(w - 80, 10, 70, 25);
    }

    static void close_cb(Fl_Widget*, void* userdata)
    {
        MainWindow* self = (MainWindow*)userdata;
        self->hide();
    }

private:
    static void timer_cb(void* userdata)
    {
        MainWindow* self = (MainWindow*)userdata;
        self->redraw();
        Fl::repeat_timeout(1.0 / 60.0, &MainWindow::timer_cb, userdata);
    }

    void init_wgl() {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        if (!hwnd)
            return;

        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl() {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            if (hwnd)
                ReleaseDC(hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

};

int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    MainWindow window(800, 600, "FLTK + WGL + Full Events + Fixed GL");
    window.show(argc, argv);

    return Fl::run();
}
#endif

#if 0

class MainWindow : public Fl_Window {
public:
    MainWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_hdc(nullptr),
          m_hglrc(nullptr),
          m_program(0),
          m_vao(0),
          m_vbo(0),
          m_overlay(nullptr)
    {
        begin();

        m_overlay = new Fl_Button(w - 80, 10, 70, 25, "Close");
        m_overlay->callback(&MainWindow::close_cb, this);

        end();

        resizable(this);

        // render loop: ~60 FPS
        Fl::add_timeout(1.0 / 60.0, &MainWindow::timer_cb, this);
    }

    ~MainWindow() override {
        destroy_gl();
        destroy_wgl();
    }

    // ---------------- FLTK Events ----------------
    int handle(int e) override
    {
        switch (e)
        {
        case FL_PUSH:
            printf("Mouse down: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_DRAG:
            printf("Mouse drag: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_RELEASE:
            printf("Mouse up\n");
            return 1;

        case FL_KEYDOWN:
            printf("Key down: %d\n", Fl::event_key());
            return 1;

        case FL_KEYUP:
            printf("Key up: %d\n", Fl::event_key());
            return 1;

        case FL_DND_ENTER:
            printf("FL_DND_ENTER: %s\n", Fl::event_text());
            return 1;

        case FL_DND_DRAG:
            printf("FL_DND_DRAG: %s\n", Fl::event_text());
            return 1;

        case FL_DND_RELEASE:
            printf("FL_DND_RELEASE: %s\n", Fl::event_text());
            return 1; // accept DnD

        case FL_PASTE: {
            int mx = Fl::event_x();
            int my = Fl::event_y();
            const char* data = Fl::event_text();

            printf("DROP at %d,%d: %s\n", mx, my, data);
            return 1;
        }

        case FL_SHOW:
            init_wgl();
            init_gl();
            return Fl_Window::handle(e);

        case FL_HIDE:
            return Fl_Window::handle(e);
        }
        return Fl_Window::handle(e);
    }

    void draw() override {
        Fl_Window::draw();

        if (!m_hdc || !m_hglrc || !m_program)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_program);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

    void resize(int x, int y, int w, int h) override {
        Fl_Window::resize(x, y, w, h);
        if (m_overlay)
            m_overlay->resize(w - 80, 10, 70, 25);
    }

    // ---------------- Close ----------------
    static void close_cb(Fl_Widget*, void* userdata)
    {
        MainWindow* self = (MainWindow*)userdata;
        self->hide();
    }

private:
    // render loop timer
    static void timer_cb(void* userdata)
    {
        MainWindow* self = (MainWindow*)userdata;
        self->redraw();
        Fl::repeat_timeout(1.0 / 60.0, &MainWindow::timer_cb, userdata);
    }

    void init_wgl() {
        if (m_hdc)
            return;

        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_wgl() {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

    void init_gl() {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        const char* vsSrc =
            "#version 120\n"
            "attribute vec2 a_pos;\n"
            "void main(){ gl_Position = vec4(a_pos, 0.0, 1.0); }\n";

        const char* fsSrc =
            "#version 120\n"
            "void main(){ gl_FragColor = vec4(1.0, 0.4, 0.2, 1.0); }\n";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsSrc, nullptr);
        glCompileShader(fs);

        m_program = glCreateProgram();
        glAttachShader(m_program, vs);
        glAttachShader(m_program, fs);
        glLinkProgram(m_program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        float verts[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.0f,  0.5f
        };

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        GLint loc = glGetAttribLocation(m_program, "a_pos");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);

        wglMakeCurrent(nullptr, nullptr);
    }

    void destroy_gl() {
        if (m_vbo) {
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
        if (m_vao) {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
        if (m_program) {
            glDeleteProgram(m_program);
            m_program = 0;
        }
    }

    HDC     m_hdc;
    HGLRC   m_hglrc;
    GLuint  m_program;
    GLuint  m_vao;
    GLuint  m_vbo;

    Fl_Button* m_overlay;
};

int main(int argc, char** argv)
{
    Fl::visual(FL_DOUBLE | FL_INDEX);

    MainWindow window(800, 600, "FLTK + WGL + Full Events + Shader + Timer");
    window.show(argc, argv);

    return Fl::run();
}
#endif

#if 0
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/x.H>

#include <windows.h>
#include <GL/gl.h>
#include <GL/wgl.h>

class GLCanvas : public Fl_Window {
public:
    GLCanvas(int x, int y, int w, int h)
        : Fl_Window(x, y, w, h, nullptr),
          m_hdc(nullptr),
          m_hglrc(nullptr),
          m_program(0),
          m_vao(0),
          m_vbo(0)
    {
        box(FL_NO_BOX);
    }

    ~GLCanvas() override {
        destroy_gl();
        destroy_context();
    }

    void draw() override {
        if (!m_hglrc)
            init_context();
        if (!m_program)
            init_gl();

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_program);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

private:
    void init_context() {
        HWND hwnd = (HWND)fl_xid(this);
        m_hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
    }

    void destroy_context() {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            HWND hwnd = (HWND)fl_xid(this);
            ReleaseDC(hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

    void init_gl() {
        wglMakeCurrent(m_hdc, m_hglrc);

        const char* vsSrc =
            "#version 120\n"
            "attribute vec2 a_pos;\n"
            "void main(){ gl_Position = vec4(a_pos, 0.0, 1.0); }\n";

        const char* fsSrc =
            "#version 120\n"
            "void main(){ gl_FragColor = vec4(1.0, 0.4, 0.2, 1.0); }\n";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsSrc, nullptr);
        glCompileShader(fs);

        m_program = glCreateProgram();
        glAttachShader(m_program, vs);
        glAttachShader(m_program, fs);
        glLinkProgram(m_program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        float verts[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.0f,  0.5f
        };

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        GLint loc = glGetAttribLocation(m_program, "a_pos");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);

        wglMakeCurrent(nullptr, nullptr);
    }

    void destroy_gl() {
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_program) glDeleteProgram(m_program);
    }

    HDC   m_hdc;
    HGLRC m_hglrc;

    GLuint m_program;
    GLuint m_vao;
    GLuint m_vbo;
};

class MainWindow : public Fl_Window {
public:
    MainWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title),
          m_glCanvas(nullptr),
          m_overlay(nullptr)
    {
        begin();

        m_glCanvas = new GLCanvas(0, 0, w, h);
        m_glCanvas->end();

        m_overlay = new Fl_Button(w - 80, 10, 70, 25, "Overlay");
        m_overlay->callback(&MainWindow::close_cb, this);

        end();

        resizable(m_glCanvas);
    }

    // ---------------- FLTK Events ----------------
    int handle(int e) override
    {
        switch (e)
        {
        case FL_PUSH:
            printf("Mouse down: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_DRAG:
            printf("Mouse drag: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_RELEASE:
            printf("Mouse up\n");
            return 1;

        case FL_KEYDOWN:
            printf("Key down: %d\n", Fl::event_key());
            return 1;

        case FL_KEYUP:
            printf("Key up: %d\n", Fl::event_key());
            return 1;

        case FL_DND_ENTER:
            printf("FL_DND_ENTER: %s\n", Fl::event_text());
            return 1;

        case FL_DND_DRAG:
            printf("FL_DND_DRAG: %s\n", Fl::event_text());
            return 1;

        case FL_DND_RELEASE:
            printf("FL_DND_RELEASE: %s\n", Fl::event_text());
            return 1;

        case FL_PASTE: {
            int mx = Fl::event_x();
            int my = Fl::event_y();
            const char* data = Fl::event_text();
            printf("DROP at %d,%d: %s\n", mx, my, data);
            return 1;
        }

        case FL_SHOW:
            printf("FL_SHOW\n");
            return Fl_Window::handle(e);

        case FL_HIDE:
            printf("FL_HIDE\n");
            return Fl_Window::handle(e);
        }

        return Fl_Window::handle(e);
    }

    // ---------------- Close ----------------
    static void close_cb(Fl_Widget*, void* userdata)
    {
        MainWindow* self = (MainWindow*)userdata;
        self->hide();
    }

    void resize(int x, int y, int w, int h) override {
        Fl_Window::resize(x, y, w, h);
        if (m_glCanvas)
            m_glCanvas->resize(0, 0, w, h);
        if (m_overlay)
            m_overlay->resize(w - 80, 10, 70, 25);
    }

private:
    GLCanvas*  m_glCanvas;
    Fl_Button* m_overlay;
};

int main(int argc, char** argv) {
    Fl::visual(FL_DOUBLE | FL_INDEX);

    MainWindow win(800, 600, "FLTK + WGL + Shader + Overlay + Full Events");
    win.show(argc, argv);

    while (Fl::wait()) {
        win.draw();
    }
}

// =========================================================
// MainWindow class
// =========================================================
class MainWindow : public Fl_Window
{
public:
    HDC   hdc   = nullptr;
    HGLRC hglrc = nullptr;
    GLuint program = 0;
    GLint uScreenSize = -1;

    MainWindow(int W, int H, const char* L)
        : Fl_Window(W, H, L)
    {
        begin();
        new Fl_Button(10, 10, 120, 30, "Overlay");
        end();

        callback(close_cb, this);
        resizable(this);

        // Enable drag & drop
        this->when(FL_WHEN_CHANGED);
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Window::resize(X, Y, W, H);
        printf("Resize: %d x %d\n", W, H);
    }
    // ---------------- WGL Setup ----------------
    void init_wgl() {
        HWND hwnd = (HWND)fl_xid(this);
        hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pf = ChoosePixelFormat(hdc, &pfd);
        SetPixelFormat(hdc, pf, &pfd);

        hglrc = wglCreateContext(hdc);
        wglMakeCurrent(hdc, hglrc);

        glewInit();
    }

    // ---------------- GL Init ----------------
    void init_gl() {
        const char* vs = R"(
            #version 330 core
            uniform vec2 screenSize;
            out vec2 uv;
            void main() {
                vec2 pos;
                if (gl_VertexID == 0) pos = vec2(-1,-1);
                else if (gl_VertexID == 1) pos = vec2( 1,-1);
                else if (gl_VertexID == 2) pos = vec2( 1, 1);
                else pos = vec2(-1, 1);

                gl_Position = vec4(pos,0,1);
                uv = pos * 0.5 + 0.5;
            }
        )";

        const char* fs = R"(
            #version 330 core
            in vec2 uv;
            out vec4 color;
            void main() {
                if (uv.x < 0.5 && uv.y < 0.5) color = vec4(1,0,0,1);
                else if (uv.x >= 0.5 && uv.y < 0.5) color = vec4(0,1,0,1);
                else if (uv.x < 0.5 && uv.y >= 0.5) color = vec4(0,0,1,1);
                else color = vec4(1,1,0,1);
            }
        )";

        program = createProgram(vs, fs);
        uScreenSize = glGetUniformLocation(program, "screenSize");

        glDisable(GL_DEPTH_TEST);
    }

    // ---------------- Rendering ----------------
    void render() {
        wglMakeCurrent(hdc, hglrc);

        glViewport(0, 0, w(), h());
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform2f(uScreenSize, (float)w(), (float)h());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        SwapBuffers(hdc);
    }

    // ---------------- FLTK Events ----------------
    int handle(int e) override
    {
        switch (e)
        {
        case FL_PUSH:
            printf("Mouse down: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_DRAG:
            printf("Mouse drag: %d %d\n", Fl::event_x(), Fl::event_y());
            return 1;

        case FL_RELEASE:
            printf("Mouse up\n");
            return 1;

        case FL_KEYDOWN:
            printf("Key down: %d\n", Fl::event_key());
            return 1;

        case FL_KEYUP:
            printf("Key up: %d\n", Fl::event_key());
            return 1;

        case FL_DND_ENTER:
            printf("FL_DND_ENTER: %s\n", Fl::event_text());
            return 1;

        case FL_DND_DRAG:
            printf("FL_DND_DRAG: %s\n", Fl::event_text());
            return 1;

        case FL_DND_RELEASE:
            printf("FL_DND_RELEASE: %s\n", Fl::event_text());
            return 1; // accept DnD

        case FL_PASTE: {
            int mx = Fl::event_x();
            int my = Fl::event_y();
            const char* data = Fl::event_text();

            printf("DROP at %d,%d: %s\n", mx, my, data);
            return 1;
        }

        case FL_SHOW:
            init_wgl();
            init_gl();
            return Fl_Window::handle(e);

        case FL_HIDE:
            return Fl_Window::handle(e);
        }
        return Fl_Window::handle(e);
    }

    // ---------------- Close ----------------
    static void close_cb(Fl_Widget*, void* userdata)
    {
        MainWindow* self = (MainWindow*)userdata;
        self->hide();
    }
};

// =========================================================
// Main
// =========================================================
int main(int argc, char** argv) {
    MainWindow win(800, 600, "FLTK + WGL + GLEW + Events");
    win.show(argc, argv);

    while (Fl::wait()) {
        win.render();
    }

    return 0;
}
#endif
