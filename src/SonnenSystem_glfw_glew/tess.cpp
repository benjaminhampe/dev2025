#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

// Shader sources
const char* vertexShaderSrc = R"(
#version 450 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
out vec3 vPosition;
out vec3 vNormal;
void main() {
    vPosition = inPosition;
    vNormal = normalize(inNormal);
}
)";

const char* tessControlShaderSrc = R"(
#version 450 core
layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
out vec3 tcPosition[];
out vec3 tcNormal[];
uniform mat4 viewMatrix;
void main() {
    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
    vec3 viewDir = normalize((viewMatrix * vec4(vPosition[gl_InvocationID], 1.0)).xyz);
    float facing = dot(vNormal[gl_InvocationID], viewDir);
    float tessLevel = mix(4.0, 64.0, clamp(-facing, 0.0, 1.0));
    gl_TessLevelOuter[0] = tessLevel;
    gl_TessLevelOuter[1] = tessLevel;
    gl_TessLevelOuter[2] = tessLevel;
    gl_TessLevelInner[0] = tessLevel;
}
)";

const char* tessEvalShaderSrc = R"(
#version 450 core
layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
in vec3 tcNormal[];
out vec3 teNormal;
out vec3 tePosition;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main() {
    vec3 pos = gl_TessCoord.x * tcPosition[0] +
               gl_TessCoord.y * tcPosition[1] +
               gl_TessCoord.z * tcPosition[2];
    vec3 norm = normalize(gl_TessCoord.x * tcNormal[0] +
                          gl_TessCoord.y * tcNormal[1] +
                          gl_TessCoord.z * tcNormal[2]);
    tePosition = pos;
    teNormal = norm;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 450 core
in vec3 teNormal;
out vec4 fragColor;
void main() {
    fragColor = vec4(teNormal * 0.5 + 0.5, 1.0);
}
)";

// Shader compilation helper
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader error:\n" << log << std::endl;
    }
    return shader;
}

// Sphere generation
void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, int rings, int sectors) {
    float const R = 1.0f / (rings - 1);
    float const S = 1.0f / (sectors - 1);
    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            float y = sin(-M_PI_2 + M_PI * r * R);
            float x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(x); // normal
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    for (int r = 0; r < rings - 1; ++r) {
        for (int s = 0; s < sectors - 1; ++s) {
            int i0 = r * sectors + s;
            int i1 = r * sectors + (s + 1);
            int i2 = (r + 1) * sectors + (s + 1);
            int i3 = (r + 1) * sectors + s;
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Tessellated Sphere", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphere(vertices, indices, 64, 64);

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint tcs = compileShader(GL_TESS_CONTROL_SHADER, tessControlShaderSrc);
    GLuint tes = compileShader(GL_TESS_EVALUATION_SHADER, tessEvalShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, tcs);
    glAttachShader(program, tes);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(vao);
        glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
