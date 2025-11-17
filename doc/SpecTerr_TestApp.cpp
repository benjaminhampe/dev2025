// TestApp.cpp - GLFW-based test application for Spectrum 3D Terrain VST
// Author: MiniMax Agent
// Date: 2025-11-12

#include "Spectrum3DTerrainVST.h"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <fstream>
#include <sstream>

// Global variables for audio simulation
class AudioSimulator {
public:
    float sampleRate;
    float frequency;
    float amplitude;
    std::vector<float> channels[8];
    
    AudioSimulator() : sampleRate(44100.0f), frequency(440.0f), amplitude(0.5f) {
        for (int i = 0; i < 8; i++) {
            channels[i].resize(1024);
        }
    }
    
    void generateSamples(float** outputs, int numChannels, int sampleFrames) {
        for (int ch = 0; ch < numChannels && ch < 8; ch++) {
            for (int i = 0; i < sampleFrames; i++) {
                float time = (currentTime + i) / sampleRate;
                
                // Generate different tones for each channel
                float freq = frequency * (1.0f + ch * 0.5f);
                
                // Add some harmonics and modulation
                float sample = amplitude * (std::sin(2.0f * M_PI * freq * time) +
                                           0.3f * std::sin(2.0f * M_PI * freq * 2.0f * time) +
                                           0.1f * std::sin(2.0f * M_PI * freq * 3.0f * time));
                
                // Add some modulation
                sample *= (1.0f + 0.2f * std::sin(2.0f * M_PI * 2.0f * time));
                
                // Add noise
                sample += 0.01f * ((float)rand() / RAND_MAX - 0.5f);
                
                outputs[ch][i] = sample;
            }
        }
        currentTime += sampleFrames;
    }
    
    void setFrequency(float freq) {
        frequency = freq;
    }
    
    void setAmplitude(float amp) {
        amplitude = amp;
    }
    
private:
    float currentTime = 0.0f;
};

// Global application state
static AudioSimulator g_audioSim;
static GLFWwindow* g_window = nullptr;
static Spectrum3DTerrainVST* g_plugin = nullptr;
static bool g_running = true;

// GLFW error callback
void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << error << " - " << description << std::endl;
}

// Keyboard callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                g_running = false;
                break;
            case GLFW_KEY_SPACE:
                // Reset camera
                if (g_plugin) {
                    g_plugin->cameraAngleX = 0.0f;
                    g_plugin->cameraAngleY = 0.3f;
                    g_plugin->cameraDistance = 3.0f;
                }
                break;
            case GLFW_KEY_1:
                if (g_plugin) {
                    g_plugin->setParameter(Spectrum3DTerrainVST::PARAM_FFT_SIZE, 0.0f); // 128
                }
                break;
            case GLFW_KEY_2:
                if (g_plugin) {
                    g_plugin->setParameter(Spectrum3DTerrainVST::PARAM_FFT_SIZE, 0.33f); // 512
                }
                break;
            case GLFW_KEY_3:
                if (g_plugin) {
                    g_plugin->setParameter(Spectrum3DTerrainVST::PARAM_FFT_SIZE, 0.66f); // 2048
                }
                break;
            case GLFW_KEY_F1:
                if (g_plugin) {
                    bool current = g_plugin->getParameter(Spectrum3DTerrainVST::PARAM_COMBINED_MODE) > 0.5f;
                    g_plugin->setParameter(Spectrum3DTerrainVST::PARAM_COMBINED_MODE, current ? 0.0f : 1.0f);
                }
                break;
            case GLFW_KEY_F2:
                if (g_plugin) {
                    float current = g_plugin->getParameter(Spectrum3DTerrainVST::PARAM_CHANNEL_SELECT);
                    float next = std::min(0.875f, current + 0.125f);
                    g_plugin->setParameter(Spectrum3DTerrainVST::PARAM_CHANNEL_SELECT, next);
                }
                break;
            case GLFW_KEY_F3:
                if (g_plugin) {
                    float current = g_plugin->getParameter(Spectrum3DTerrainVST::PARAM_HISTORY_LENGTH);
                    float next = std::min(0.98f, current + 0.02f);
                    g_plugin->setParameter(Spectrum3DTerrainVST::PARAM_HISTORY_LENGTH, next);
                }
                break;
            case GLFW_KEY_UP:
                g_audioSim.setFrequency(g_audioSim.frequency * 1.05946f); // +1 semitone
                break;
            case GLFW_KEY_DOWN:
                g_audioSim.setFrequency(g_audioSim.frequency / 1.05946f); // -1 semitone
                break;
            case GLFW_KEY_LEFT:
                g_audioSim.setAmplitude(std::max(0.0f, g_audioSim.amplitude - 0.1f));
                break;
            case GLFW_KEY_RIGHT:
                g_audioSim.setAmplitude(std::min(1.0f, g_audioSim.amplitude + 0.1f));
                break;
        }
    }
}

// Mouse button callback
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (g_plugin) {
        bool pressed = (action == GLFW_PRESS);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            g_plugin->mouseButtonPressed[0] = pressed;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            g_plugin->mouseButtonPressed[2] = pressed;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            g_plugin->mouseButtonPressed[1] = pressed;
        }
    }
}

