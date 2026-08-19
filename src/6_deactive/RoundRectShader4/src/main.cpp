// main.cpp
#include <de_opengl.h>
#include <GLFW/glfw3.h>
#include <iostream>

//
// ─────────────────────────────────────────────────────────────
// Vertex Shader
// ─────────────────────────────────────────────────────────────
//
static const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 aPos;

out vec2 vUV;

void main()
{
    // Convert NDC [-1,1] to UV [0,1]
    vUV = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

//
// ─────────────────────────────────────────────────────────────
// Fragment Shader
// Sharp edges + smooth corners ONLY
// ─────────────────────────────────────────────────────────────
//
static const char* kFragmentShader = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec2 uResolution;

// Rectangle parameters (in pixels)
uniform vec2  uRectCenter;   // center of rectangle
uniform vec2  uRectSize;     // half-size (width/2, height/2)
uniform float uRadius;       // corner radius

void main()
{
    // Background (yellow)
    vec3 bg = vec3(1.0, 1.0, 0.0);
    // Foreground (red)
    vec3 fg = vec3(1.0, 0.0, 0.0);

    // Pixel coordinates
    vec2 frag = vUV * uResolution;

    // Position relative to rectangle center
    vec2 p = frag - uRectCenter;

    vec2 size  = uRectSize;
    float r    = uRadius;

    // ─────────────────────────────────────────────
    // Compute the "inner box":
    // This is the region where edges are straight.
    //
    // Example:
    //   size  = (100, 50)
    //   r     = 20
    //   inner = (80, 30)
    //
    // Any pixel with |p.x| <= 80 AND |p.y| <= 30
    // is inside the sharp-edged core.
    // ─────────────────────────────────────────────
    vec2 inner = size - vec2(r);
    vec2 q = abs(p);

    bool inCore      = (q.x <= inner.x && q.y <= inner.y);
    bool outsideRect = (q.x >  size.x  || q.y >  size.y);
    bool inCorner    = (!inCore && !outsideRect);

    float alpha = 0.0;

    if (inCore) {
        // ─────────────────────────────────────────────
        // SHARP STRAIGHT EDGES
        // No SDF, no AA, no smoothing.
        // ─────────────────────────────────────────────
        alpha = 1.0;
    }
    else if (inCorner) {
        // ─────────────────────────────────────────────
        // CORNER REGION ONLY
        //
        // Compute SDF for quarter circle:
        //   d = distance from pixel to rounded corner arc
        //
        // Example:
        //   q - inner = how far outside the inner box we are
        //   max(q-inner,0) clamps to corner region
        //   length(...) gives distance to arc
        //   subtract radius → signed distance
        // ─────────────────────────────────────────────
        float d = length(max(q - inner, 0.0)) - r;

        // Pixel-wide smoothing
        float aa = fwidth(d);

        // Smooth only near the arc
        alpha = smoothstep(0.0, -aa, d);
    }
    else {
        // Outside rectangle entirely
        alpha = 0.0;
    }

    vec3 color = mix(bg, fg, alpha);
    FragColor = vec4(color, 1.0);
}
)";

//
// ─────────────────────────────────────────────────────────────
// OpenGL setup
// ─────────────────────────────────────────────────────────────
//
static GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        std::cerr << "Shader error:\n" << log << std::endl;
    }
    return s;
}

static GLuint createProgram(const char* vs, const char* fs)
{
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
        std::cerr << "Link error:\n" << log << std::endl;
    }

    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int winW = 800, winH = 600;
    GLFWwindow* window = glfwCreateWindow(winW, winH, "Sharp Edges + Smooth Corners", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

	ensureDesktopOpenGL();

    // gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLuint prog = createProgram(kVertexShader, kFragmentShader);

    // Fullscreen quad
    float quad[] = {
        -1.f, -1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f,  1.f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

    GLint locResolution = glGetUniformLocation(prog, "uResolution");
    GLint locCenter     = glGetUniformLocation(prog, "uRectCenter");
    GLint locSize       = glGetUniformLocation(prog, "uRectSize");
    GLint locRadius     = glGetUniformLocation(prog, "uRadius");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &winW, &winH);
        glViewport(0, 0, winW, winH);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glUniform2f(locResolution, (float)winW, (float)winH);

        // Rectangle parameters
        float rectW = winW * 0.6f;
        float rectH = winH * 0.4f;
        float radius = 60.0f;

        glUniform2f(locCenter, winW * 0.5f, winH * 0.5f);
        glUniform2f(locSize, rectW * 0.5f, rectH * 0.5f);
        glUniform1f(locRadius, radius);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
    }

    return 0;
}
