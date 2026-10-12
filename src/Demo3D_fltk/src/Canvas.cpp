#include "Canvas.h"
#include "App.h"
#include "game/Draw3D.h"
#include <de/gpu/BenniGL.h>

static void button_cb(Fl_Widget *widget, void *)
{
    add_output("run %s callback", widget->label());
}

Canvas::Canvas(int x, int y, int w, int h)
    : Fl_Gl_Window(x, y, w, h)
{
    mode(FL_RGB8 | FL_DOUBLE | FL_OPENGL3 | FL_DEPTH | FL_STENCIL);
    shaderProgram = 0;
    gl_version_major = 0;

    Fl::set_color(FL_FREE_COLOR, 255, 255, 255, 140); // partially transparent white

    auto & ui = App::getInstance()->ui;

    begin();

    // Create here widgets to go above the GL3 scene
    ui.btnCanvas1 = new Fl_Button( 0, 0, 60, 30, "button");
    ui.btnCanvas1->color(FL_FREE_COLOR);
    ui.btnCanvas1->box(FL_DOWN_BOX );
    ui.btnCanvas1->callback(button_cb, NULL);

    ui.btnCanvas2 = new Fl_Button( 0, 170, 60, 30, "button2");
    ui.btnCanvas2->color(FL_FREE_COLOR);
    ui.btnCanvas2->box(FL_BORDER_BOX );
    ui.btnCanvas2->callback(button_cb, NULL);

    end();
}

void Canvas::initGL()
{
    BGL::init();

    Draw3D_init();
}

void Canvas::drawGL()
{
    int w = pixel_w();
    int h = pixel_h();
    glViewport(0, 0, w, h);
    glClearColor(0.08f, 0.8f, 0.8f, 1.0f);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Draw3D_draw(w, h);
/*
    if (gl_version_major >= 3 && !shaderProgram)
    {

    GLuint  vs;
    GLuint  fs;
    int Mslv, mslv; // major and minor version numbers of the shading language
    sscanf((char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%d.%d", &Mslv, &mslv);
    add_output("Shading Language Version=%d.%d",Mslv, mslv);
    const char *vss_format=""
    "#version %d%d\n\
    uniform vec2 p;\
    in vec4 position;\
    in vec4 colour;\
    out vec4 colourV;\
    void main (void)\
    {\
    colourV = colour;\
    gl_Position = vec4(p, 0.0, 0.0) + position;\
    }";

    char vss_string[300];
    const char *vss = vss_string;
    snprintf(vss_string, 300, vss_format, Mslv, mslv);
    const char *fss_format="#version %d%d\n\
    in vec4 colourV;\
    out vec4 fragColour;\
    void main(void)\
    {\
    fragColour = colourV;\
    }";
    char fss_string[200];
    const char *fss = fss_string;
    snprintf(fss_string, 200, fss_format, Mslv, mslv);

    GLint err;
    GLchar CLOG[1000];
    GLsizei length;
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vss, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &err);
    if (err != GL_TRUE)
    {
        glGetShaderInfoLog(vs, sizeof(CLOG), &length, CLOG);
        add_output("vs ShaderInfoLog=%s",CLOG);
    }

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fss, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &err);
    if (err != GL_TRUE)
    {
        glGetShaderInfoLog(fs, sizeof(CLOG), &length, CLOG);
        add_output("fs ShaderInfoLog=%s",CLOG);
    }
    // Attach the shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glBindFragDataLocation(shaderProgram, 0, "fragColour");
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &err);
    if (err != GL_TRUE)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(CLOG), &length, CLOG);
        add_output("link log=%s", CLOG);
    }
    // Get pointers to uniforms and attributes
    positionUniform = glGetUniformLocation(shaderProgram, "p");
    colourAttribute = glGetAttribLocation(shaderProgram, "colour");
    positionAttribute = glGetAttribLocation(shaderProgram, "position");
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Upload vertices (1st four values in a row) and colours (following four values)
    GLfloat vertexData[]= { -0.5,-0.5,0.0,1.0,   1.0,0.0,0.0,1.0,
    -0.5, 0.5,0.0,1.0,   0.0,1.0,0.0,1.0,
    0.5, 0.5,0.0,1.0,   0.0,0.0,1.0,1.0,
    0.5,-0.5,0.0,1.0,   1.0,1.0,1.0,1.0};
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4*8*sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray((GLuint)positionAttribute);
    glEnableVertexAttribArray((GLuint)colourAttribute  );
    glVertexAttribPointer((GLuint)positionAttribute, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
    glVertexAttribPointer((GLuint)colourAttribute  , 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (char*)0+4*sizeof(GLfloat));
    glUseProgram(shaderProgram);
    }
    else if ((!valid())) {
    glViewport(0, 0, pixel_w(), pixel_h());
    }
    glClearColor(0.08f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (shaderProgram) {
    GLfloat p[]={0,0};
    glUniform2fv(positionUniform, 1, (const GLfloat *)&p);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
*/
}

