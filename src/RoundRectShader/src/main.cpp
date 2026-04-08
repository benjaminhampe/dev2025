// main.cpp
#include <de_opengl.h>
#include <GLFW/glfw3.h>
#include <iostream>

static const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 aPos;

out vec2 vUV;

void main()
{
    // aPos is in NDC [-1,1], map to [0,1] UV
    vUV = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static const char* kFragmentShader = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec2 uResolution;

// Rounded rect parameters (in screen space)
uniform vec2  uRectCenter;   // center in pixels
uniform vec2  uRectSize;     // half-size in pixels (width/2, height/2)
uniform float uRadius;       // corner radius in pixels

// Signed distance for rounded rect corners
float sdRoundRect(vec2 p, vec2 size, float radius)
{
    vec2 q = abs(p) - size + vec2(radius);
    return length(max(q, 0.0)) - radius;
}

void main()
{
    // Background: solid yellow
    vec3 bgColor = vec3(1.0, 1.0, 0.0); // yellow
    vec3 fgColor = vec3(1.0, 0.0, 0.0); // red

    // Convert UV to pixel coordinates
    vec2 fragCoord = vUV * uResolution;

    // Position relative to rect center
    vec2 p = fragCoord - uRectCenter;

    // Core rectangle region (without rounded corners)
    vec2 size = uRectSize;
    float r = uRadius;

    // Inner box where edges should be perfectly sharp
    vec2 inner = size - vec2(r);
    vec2 q = abs(p);

    // insideCore = 1 in the central box (no AA needed), 0 outside
    float insideCore = step(q.x, inner.x) * step(q.y, inner.y);

    // Distance for the rounded corners
    float dCorner = sdRoundRect(p, size, r);

    // Antialias only near the corners
    float aa = fwidth(dCorner);
    float cornerAlpha = smoothstep(0.0, -aa, dCorner); // 1 inside, 0 outside

    // Combine: sharp in core, smooth at corners
    float alpha = max(insideCore, cornerAlpha);

    vec3 color = mix(bgColor, fgColor, alpha);
    FragColor = vec4(color, 1.0);
}
)";

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
        std::cerr << "Shader compile error: " << log << std::endl;
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
        std::cerr << "Program link error: " << log << std::endl;
    }
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int winW = 800;
    int winH = 600;
    GLFWwindow* window = glfwCreateWindow(winW, winH, "Rounded Rect AA", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ensureDesktopOpenGL();

    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     std::cerr << "Failed to init GLAD\n";
    //     return 1;
    // }

    GLuint prog = createProgram(kVertexShader, kFragmentShader);

    // Fullscreen quad (NDC)
    float quadVerts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);

        // Set uniforms
        glUniform2f(locResolution, (float)winW, (float)winH);

        // Rounded rect parameters
        float rectW = winW * 0.6f;
        float rectH = winH * 0.4f;
        float radius = 40.0f;

        glUniform2f(locCenter, winW * 0.5f, winH * 0.5f);
        glUniform2f(locSize, rectW * 0.5f, rectH * 0.5f);
        glUniform1f(locRadius, radius);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(prog);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
