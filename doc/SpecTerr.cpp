// Spectrum3DTerrainVST.cpp - VST 2.4 Plugin Implementation
// Author: MiniMax Agent
// Date: 2025-11-12

#include "Spectrum3DTerrainVST.h"
#include <pffft.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// =============================================================================
// Additional Implementation Details
// =============================================================================

// Global instance for GLFW callbacks
Spectrum3DTerrainVST* g_pluginInstance = nullptr;
GLFWwindow* g_window = nullptr;

// =============================================================================
// GLFW Helper Functions
// =============================================================================

void setupGLFWCallbacks(GLFWwindow* window, Spectrum3DTerrainVST* plugin) {
    g_pluginInstance = plugin;
    g_window = window;
    
    glfwSetWindowUserPointer(window, plugin);
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        if (g_pluginInstance) {
            bool pressed = (action == GLFW_PRESS);
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                g_pluginInstance->mouseButtonPressed[0] = pressed;
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                g_pluginInstance->mouseButtonPressed[2] = pressed;
            } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                g_pluginInstance->mouseButtonPressed[1] = pressed;
            }
        }
    });
    
    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
        if (g_pluginInstance) {
            static double lastX = 0, lastY = 0;
            
            if (g_pluginInstance->mouseButtonPressed[0]) {
                double dx = x - lastX;
                double dy = y - lastY;
                g_pluginInstance->cameraAngleX += dx * 0.01f;
                g_pluginInstance->cameraAngleY = std::max(-1.5f, std::min(1.5f, g_pluginInstance->cameraAngleY + dy * 0.01f));
            }
            
            lastX = x;
            lastY = y;
            g_pluginInstance->mouseX = x;
            g_pluginInstance->mouseY = y;
        }
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xOffset, double yOffset) {
        if (g_pluginInstance) {
            g_pluginInstance->cameraDistance = std::max(1.0f, std::min(10.0f, g_pluginInstance->cameraDistance + (float)yOffset * 0.1f));
        }
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        if (g_pluginInstance && action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    g_pluginInstance->parameters[Spectrum3DTerrainVST::PARAM_SHOW_UI] = 
                        (g_pluginInstance->parameters[Spectrum3DTerrainVST::PARAM_SHOW_UI] > 0.5f) ? 0.0f : 1.0f;
                    break;
                case GLFW_KEY_SPACE:
                    g_pluginInstance->cameraAngleX = 0.0f;
                    g_pluginInstance->cameraAngleY = 0.3f;
                    g_pluginInstance->cameraDistance = 3.0f;
                    break;
                case GLFW_KEY_1:
                    g_pluginInstance->parameters[Spectrum3DTerrainVST::PARAM_FFT_SIZE] = 0.0f; // 128
                    g_pluginInstance->updateParameters();
                    break;
                case GLFW_KEY_2:
                    g_pluginInstance->parameters[Spectrum3DTerrainVST::PARAM_FFT_SIZE] = 0.33f; // 512
                    g_pluginInstance->updateParameters();
                    break;
                case GLFW_KEY_3:
                    g_pluginInstance->parameters[Spectrum3DTerrainVST::PARAM_FFT_SIZE] = 0.66f; // 2048
                    g_pluginInstance->updateParameters();
                    break;
            }
        }
    });
}

// =============================================================================
// Enhanced FFT Implementation with PFFFT
// =============================================================================

struct PFFFTContext {
    pffft_plan_t planForward;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    int fftSize;
    bool initialized;
    
    PFFFTContext() : planForward(nullptr), fftSize(0), initialized(false) {}
    
    ~PFFFTContext() {
        if (initialized) {
            pffft_destroy_plan(planForward);
        }
    }
    
    bool initialize(int size) {
        if (initialized && fftSize == size) return true;
        
        if (initialized) {
            pffft_destroy_plan(planForward);
        }
        
        planForward = pffft_new_plan(size, nullptr, PFFFT_FORWARD, 0);
        if (!planForward) return false;
        
        inputBuffer.resize(size);
        outputBuffer.resize(size * 2); // Complex output
        
        fftSize = size;
        initialized = true;
        return true;
    }
    
