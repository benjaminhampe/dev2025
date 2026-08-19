#pragma once
#include <GLFW/glfw3.h>
#include <thread>
#include <atomic>

class ExtraGLWindow {
public:
    ExtraGLWindow(int width, int height, const char* title)
        : m_width(width), m_height(height), m_title(title),
          m_running(false), m_window(nullptr), m_glfwInitialized(false) {}

    ~ExtraGLWindow() {
        stop();
    }

    void start() {
        if (m_running) return;
        m_running = true;
        m_renderThread = std::thread(&ExtraGLWindow::run, this);
    }

    void stop() {
        m_running = false;
        if (m_renderThread.joinable())
            m_renderThread.join();
        if (m_window)
            glfwDestroyWindow(m_window);
        if (m_glfwInitialized)
            glfwTerminate();
    }

    // V2:
    void show() {
        if (m_window)
            glfwShowWindow(m_window);
    }

    void hide() {
        if (m_window)
            glfwHideWindow(m_window);
    }

    void bringToFront() {
        if (m_window) {
            glfwFocusWindow(m_window);
            glfwRestoreWindow(m_window);
        }
    }

    // V3:
    bool isVisible() const {
        return m_window && glfwGetWindowAttrib(m_window, GLFW_VISIBLE);
    }

private:
    int m_width;
    int m_height;
    const char* m_title;
    std::atomic<bool> m_running;
    GLFWwindow* m_window;
    std::thread m_renderThread;
    bool m_glfwInitialized;

    void run() {
        if (!glfwInit()) {
            // GLFW already initialized by Rack — skip termination
            m_glfwInitialized = false;
        } else {
            m_glfwInitialized = true;
        }

        m_window = glfwCreateWindow(m_width, m_height, m_title, NULL, NULL);
        if (!m_window) {
            if (m_glfwInitialized)
                glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        // V1:
        while (m_running && !glfwWindowShouldClose(m_window)) 
        {
            glfwMakeContextCurrent(m_window);
            glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Custom OpenGL drawing here
            drawDemoLines();
            
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }

        // V2:
        while (m_running) 
        {
            if (glfwWindowShouldClose(m_window)) 
            {
                glfwHideWindow(m_window);
                glfwSetWindowShouldClose(m_window, GLFW_FALSE); // prevent actual close
            }

            glfwMakeContextCurrent(m_window);
            
            // Custom OpenGL drawing here
            drawDemoLines();
            
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }


        glfwDestroyWindow(m_window);
        m_window = nullptr;
        if (m_glfwInitialized)
            glfwTerminate();
    }

    void drawDemoLines() {
        // Set up viewport and projection
        glViewport(0, 0, 800, 600);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);  // 2D projection
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Clear background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw lines
        glLineWidth(2.0f);
        glBegin(GL_LINES);

        // Red diagonal
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(100, 100);
        glVertex2f(700, 500);

        // Green horizontal
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(100, 300);
        glVertex2f(700, 300);

        // Blue vertical
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(400, 100);
        glVertex2f(400, 500);

        glEnd();
    }

};