// Mouse position callback
void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (g_plugin) {
        static double lastX = 0, lastY = 0;
        
        if (g_plugin->mouseButtonPressed[0]) {
            double dx = x - lastX;
            double dy = y - lastY;
            g_plugin->cameraAngleX += dx * 0.01f;
            g_plugin->cameraAngleY = std::max(-1.5f, std::min(1.5f, g_plugin->cameraAngleY + dy * 0.01f));
        }
        
        lastX = x;
        lastY = y;
        g_plugin->mouseX = x;
        g_plugin->mouseY = y;
    }
}

// Scroll callback
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (g_plugin) {
        g_plugin->cameraDistance = std::max(1.0f, std::min(10.0f, g_plugin->cameraDistance + (float)yOffset * 0.1f));
    }
}

// Window resize callback
void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Render text overlay (simplified)
void renderTextOverlay() {
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Background for text
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(10, 550);
    glVertex2f(790, 550);
    glVertex2f(790, 590);
    glVertex2f(10, 590);
    glEnd();
    
    // Text color
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Instructions
    glBegin(GL_LINES);
    glEnd();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

// Print help information
void printHelp() {
    std::cout << "\n=== Spectrum 3D Terrain VST Plugin Test Application ===\n" << std::endl;
    std::cout << "Mouse Controls:" << std::endl;
    std::cout << "  Left Mouse:   Rotate camera" << std::endl;
    std::cout << "  Right Mouse:  Zoom in/out" << std::endl;
    std::cout << "  Scroll Wheel: Zoom in/out" << std::endl;
    std::cout << "\nKeyboard Controls:" << std::endl;
    std::cout << "  ESC:          Exit application" << std::endl;
    std::cout << "  SPACE:        Reset camera" << std::endl;
    std::cout << "  1,2,3:        Set FFT size (128, 512, 2048)" << std::endl;
    std::cout << "  F1:           Toggle combined/single channel mode" << std::endl;
    std::cout << "  F2:           Switch to next channel" << std::endl;
    std::cout << "  F3:           Increase history length" << std::endl;
    std::cout << "  UP/DOWN:      Increase/decrease frequency" << std::endl;
    std::cout << "  LEFT/RIGHT:   Decrease/increase amplitude" << std::endl;
    std::cout << "\nCurrent Settings:" << std::endl;
    std::cout << "  Frequency:    " << g_audioSim.frequency << " Hz" << std::endl;
    std::cout << "  Amplitude:    " << g_audioSim.amplitude << std::endl;
    std::cout << "\n========================================================\n" << std::endl;
}

// Main application
int main() {
    std::cout << "Starting Spectrum 3D Terrain VST Plugin Test Application..." << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwSetErrorCallback(errorCallback);
    
    // Create OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    
    // Create window
    g_window = glfwCreateWindow(800, 600, "Spectrum 3D Terrain VST Plugin", nullptr, nullptr);
    if (!g_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1); // VSync
    
    // Set callbacks
    glfwSetKeyCallback(g_window, keyCallback);
    glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
    glfwSetCursorPosCallback(g_window, cursorPosCallback);
    glfwSetScrollCallback(g_window, scrollCallback);
    glfwSetWindowSizeCallback(g_window, windowSizeCallback);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(g_window);
        glfwTerminate();
        return -1;
    }
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
    
    // Create VST plugin
    g_plugin = new Spectrum3DTerrainVST(nullptr); // No audio master in test app
    if (!g_plugin) {
        std::cerr << "Failed to create VST plugin" << std::endl;
        glfwDestroyWindow(g_window);
        glfwTerminate();
        return -1;
    }
    
    // Initialize OpenGL in plugin
    if (!g_plugin->initOpenGL()) {
        std::cerr << "Failed to initialize OpenGL in plugin" << std::endl;
        delete g_plugin;
        glfwDestroyWindow(g_window);
        glfwTerminate();
        return -1;
    }
    
    // Print help
    printHelp();
    
    // Main loop
    auto lastTime = std::chrono::high_resolution_clock::now();
    const float targetFrameTime = 1.0f / 60.0f; // 60 FPS
    
    while (!glfwWindowShouldClose(g_window) && g_running) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Process events
        glfwPollEvents();
        
        // Generate audio data and process through plugin
        const int numChannels = 2;
        const int blockSize = 512;
        
        float* inputs[8];
        float* outputs[8];
        
        static float inputBuffers[8][1024];
        static float outputBuffers[8][1024];
        
        for (int ch = 0; ch < 8; ch++) {
            inputs[ch] = inputBuffers[ch];
            outputs[ch] = outputBuffers[ch];
        }
        
        // Generate audio samples
        g_audioSim.generateSamples(inputs, numChannels, blockSize);
        
        // Process through VST plugin
        g_plugin->processReplacing(inputs, outputs, blockSize);
        
        // Render OpenGL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        g_plugin->renderOpenGL();
        
        // Render text overlay
        renderTextOverlay();
        
        // Swap buffers
        glfwSwapBuffers(g_window);
        
        // Frame rate limiting
        float frameTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - currentTime).count();
        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::microseconds((int)((targetFrameTime - frameTime) * 1000000)));
        }
        
        // Update help text with current settings
        if ((int)(currentTime.time_since_epoch().count() / 10000000) % 10 == 0) {
            system("clear"); // Clear terminal (Linux/Mac)
            printHelp();
        }
    }
    
    // Cleanup
    std::cout << "\nCleaning up..." << std::endl;
    
    delete g_plugin;
    glfwDestroyWindow(g_window);
    glfwTerminate();
    
    std::cout << "Application exited successfully." << std::endl;
    
    return 0;
}