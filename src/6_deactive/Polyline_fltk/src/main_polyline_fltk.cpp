//
// Tiny OpenGL v3 demo program for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2024 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <stdarg.h>
#include <FL/Fl.H>
#include <FL/platform.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include "GL_Util.h"
#include "GL_Shader1.h"
#include "GL_RoundRectShader.h"
#include <de/polyline/PolylineTest.h>
#include <de_opengl.h>
#include <FL/gl.h> // for gl_texture_reset()

class MainWindow : public Fl_Gl_Window
{
    de::gpu::VideoDriver* m_driver;
    GL_Driver m_driver2;
    GL_Shader1 m_shader1;
    GL_RoundRectShader m_roundRectShader;
    de::polyline::PolylineRenderer m_polylineRenderer;
public:
    MainWindow(int x, int y, int w, int h)
        : Fl_Gl_Window(x, y, w, h)
    {
        mode(FL_RGB8 | FL_DOUBLE | FL_OPENGL3);
        m_driver = new de::gpu::VideoDriver();
    }

    ~MainWindow() override
    {
        if (m_driver)
        {
            m_driver->close();
            delete m_driver;
        }
    }

    void draw(void) FL_OVERRIDE
    {
        int w = pixel_w();
        int h = pixel_h();
        // glViewport(0, 0, pixel_w(), pixel_h());
        // glClearColor(0.08f, 0.8f, 0.8f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);


        // m_shader1.draw(0,0);
        // m_shader1.draw(100,100);

        // m_roundRectShader.draw(w, h);

        m_driver->resize(w,h);
        m_driver->beginRender( glm::vec4{ 0.08f, 0.8f, 0.8f, 1.0f } );

        m_polylineRenderer.draw();

        m_driver->draw2DPerfOverlay();
        m_driver->endRender();

        Fl_Gl_Window::draw(); // Draw FLTK child widgets.
    }

    int handle(int event) FL_OVERRIDE
    {
        static int first = 1;
        if (first && event == FL_SHOW && shown())
        {
            first = 0;
            make_current();
            #ifndef __APPLE__
            ensureDesktopOpenGL();
            //GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
            #ifdef FLTK_USE_WAYLAND
            // glewInit returns GLEW_ERROR_NO_GLX_DISPLAY with Wayland
            // see https://github.com/nigels-com/glew/issues/273
            if (fl_wl_display() && err == GLEW_ERROR_NO_GLX_DISPLAY) err = GLEW_OK;
            #endif
            //if (err)
            //    Fl::warning("glewInit() failed returning %u", err);
            //else
                add_output("Using GLEW %s\n", glewGetString(GLEW_VERSION));
            #endif
            m_driver2.init();
            m_driver->open(1024,768);
            m_polylineRenderer.init( m_driver );
            redraw();
        }

        int retval = Fl_Gl_Window::handle(event);
        if (retval) return retval;

        if (event == FL_PUSH)
        {
            m_shader1.update();
            redraw();
            add_output("push  Fl_Gl_Window::pixels_per_unit()=%.1f\n", pixels_per_unit());
            return 1;
        }
        return retval;
    }
    void reset(void)
    {
        //shaderProgram = 0;
        gl_texture_reset();
    }
};

void toggle_double(Fl_Widget *wid, void *data)
{
    static bool doublebuff = true;
    doublebuff = !doublebuff;
    auto glwin = (MainWindow*)data;
    int flags = glwin->mode();
    if (doublebuff) flags |= FL_DOUBLE; else flags &= ~FL_DOUBLE;
    glwin->reset();
    glwin->hide();
    glwin->mode(flags);
    glwin->show();
}

Fl_Text_Display *output; // shared between output_win() and add_output()

void output_win(MainWindow *gl)
{
    output = new Fl_Text_Display(300,0,500, 280);
    Fl_Light_Button *lb = new Fl_Light_Button(300, 280, 500, 20, "Double-Buffered");
    lb->callback(toggle_double);
    lb->user_data(gl);
    lb->value(1);
    output->buffer(new Fl_Text_Buffer());
}

void add_output(const char *format, ...)
{
    va_list args;
    char line_buffer[10000];
    va_start(args, format);
    vsnprintf(line_buffer, sizeof(line_buffer)-1, format, args);
    va_end(args);
    output->buffer()->append(line_buffer);
    output->scroll(10000, 0);
    output->redraw();
}

void button_cb(Fl_Widget *widget, void *)
{
    add_output("run %s callback\n", widget->label());
}

void add_widgets(Fl_Gl_Window *g)
{
    Fl::set_color(FL_FREE_COLOR, 255, 255, 255, 140); // partially transparent white
    g->begin();
    // Create here widgets to go above the GL3 scene
    Fl_Button* b = new Fl_Button( 0, 0, 60, 30, "button");
    b->color(FL_FREE_COLOR);
    b->box(FL_DOWN_BOX );
    b->callback(button_cb, NULL);
    Fl_Button* b2 = new Fl_Button( 0, 170, 60, 30, "button2");
    b2->color(FL_FREE_COLOR);
    b2->box(FL_BORDER_BOX );
    b2->callback(button_cb, NULL);
    g->end();
}

int main(int argc, char **argv)
{
    Fl::use_high_res_GL(1);
    auto topwin = new Fl_Window(800, 300);
    auto win = new MainWindow(0, 0, 300, 300);
    win->end();
    output_win(win);
    add_widgets(win);
    topwin->end();
    topwin->resizable(win);
    topwin->label("Click GL panel to reshape");
    topwin->show(argc, argv);
    Fl::run();
}