    void compute(const float* input, float* magnitude) {
        if (!initialized || !planForward) return;
        
        std::copy(input, input + fftSize, inputBuffer.begin());
        
        // Apply PFFFT
        pffft_transform_ordered(planForward, inputBuffer.data(), outputBuffer.data(), nullptr, 0);
        
        // Compute magnitude from complex output
        for (int i = 0; i < fftSize / 2 + 1; i++) {
            float real = outputBuffer[2 * i];
            float imag = outputBuffer[2 * i + 1];
            magnitude[i] = std::sqrt(real * real + imag * imag);
        }
    }
};

// Global PFFFT context
static PFFFTContext g_pffftContext;

// =============================================================================
// Enhanced Audio Processing
// =============================================================================

void Spectrum3DTerrainVST::computeFFT(const std::vector<float>& audioData) {
    if (audioData.size() < currentFFTSize) return;
    
    // Initialize PFFFT if needed
    if (!g_pffftContext.initialize(currentFFTSize)) {
        // Fallback to simple magnitude computation
        for (int i = 0; i < currentFFTSize; i++) {
            fftMagnitudeBuffer[i] = std::abs(audioData[i]);
        }
        convertToDecibels();
        return;
    }
    
    // Copy and apply window function
    std::vector<float> windowedData(currentFFTSize);
    std::copy(audioData.begin(), audioData.begin() + currentFFTSize, windowedData.begin());
    
    applyWindowFunction(windowedData.data(), currentFFTSize);
    
    // Compute FFT using PFFFT
    g_pffftContext.compute(windowedData.data(), fftMagnitudeBuffer.data());
    
    convertToDecibels();
    
    // Apply smoothing if enabled
    float smoothing = parameters[PARAM_SMOOTHING];
    if (smoothing > 0.0f) {
        static std::vector<float> previousDB;
        if (previousDB.size() != currentFFTSize) {
            previousDB.resize(currentFFTSize);
            std::fill(previousDB.begin(), previousDB.end(), -120.0f);
        }
        
        float alpha = smoothing * 0.1f;
        for (int i = 0; i < currentFFTSize; i++) {
            fftDBBuffer[i] = (1.0f - alpha) * previousDB[i] + alpha * fftDBBuffer[i];
            previousDB[i] = fftDBBuffer[i];
        }
    }
}

// =============================================================================
// Enhanced OpenGL Rendering with GLM
// =============================================================================

