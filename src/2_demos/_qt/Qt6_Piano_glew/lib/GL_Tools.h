/*
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Vertex shader source code
const char* g_vertexShaderSource = R"(
    #version 330 core
    uniform vec4 u_screenRect; // x, y, width, height in pixel space
    uniform vec2 u_screenResolution;

    out vec2 v_texCoord;

    void main()
    {
        // Define rectangle vertices in screen space (pixel coordinates)
        vec2 screenVertices[4] = vec2[4](
            vec2(u_screenRect.x, u_screenRect.y + u_screenRect.w), // Top-left
            vec2(u_screenRect.x + u_screenRect.z, u_screenRect.y + u_screenRect.w), // Top-right
            vec2(u_screenRect.x + u_screenRect.z, u_screenRect.y), // Bottom-right
            vec2(u_screenRect.x, u_screenRect.y) // Bottom-left
        );
        
        // Compute position in normalized device coordinates (NDC)
        vec2 screenPos = screenVertices[gl_VertexID] / u_screenResolution * 2.0 - 1.0;
        
        // Output position and texture coordinates
        gl_Position = vec4(screenPos, 0.0, 1.0);
        v_texCoord = vec2((gl_VertexID & 1) * 1.0, ((gl_VertexID >> 1) & 1) * 1.0);
    }
)";

// Fragment shader source code
const char* g_fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec2 v_texCoord;
    
    uniform vec4 u_screenRect; // x, y, width, height in pixel space
    uniform float u_screenRadius;
    uniform vec4 u_screenColor;
    uniform vec2 u_screenResolution;
    uniform sampler2D u_screenTexture;

    void main()
    {
        vec2 screenUV = gl_FragCoord.xy;
        vec2 screenRectPos = u_screenRect.xy;
        vec2 screenRectSize = u_screenRect.zw;
        float screenRadius = u_screenRadius;

        // Calculate relative position within the rectangle
        vec2 p = screenUV - screenRectPos - screenRectSize * 0.5;
        
        // Calculate distance to the nearest edge/corner
        vec2 q = abs(p) - screenRectSize * 0.5 + vec2(screenRadius, screenRadius);
        float dist = length(max(q, 0.0)) - screenRadius;

        // Set fragment color based on distance, and blend with texture
        if (dist < 0.0)
            FragColor = texture(u_screenTexture, v_texCoord) * u_screenColor; // Apply color and texture
        else
            FragColor = vec4(0.0, 0.0, 0.0, 0.0); // Transparent
    }
)";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int compileShader(unsigned int type, const char* source);
unsigned int createShaderProgram();
unsigned int loadTexture(const char* path);

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* m_window = glfwCreateWindow(800, 600, "Rounded Rectangle", NULL, NULL);
    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int m_shaderProgram = createShaderProgram();
    unsigned int m_texture = loadTexture("path/to/your/texture.png");

    while (!glfwWindowShouldClose(m_window))
    {
        processInput(m_window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_shaderProgram);

        int u_screenResolutionLoc = glGetUniformLocation(m_shaderProgram, "u_screenResolution");
        glUniform2f(u_screenResolutionLoc, 800.0f, 600.0f);

        int u_screenRectLoc = glGetUniformLocation(m_shaderProgram, "u_screenRect");
        glUniform4f(u_screenRectLoc, 200.0f, 150.0f, 400.0f, 300.0f); // x, y, width, height

        int u_screenRadiusLoc = glGetUniformLocation(m_shaderProgram, "u_screenRadius");
        glUniform1f(u_screenRadiusLoc, 50.0f);

        int u_screenColorLoc = glGetUniformLocation(m_shaderProgram, "u_screenColor");
        glUniform4f(u_screenColorLoc, 1.0f, 0.0f, 0.0f, 1.0f); // Red color

        glBindTexture(GL_TEXTURE_2D, m_texture);
        glUniform1i(glGetUniformLocation(m_shaderProgram, "u_screenTexture"), 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glDeleteProgram(m_shaderProgram);
    glDeleteTextures(1, &m_texture);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* m_window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* m_window)
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
}

unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int createShaderProgram()
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, g_vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, g_fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int loadTexture(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    return texture;
}

*/