void Canvas::destroyGL()
{
    Draw3D_destroy();
}


void Canvas::resize(int X, int Y, int W, int H)
{
    Fl_Gl_Window::resize(X, Y, W, H);

    const float zoom = Fl::screen_scale(0);

    const int b64 = zoom * 64; // Button Width

    auto& ui = App::getInstance()->ui;

    int x1 = W/2 - b64 - 5;
    int x2 = W/2 + 5;
    int y = H- 5 - b64;
    ui.btnCanvas1->resize(x1,y,b64,b64);
    ui.btnCanvas2->resize(x2,y,b64,b64);
}

#include <GL/glew.h> // Oder Ihr Loader wie glad.h
#include <vector>

// Struktur, die den Zustand beim Erstellen speichert
// und beim Zerstören (am Ende des Scopes) automatisch wiederherstellt.
struct TextureStateSaver {
    GLint maxTextureUnits = 0;
    GLint previousActiveTexture = 0;

    // Arrays zum Speichern der Bindings pro Einheit
    std::vector<GLint> bound2D;
    std::vector<GLint> boundCube;

    TextureStateSaver() {

    }

    ~TextureStateSaver() {

    }

    void push()
    {
        // 1. Aktuell aktive Textureinheit sichern
        glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);

        // 2. Maximale Anzahl an kombinierten Einheiten ermitteln
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

        bound2D.resize(maxTextureUnits);
        boundCube.resize(maxTextureUnits);

        // 3. Zustand aller Textureinheiten auslesen und sichern
        for (GLint i = 0; i < maxTextureUnits; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound2D[i]);
            glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &boundCube[i]);
            // Fügen Sie hier bei Bedarf GL_TEXTURE_BINDING_3D / 2D_ARRAY hinzu
        }
    }

    void pop()
    {
        // 4. Zustand im Destruktor komplett wiederherstellen
        for (GLint i = 0; i < maxTextureUnits; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, bound2D[i]);
            glBindTexture(GL_TEXTURE_CUBE_MAP, boundCube[i]);
        }

        // Ursprünglich aktive Einheit als letztes zurücksetzen
        glActiveTexture(previousActiveTexture);
    }
};


void Canvas::draw()
{
    GLint current_prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_prog);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    TextureStateSaver state;

    state.push();

    drawGL();

    state.pop();

    glPopAttrib();

    glUseProgram(current_prog);

    Fl_Gl_Window::draw(); // Draw FLTK child widgets.
}

int Canvas::handle(int event)
{
    static int first = 1;
    if (first && event == FL_SHOW && shown())
    {
        first = 0;
        make_current();

        initGL();

        add_output("Using GLEW %s", glewGetString(GLEW_VERSION));
        const uchar *glv = glGetString(GL_VERSION);
        add_output("GL_VERSION=%s", glv);
        sscanf((const char *)glv, "%d", &gl_version_major);

        if (gl_version_major < 3)
        {
            add_output("\nThis platform does not support OpenGL V3 :\n"
            "FLTK widgets will appear but the programmed "
            "rendering pipeline will not run.");
            mode(mode() & ~FL_OPENGL3);
        }

        redraw();
    }

    int retval = Fl_Gl_Window::handle(event);
    if (retval) return retval;

    if (event == FL_PUSH && gl_version_major >= 3)
    {
/*
        static float factor = 1.1f;
        GLfloat data[4];
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, 4*sizeof(GLfloat), data);
        if (data[0] < -0.88 || data[0] > -0.5)
        {
            factor = 1/factor;
        }
        data[0] *= factor;
        glBufferSubData(GL_ARRAY_BUFFER, 0, 4*sizeof(GLfloat), data);
        glGetBufferSubData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), 4*sizeof(GLfloat), data);
        data[0] *= factor;
        glBufferSubData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), 4*sizeof(GLfloat), data);
        redraw();
        add_output("push  Fl_Gl_Window::pixels_per_unit()=%.1f", pixels_per_unit());
        return 1;
*/
    }
    return retval;
}

void Canvas::reset()
{
    shaderProgram = 0;
    gl_texture_reset();
}

/*

Fl_Text_Display *output; // shared between output_win() and add_output()

void output_win(SimpleGL3Window *gl)
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

    Fl_Window *topwin = new Fl_Window(800, 300);

    SimpleGL3Window *win = new SimpleGL3Window(0, 0, 300, 300);
    win->end();

    output_win(win);

    add_widgets(win);

    topwin->end();
    topwin->resizable(win);
    topwin->label("Click GL panel to reshape");
    topwin->show(argc, argv);
    Fl::run();
}

*/