void Spectrum3DTerrainVST::renderOpenGL() {
    if (!showUI) return;
    
    // Update vertex data
    updateVertexData();
    
    // Clear screen
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Set up matrices using GLM
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    // Create projection matrix
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    
    // Create view matrix (camera)
    float radius = cameraDistance;
    float camX = radius * std::cos(cameraAngleY) * std::sin(cameraAngleX);
    float camY = radius * std::sin(cameraAngleY);
    float camZ = radius * std::cos(cameraAngleY) * std::cos(cameraAngleX);
    
    view = glm::lookAt(
        glm::vec3(camX, camY, camZ),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    // Model matrix for rotation
    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
    
    // Set uniform matrices
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Draw terrain
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, g_lastIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Render UI overlay (simplified)
    renderUIOverlay();
    
    // Handle UI input
    handleUIInput();
}

// Global variable for last index count (set in updateVertexData)
int g_lastIndexCount = 0;

void Spectrum3DTerrainVST::updateVertexData() {
    // Generate terrain vertices from FFT data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    int fftSize = currentFFTSize;
    int histSize = historyLength;
    
    float freqRangeStart = parameters[PARAM_FREQ_RANGE_START];
    float freqRangeEnd = parameters[PARAM_FREQ_RANGE_END];
    float colorScale = parameters[PARAM_COLOR_SCALE] * 10.0f;
    
    // Calculate frequency range in samples
    int startSample = (int)(freqRangeStart * fftSize);
    int endSample = (int)(freqRangeEnd * fftSize);
    startSample = std::max(1, std::min(fftSize / 2 - 1, startSample));
    endSample = std::max(startSample + 1, std::min(fftSize / 2, endSample));
    
    int visibleFFTSize = endSample - startSample;
    
    // Create vertices
    for (int z = 0; z < histSize; z++) {
        for (int x = 0; x < visibleFFTSize; x++) {
            int fftIndex = startSample + x;
            
            float dbValue = historyData[z][fftIndex];
            float normalizedDB = std::max(0.0f, std::min(1.0f, (dbValue + 120.0f) / 120.0f));
            
            // Position (x, y, z)
            float xPos = (float)x / visibleFFTSize - 0.5f;
            float yPos = normalizedDB * colorScale;
            float zPos = (float)z / histSize - 0.5f;
            
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            
            // Color with improved gradient
            std::array<float, 3> color = getColorFromGradient(normalizedDB);
            vertices.push_back(color[0]);
            vertices.push_back(color[1]);
            vertices.push_back(color[2]);
            
            // Create indices for triangles
            if (x < visibleFFTSize - 1 && z < histSize - 1) {
                int current = z * visibleFFTSize + x;
                int next = z * visibleFFTSize + (x + 1);
                int below = (z + 1) * visibleFFTSize + x;
                int belowNext = (z + 1) * visibleFFTSize + (x + 1);
                
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(below);
                
                indices.push_back(next);
                indices.push_back(belowNext);
                indices.push_back(below);
            }
        }
    }
    
    g_lastIndexCount = indices.size();
    
    // Update buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    
    // Configure vertex attributes
    glBindVertexArray(vao);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// =============================================================================
// UI Overlay Rendering
// =============================================================================

void Spectrum3DTerrainVST::renderUIOverlay() {
    // Simple text overlay (would need bitmap font rendering in real implementation)
    // For now, just draw a simple border around the screen
    
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Draw border
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(10, 10);
    glVertex2f(790, 10);
    glVertex2f(790, 10);
    glVertex2f(790, 590);
    glVertex2f(790, 590);
    glVertex2f(10, 590);
    glVertex2f(10, 590);
    glVertex2f(10, 10);
    glEnd();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

// =============================================================================
// GLFW Context Management
// =============================================================================

bool Spectrum3DTerrainVST::initOpenGL() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        return false;
    }
    
    // Initialize camera
    cameraAngleX = 0.0f;
    cameraAngleY = 0.3f;
    cameraDistance = 3.0f;
    
    // Create shaders and buffers
    if (!createShaders()) return false;
    if (!createBuffers()) return false;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glLineWidth(1.0f);
    
    return true;
}

// =============================================================================
// Advanced Color Gradient
// =============================================================================

std::array<float, 3> Spectrum3DTerrainVST::getColorFromGradient(float value) {
    // Enhanced color gradient with more colors
    std::array<float, 3> color;
    value = std::max(0.0f, std::min(1.0f, value));
    
    if (value < 0.2f) {
        // Dark Blue to Blue
        float t = value / 0.2f;
        color = {0.0f, 0.0f, 0.5f + t * 0.5f};
    } else if (value < 0.4f) {
        // Blue to Cyan
        float t = (value - 0.2f) / 0.2f;
        color = {0.0f, t, 1.0f};
    } else if (value < 0.6f) {
        // Cyan to Green
        float t = (value - 0.4f) / 0.2f;
        color = {0.0f, 1.0f, 1.0f - t};
    } else if (value < 0.8f) {
        // Green to Yellow
        float t = (value - 0.6f) / 0.2f;
        color = {t, 1.0f, 0.0f};
    } else {
        // Yellow to Red
        float t = (value - 0.8f) / 0.2f;
        color = {1.0f, 1.0f - t, 0.0f};
    }
    
    return color;
}

// =============================================================================
// Audio Host Integration Helper
// =============================================================================

// This function should be called from the audio host to get the plugin
extern "C" {
    __attribute__((visibility("default"))) VstInt32 VSTPluginMain(audioMasterCallback audioMaster) {
        static bool initialized = false;
        if (!initialized) {
            initialized = true;
            // Initialize GLFW if needed (in host application context)
        }
        
        return VSTPluginMainVST(audioMaster);
    }
}

// VST 2.4 plugin main entry
VstInt32 VSTPluginMainVST(audioMasterCallback audioMaster) {
    if (!audioMaster(0, audioMasterVersion, 0, 0, 0, 0)) {
        return 0; // Old version
    }
    
    Spectrum3DTerrainVST* plugin = new Spectrum3DTerrainVST(audioMaster);
    return (VstInt32)(void*)plugin;
}

// Main entry for testing (standalone executable)
int main() {
    // This would be used for standalone testing
    // In real VST host, this wouldn't be used
    return 0;
}