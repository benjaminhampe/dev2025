// main.cpp
#include <de_opengl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

//
// ─────────────────────────────────────────────────────────────
//   Vertex Shader
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
//   Fragment Shader
//   Sharp edges + smooth corners only
// ─────────────────────────────────────────────────────────────
//
static const char* kFragmentShader = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec2 uResolution;

// Rounded rectangle parameters (in pixels)
uniform vec2  uRectCenter;   // center of rectangle
uniform vec2  uRectSize;     // half-size (width/2, height/2)
uniform float uRadius;       // corner radius

//
// Signed distance for rounded corners only
//
// Explanation:
//   We want the SDF to operate ONLY in the corner region.
//   The straight edges must remain perfectly sharp.
//
//   Let size = (W/2, H/2)
//   Let inner = size - radius
//
//   The "inner box" is the region where the rectangle has
//   perfectly straight edges. No smoothing should happen there.
//
//   The SDF is computed only when the pixel is outside the
//   inner box, i.e. in the corner region.
//
//   Math example:
//     Suppose size = (100, 50), radius = 20
//     inner = (80, 30)
//
//     Any pixel with |p.x| <= 80 AND |p.y| <= 30
//     is inside the sharp-edged core.
//
//     Only when |p.x| > 80 OR |p.y| > 30
//     do we compute the rounded-corner SDF.
//
//
float sdRoundCorner(vec2 p, vec2 inner, float radius)
{
    // q = how far outside the inner box we are
    vec2 q = abs(p) - inner;

    // max(q,0) keeps only the positive part (outside region)
    // length(max(q,0)) gives distance to the quarter circle
    return length(max(q, 0.0)) - radius;
}

void main()
{
    // Background color (yellow)
    vec3 bg = vec3(1.0, 1.0, 0.0);

    // Foreground color (red)
    vec3 fg = vec3(1.0, 0.0, 0.0);

    // Convert UV to pixel coordinates
    vec2 frag = vUV * uResolution;

    // Position relative to rectangle center
    vec2 p = frag - uRectCenter;

    vec2 size  = uRectSize;
    float r    = uRadius;

    // Compute the inner sharp-edged box
    vec2 inner = size - vec2(r);

    // ─────────────────────────────────────────────
    // 1. SHARP CORE MASK
    //    insideCore = 1 inside the inner box
    //    insideCore = 0 outside
    //
    //    This produces PERFECTLY SHARP EDGES.
    // ─────────────────────────────────────────────
    vec2 q = abs(p);
    float insideCore =
        step(q.x, inner.x) *
        step(q.y, inner.y);

    // ─────────────────────────────────────────────
    // 2. CORNER SDF (only active outside inner box)
    // ─────────────────────────────────────────────
    float d = sdRoundCorner(p, inner, r);

    // ─────────────────────────────────────────────
    // 3. ANTIALIASING ONLY IN CORNER REGION
    //
    //    fwidth(d) ≈ how fast d changes across pixels
    //    smoothstep(0, -aa, d) gives:
    //      1 inside
    //      0 outside
    //      smooth transition only near d=0
    //
    //    Because d is only meaningful in the corner region,
    //    the straight edges remain perfectly sharp.
    // ─────────────────────────────────────────────
    float aa = fwidth(d);
    float cornerAlpha = smoothstep(0.0, -aa, d);

    // ─────────────────────────────────────────────
    // 4. COMBINE
    //
    //    alpha = 1 in sharp core
    //    alpha = smooth cornerAlpha in corner region
    //
    //    max() ensures corners blend correctly.
    // ─────────────────────────────────────────────
    float alpha = max(insideCore, cornerAlpha);

    // Mix background and foreground
    vec3 color = mix(bg, fg, alpha);
    FragColor = vec4(color, 1.0);
}
)";

//
// ─────────────────────────────────────────────────────────────
//   GLFW + GLAD setup
// ─────────────────────────────────────────────────────────────
//
static void glfwErrorCallback(int code, const char* desc)
{
    std::cerr << "GLFW Error " << code << ": " << desc << std::endl;
}

static GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(s, len, nullptr, log.data());
        std::cerr << "Shader compile error:\n" << log << std::endl;
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

    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(p, len, nullptr, log.data());
        std::cerr << "Program link error:\n" << log << std::endl;
    }

    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int winW = 800, winH = 600;
    GLFWwindow* window = glfwCreateWindow(winW, winH, "Sharp Edges + Smooth Corners", nullptr, nullptr);
    if (!window) return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

	ensureDesktopOpenGL();
	
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // std::cerr << "Failed to init GLAD\n";
        // return 1;
    // }

    GLuint prog = createProgram(kVertexShader, kFragmentShader);

    // Fullscreen quad
    float quadVerts[] = {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glBindVertexArray(0);

    glUseProgram(prog);
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

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(prog);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
