#ifndef SPECTRUM3D_TERRAIN_VST_H
#define SPECTRUM3D_TERRAIN_VST_H

// VST 2.4 Plugin: 3D Spectrum Terrain Visualization
// Dependencies: PFFFT, GLFW, GLEW, GLM
// Author: MiniMax Agent
// Date: 2025-11-12

#include <vector>
#include <array>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <memory>

// Forward declarations for VST2.4
struct VstEvent;
struct VstEvents;
struct VstMidiEvent;
struct VstTimeInfo;

// =============================================================================
// VST 2.4 Plugin Definition
// =============================================================================

class Spectrum3DTerrainVST : public AudioEffect {
public:
    Spectrum3DTerrainVST(audioMasterCallback audioMaster);
    ~Spectrum3DTerrainVST();

    // VST Required Methods
    void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override;
    VstInt32 processEvents(VstEvents* events) override;
    
    void setParameter(VstInt32 index, float value) override;
    float getParameter(VstInt32 index) override;
    void getParameterName(VstInt32 index, char* label) override;
    void getParameterDisplay(VstInt32 index, char* text) override;
    void getParameterLabel(VstInt32 index, char* label) override;
    
    VstInt32 getVendorVersion() override { return 1000; }
    void* getVendorEffect() override { return (void*)this; }
    bool getEffectName(char* name) override;
    bool getProductString(char* text) override;
    bool getVendorString(char* text) override;
    
    VstPlugCategory getPlugCategory() override { return kPlugCategVisualization; }
    
    // OpenGL and Rendering
    bool initOpenGL();
    void renderOpenGL();
    void cleanupOpenGL();
    
    // UI and Input
    void initUI();
    void cleanupUI();
    void handleUIInput();
    bool initMIDI();

private:
    // Audio processing data
    static constexpr int MAX_CHANNELS = 8;
    static constexpr int MAX_FFT_SIZE = 8192;
    static constexpr int MAX_HISTORY = 512;
    
    // OpenGL and FFT state
    int currentFFTSize;
    int historyLength;
    int selectedChannel;
    bool combinedMode;
    bool showUI;
    
    // Audio buffers and FFT data
    std::vector<std::vector<float>> channelBuffers;
    std::vector<float> windowBuffer;
    std::vector<float> fftMagnitudeBuffer;
    std::vector<float> fftDBBuffer;
    std::vector<std::vector<float>> historyData;
    
    // OpenGL resources
    unsigned int vao, vbo, ebo, texture;
    unsigned int shaderProgram;
    unsigned int vertexShader, fragmentShader;
    
    // UI state
    bool mouseButtonPressed[3];
    double mouseX, mouseY;
    float cameraAngleX, cameraAngleY, cameraDistance;
    
    // VST parameters
    enum ParameterIndices {
        PARAM_FFT_SIZE = 0,
        PARAM_HISTORY_LENGTH,
        PARAM_CHANNEL_SELECT,
        PARAM_COMBINED_MODE,
        PARAM_FREQ_RANGE_START,
        PARAM_FREQ_RANGE_END,
        PARAM_COLOR_SCALE,
        PARAM_SMOOTHING,
        PARAM_WINDOW_TYPE,
        PARAM_SHOW_UI,
        NUM_PARAMETERS
    };
    
    float parameters[NUM_PARAMETERS];
    char parameterValues[NUM_PARAMETERS][64];
    
    // Internal methods
    void updateParameters();
    void processAudioBlock(float** inputs, VstInt32 sampleFrames);
    void computeFFT(const std::vector<float>& audioData);
    void updateHistory();
    void convertToDecibels();
    void applyWindowFunction(float* data, int size);
    
    // OpenGL setup methods
    bool createShaders();
    bool createBuffers();
    void updateVertexData();
    
    // Utility methods
    void clamp(float& value, float min, float max);
    float lerp(float a, float b, float t);
    float smoothStep(float edge0, float edge1, float x);
    
    // Window functions
    void applyHannWindow(float* data, int size);
    void applyHammingWindow(float* data, int size);
    void applyBlackmanWindow(float* data, int size);
    
    // Color gradient
    std::array<float, 3> getColorFromGradient(float value);
};

// =============================================================================
// VST Plugin Factory
// =============================================================================

static AEffect* vstPlugin(audioMasterCallback audioMaster) {
    Spectrum3DTerrainVST* plugin = new Spectrum3DTerrainVST(audioMaster);
    return plugin->getAEffect();
}

// =============================================================================
// VST Main Entry Point
// =============================================================================

extern "C" {
    __attribute__((visibility("default"))) VstInt32 main_mainsym(VstInt32 vendorSpecific,
                                                                VstInt32* ptr,
                                                                VstCallback callback) {
        return 0; // Legacy VST
    }
}

// =============================================================================
// AudioEffectX Constructor Implementation
// =============================================================================

Spectrum3DTerrainVST::Spectrum3DTerrainVST(audioMasterCallback audioMaster) 
    : AudioEffectX(audioMaster, 1, NUM_PARAMETERS) {
    
    // Initialize audio data
    currentFFTSize = 1024;
    historyLength = 256;
    selectedChannel = 0;
    combinedMode = true;
    showUI = true;
    
    // Initialize audio buffers
    channelBuffers.resize(MAX_CHANNELS);
    for (auto& buffer : channelBuffers) {
        buffer.resize(currentFFTSize);
    }
    
    windowBuffer.resize(MAX_FFT_SIZE);
    fftMagnitudeBuffer.resize(MAX_FFT_SIZE);
    fftDBBuffer.resize(MAX_FFT_SIZE);
    historyData.resize(MAX_HISTORY);
    for (auto& hist : historyData) {
        hist.resize(MAX_FFT_SIZE);
    }
    
    // Initialize parameters
    for (int i = 0; i < NUM_PARAMETERS; i++) {
        parameters[i] = 0.0f;
    }
    parameters[PARAM_FFT_SIZE] = 1024.0f / MAX_FFT_SIZE;
    parameters[PARAM_HISTORY_LENGTH] = 256.0f / MAX_HISTORY;
    parameters[PARAM_CHANNEL_SELECT] = 0.0f;
    parameters[PARAM_FREQ_RANGE_START] = 20.0f / 22050.0f; // 20Hz normalized
    parameters[PARAM_FREQ_RANGE_END] = 20000.0f / 22050.0f; // 20kHz normalized
    parameters[PARAM_COLOR_SCALE] = 1.0f;
    parameters[PARAM_SMOOTHING] = 0.8f;
    parameters[PARAM_WINDOW_TYPE] = 0.0f; // Hann window
    parameters[PARAM_SHOW_UI] = 1.0f;
    
    // Set program information
    setNumPrograms(1);
    setProgramName("Default");
    
    // Initialize OpenGL
    if (!initOpenGL()) {
        // Handle OpenGL initialization failure
    }
    
    // Initialize UI
    initUI();
    
    // Initialize MIDI
    initMIDI();
}

Spectrum3DTerrainVST::~Spectrum3DTerrainVST() {
    cleanupOpenGL();
    cleanupUI();
}

// =============================================================================
// VST Required Methods Implementation
// =============================================================================

void Spectrum3DTerrainVST::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) {
    if (!inputs || !outputs) return;
    
    // Process audio block
    processAudioBlock(inputs, sampleFrames);
    
    // Copy input to output (pass-through)
    int channels = std::min(getNumInputs(), getNumOutputs());
    for (int ch = 0; ch < channels; ch++) {
        if (inputs[ch] && outputs[ch]) {
            std::memcpy(outputs[ch], inputs[ch], sampleFrames * sizeof(float));
        }
    }
    
    // Render OpenGL if UI is visible
    if (showUI && sampleFrames > 0) {
        renderOpenGL();
    }
}

VstInt32 Spectrum3DTerrainVST::processEvents(VstEvents* events) {
    if (!events) return 0;
    
    for (VstInt32 i = 0; i < events->numEvents; i++) {
        VstEvent* event = events->events[i];
        
        if (event->type == kVstMidiType) {
            VstMidiEvent* midiEvent = (VstMidiEvent*)event;
            
            // Simple MIDI handling - CC messages for parameters
            unsigned char status = midiEvent->midiData[0] & 0xF0;
            unsigned char channel = midiEvent->midiData[0] & 0x0F;
            
            if (status == 0xB0) { // Control Change
                unsigned char controller = midiEvent->midiData[1];
                unsigned char value = midiEvent->midiData[2];
                
                float normalizedValue = value / 127.0f;
                
                switch (controller) {
                    case 1: setParameter(PARAM_FREQ_RANGE_START, normalizedValue); break;
                    case 2: setParameter(PARAM_FREQ_RANGE_END, normalizedValue); break;
                    case 3: setParameter(PARAM_COLOR_SCALE, normalizedValue); break;
                    case 4: setParameter(PARAM_SMOOTHING, normalizedValue); break;
                    case 5: setParameter(PARAM_WINDOW_TYPE, normalizedValue * 3.0f); break;
                }
            }
        }
    }
    
    return 1;
}

void Spectrum3DTerrainVST::setParameter(VstInt32 index, float value) {
    if (index < 0 || index >= NUM_PARAMETERS) return;
    
    parameters[index] = value;
    updateParameters();
    
    // Update parameter display
    switch (index) {
        case PARAM_FFT_SIZE:
            sprintf(parameterValues[index], "%d", currentFFTSize);
            break;
        case PARAM_HISTORY_LENGTH:
            sprintf(parameterValues[index], "%d", historyLength);
            break;
        case PARAM_CHANNEL_SELECT:
            sprintf(parameterValues[index], "%d", selectedChannel);
            break;
        case PARAM_COMBINED_MODE:
            sprintf(parameterValues[index], "%s", combinedMode ? "Combined" : "Single");
            break;
        case PARAM_FREQ_RANGE_START:
            sprintf(parameterValues[index], "%.1f Hz", 20.0f + value * 22030.0f);
            break;
        case PARAM_FREQ_RANGE_END:
            sprintf(parameterValues[index], "%.1f Hz", 20.0f + value * 22030.0f);
            break;
        case PARAM_COLOR_SCALE:
            sprintf(parameterValues[index], "%.2f", value * 10.0f);
            break;
        case PARAM_SMOOTHING:
            sprintf(parameterValues[index], "%.2f", value);
            break;
        case PARAM_WINDOW_TYPE:
            {
                int windowType = (int)(value * 3.0f);
                const char* windows[] = {"Hann", "Hamming", "Blackman", "None"};
                sprintf(parameterValues[index], "%s", windows[windowType]);
            }
            break;
        case PARAM_SHOW_UI:
            sprintf(parameterValues[index], "%s", value > 0.5f ? "ON" : "OFF");
            break;
    }
}

float Spectrum3DTerrainVST::getParameter(VstInt32 index) {
    if (index < 0 || index >= NUM_PARAMETERS) return 0.0f;
    return parameters[index];
}

void Spectrum3DTerrainVST::getParameterName(VstInt32 index, char* label) {
    switch (index) {
        case PARAM_FFT_SIZE: strcpy(label, "FFT Size"); break;
        case PARAM_HISTORY_LENGTH: strcpy(label, "History"); break;
        case PARAM_CHANNEL_SELECT: strcpy(label, "Channel"); break;
        case PARAM_COMBINED_MODE: strcpy(label, "Mode"); break;
        case PARAM_FREQ_RANGE_START: strcpy(label, "Freq Start"); break;
        case PARAM_FREQ_RANGE_END: strcpy(label, "Freq End"); break;
        case PARAM_COLOR_SCALE: strcpy(label, "Color Scale"); break;
        case PARAM_SMOOTHING: strcpy(label, "Smoothing"); break;
        case PARAM_WINDOW_TYPE: strcpy(label, "Window"); break;
        case PARAM_SHOW_UI: strcpy(label, "Show UI"); break;
        default: strcpy(label, "Unknown");
    }
}

void Spectrum3DTerrainVST::getParameterDisplay(VstInt32 index, char* text) {
    strcpy(text, parameterValues[index]);
}

void Spectrum3DTerrainVST::getParameterLabel(VstInt32 index, char* label) {
    switch (index) {
        case PARAM_FFT_SIZE: strcpy(label, "samples"); break;
        case PARAM_HISTORY_LENGTH: strcpy(label, "frames"); break;
        case PARAM_CHANNEL_SELECT: strcpy(label, "ch"); break;
        case PARAM_COMBINED_MODE: strcpy(label, ""); break;
        case PARAM_FREQ_RANGE_START: strcpy(label, "Hz"); break;
        case PARAM_FREQ_RANGE_END: strcpy(label, "Hz"); break;
        case PARAM_COLOR_SCALE: strcpy(label, "dB"); break;
        case PARAM_SMOOTHING: strcpy(label, ""); break;
        case PARAM_WINDOW_TYPE: strcpy(label, ""); break;
        case PARAM_SHOW_UI: strcpy(label, ""); break;
        default: strcpy(label, "");
    }
}

bool Spectrum3DTerrainVST::getEffectName(char* name) {
    strcpy(name, "Spectrum 3D Terrain");
    return true;
}

bool Spectrum3DTerrainVST::getProductString(char* text) {
    strcpy(text, "Spectrum 3D Terrain VST Plugin");
    return true;
}

bool Spectrum3DTerrainVST::getVendorString(char* text) {
    strcpy(text, "MiniMax Agent");
    return true;
}

// =============================================================================
// Audio Processing Implementation
// =============================================================================

void Spectrum3DTerrainVST::processAudioBlock(float** inputs, VstInt32 sampleFrames) {
    if (!inputs || sampleFrames <= 0) return;
    
    // Collect audio data for selected channels
    int numChannels = std::min(getNumInputs(), MAX_CHANNELS);
    
    // Apply audio data to buffers for FFT processing
    for (int ch = 0; ch < numChannels && ch < currentFFTSize; ch++) {
        if (inputs[ch]) {
            // Copy audio data to buffer (taking the most recent samples)
            int copySize = std::min(sampleFrames, currentFFTSize);
            int offset = sampleFrames - copySize;
            
            for (int i = 0; i < copySize && i < currentFFTSize; i++) {
                channelBuffers[ch][i] = inputs[ch][offset + i];
            }
        }
    }
    
    // Compute FFT for selected channel(s)
    if (combinedMode && numChannels >= 2) {
        // Combine channels (L+R)/2
        std::vector<float> combinedData(currentFFTSize, 0.0f);
        for (int i = 0; i < currentFFTSize; i++) {
            if (numChannels >= 2) {
                combinedData[i] = (channelBuffers[0][i] + channelBuffers[1][i]) * 0.5f;
            } else {
                combinedData[i] = channelBuffers[0][i];
            }
        }
        computeFFT(combinedData);
    } else {
        // Use selected single channel
        int channelIndex = std::min(selectedChannel, numChannels - 1);
        computeFFT(channelBuffers[channelIndex]);
    }
    
    updateHistory();
}

void Spectrum3DTerrainVST::computeFFT(const std::vector<float>& audioData) {
    if (audioData.size() < currentFFTSize) return;
    
    // Copy and apply window function
    std::vector<float> windowedData(currentFFTSize);
    std::copy(audioData.begin(), audioData.begin() + currentFFTSize, windowedData.begin());
    
    applyWindowFunction(windowedData.data(), currentFFTSize);
    
    // Placeholder for PFFFT computation
    // In actual implementation, you would call:
    // pffft_transform_ordered(windowedData.data(), fftMagnitudeBuffer.data(), NULL, fftPlan);
    
    // For now, simulate FFT magnitude computation
    for (int i = 0; i < currentFFTSize; i++) {
        // Simple magnitude approximation for demonstration
        fftMagnitudeBuffer[i] = std::abs(windowedData[i]);
    }
    
    convertToDecibels();
    
    // Apply smoothing if enabled
    float smoothing = parameters[PARAM_SMOOTHING];
    if (smoothing > 0.0f) {
        float alpha = smoothing * 0.1f;
        for (int i = 0; i < currentFFTSize; i++) {
            static std::vector<float> previousDB;
            if (previousDB.size() != currentFFTSize) {
                previousDB.resize(currentFFTSize);
                std::fill(previousDB.begin(), previousDB.end(), -120.0f);
            }
            
            fftDBBuffer[i] = (1.0f - alpha) * previousDB[i] + alpha * fftDBBuffer[i];
            previousDB[i] = fftDBBuffer[i];
        }
    }
}

void Spectrum3DTerrainVST::updateHistory() {
    // Shift history and add new FFT data
    for (int i = historyLength - 1; i > 0; i--) {
        std::copy(historyData[i-1].begin(), historyData[i-1].end(), historyData[i].begin());
    }
    
    // Add current FFT data to history
    std::copy(fftDBBuffer.begin(), fftDBBuffer.end(), historyData[0].begin());
}

void Spectrum3DTerrainVST::convertToDecibels() {
    for (int i = 0; i < currentFFTSize; i++) {
        float magnitude = fftMagnitudeBuffer[i];
        if (magnitude < 1e-10f) {
            fftDBBuffer[i] = -120.0f; // Minimum dB value
        } else {
            fftDBBuffer[i] = 20.0f * std::log10(magnitude);
        }
    }
}

void Spectrum3DTerrainVST::applyWindowFunction(float* data, int size) {
    int windowType = (int)(parameters[PARAM_WINDOW_TYPE] * 3.0f);
    
    switch (windowType) {
        case 0: applyHannWindow(data, size); break;
        case 1: applyHammingWindow(data, size); break;
        case 2: applyBlackmanWindow(data, size); break;
        case 3: /* No window */ break;
    }
}

void Spectrum3DTerrainVST::applyHannWindow(float* data, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (size - 1)));
        data[i] *= window;
    }
}

void Spectrum3DTerrainVST::applyHammingWindow(float* data, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (size - 1));
        data[i] *= window;
    }
}

void Spectrum3DTerrainVST::applyBlackmanWindow(float* data, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.42f - 0.5f * std::cos(2.0f * M_PI * i / (size - 1)) 
                          + 0.08f * std::cos(4.0f * M_PI * i / (size - 1));
        data[i] *= window;
    }
}

// =============================================================================
// OpenGL Implementation
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

bool Spectrum3DTerrainVST::createShaders() {
    // Vertex Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 vertexColor;
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            vertexColor = aColor;
        }
    )";
    
    // Fragment Shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";
    
    // Compile vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        return false;
    }
    
    // Compile fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        return false;
    }
    
    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        return false;
    }
    
    return true;
}

bool Spectrum3DTerrainVST::createBuffers() {
    // Create VAO, VBO, EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Allocate initial buffer size
    glBufferData(GL_ARRAY_BUFFER, MAX_FFT_SIZE * MAX_HISTORY * 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    
    glBindVertexArray(0);
    
    return true;
}

void Spectrum3DTerrainVST::updateVertexData() {
    // Generate terrain vertices from FFT data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    int fftSize = currentFFTSize;
    int histSize = historyLength;
    
    float freqRangeStart = parameters[PARAM_FREQ_RANGE_START];
    float freqRangeEnd = parameters[PARAM_FREQ_RANGE_END];
    float colorScale = parameters[PARAM_COLOR_SCALE] * 10.0f;
    
    // Create vertices
    for (int z = 0; z < histSize; z++) {
        for (int x = 0; x < fftSize; x++) {
            float freq = (float)x / fftSize;
            
            // Apply frequency range filtering
            if (freq < freqRangeStart || freq > freqRangeEnd) continue;
            
            float dbValue = historyData[z][x];
            float normalizedDB = std::max(0.0f, std::min(1.0f, (dbValue + 120.0f) / 120.0f));
            
            // Position (x, y, z)
            vertices.push_back((float)x / fftSize - 0.5f);  // X
            vertices.push_back(normalizedDB * colorScale - 1.0f);  // Y
            vertices.push_back((float)z / histSize - 0.5f);  // Z
            
            // Color
            std::array<float, 3> color = getColorFromGradient(normalizedDB);
            vertices.push_back(color[0]);
            vertices.push_back(color[1]);
            vertices.push_back(color[2]);
            
            // Create indices for triangles
            if (x < fftSize - 1 && z < histSize - 1) {
                int current = z * fftSize + x;
                int next = z * fftSize + (x + 1);
                int below = (z + 1) * fftSize + x;
                int belowNext = (z + 1) * fftSize + (x + 1);
                
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(below);
                
                indices.push_back(next);
                indices.push_back(belowNext);
                indices.push_back(below);
            }
        }
    }
    
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

std::array<float, 3> Spectrum3DTerrainVST::getColorFromGradient(float value) {
    // Color gradient: Blue -> Cyan -> Green -> Yellow -> Red
    std::array<float, 3> color;
    
    if (value < 0.25f) {
        // Blue to Cyan
        float t = value / 0.25f;
        color = {0.0f, t, 1.0f};
    } else if (value < 0.5f) {
        // Cyan to Green
        float t = (value - 0.25f) / 0.25f;
        color = {0.0f, 1.0f, 1.0f - t};
    } else if (value < 0.75f) {
        // Green to Yellow
        float t = (value - 0.5f) / 0.25f;
        color = {t, 1.0f, 0.0f};
    } else {
        // Yellow to Red
        float t = (value - 0.75f) / 0.25f;
        color = {1.0f, 1.0f - t, 0.0f};
    }
    
    return color;
}

void Spectrum3DTerrainVST::renderOpenGL() {
    if (!showUI) return;
    
    // Update vertex data
    updateVertexData();
    
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Set up matrices (placeholder for GLM implementation)
    // mat4 model, view, projection;
    // You would use GLM to create these matrices
    
    // Apply camera transformation
    float radius = cameraDistance;
    float camX = radius * std::cos(cameraAngleY) * std::sin(cameraAngleX);
    float camY = radius * std::sin(cameraAngleY);
    float camZ = radius * std::cos(cameraAngleY) * std::cos(cameraAngleX);
    
    // Draw terrain
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, /* indexCount */, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Handle UI input
    handleUIInput();
}

// =============================================================================
// UI Implementation
// =============================================================================

void Spectrum3DTerrainVST::initUI() {
    // Initialize GLFW for UI if not already done
    // This would typically be done in the host application context
    
    for (int i = 0; i < 3; i++) {
        mouseButtonPressed[i] = false;
    }
}

void Spectrum3DTerrainVST::cleanupUI() {
    // Cleanup UI resources
}

void Spectrum3DTerrainVST::handleUIInput() {
    // Handle mouse input for camera control
    if (mouseButtonPressed[0]) { // Left mouse button - rotate
        cameraAngleX += 0.01f; // This would be based on mouse movement
        cameraAngleY = std::max(-1.5f, std::min(1.5f, cameraAngleY + 0.01f));
    }
    
    if (mouseButtonPressed[2]) { // Right mouse button - zoom
        cameraDistance = std::max(1.0f, std::min(10.0f, cameraDistance + 0.1f));
    }
}

// =============================================================================
// MIDI Implementation
// =============================================================================

bool Spectrum3DTerrainVST::initMIDI() {
    // MIDI handling is done in processEvents()
    return true;
}

// =============================================================================
// Parameter Management
// =============================================================================

void Spectrum3DTerrainVST::updateParameters() {
    // Update FFT size
    int newFFTSize = 128 << ((int)(parameters[PARAM_FFT_SIZE] * 6)); // 128, 256, 512, 1024, 2048, 4096, 8192
    newFFTSize = std::max(128, std::min(MAX_FFT_SIZE, newFFTSize));
    
    if (newFFTSize != currentFFTSize) {
        currentFFTSize = newFFTSize;
        for (auto& buffer : channelBuffers) {
            buffer.resize(currentFFTSize);
        }
    }
    
    // Update history length
    int newHistory = 32 + (int)(parameters[PARAM_HISTORY_LENGTH] * (MAX_HISTORY - 32));
    newHistory = std::max(32, std::min(MAX_HISTORY, newHistory));
    
    if (newHistory != historyLength) {
        historyLength = newHistory;
        for (auto& hist : historyData) {
            hist.resize(currentFFTSize);
        }
    }
    
    // Update channel selection
    selectedChannel = (int)(parameters[PARAM_CHANNEL_SELECT] * (MAX_CHANNELS - 1));
    
    // Update combined mode
    combinedMode = parameters[PARAM_COMBINED_MODE] > 0.5f;
    
    // Update UI visibility
    showUI = parameters[PARAM_SHOW_UI] > 0.5f;
}

// =============================================================================
// Utility Methods
// =============================================================================

void Spectrum3DTerrainVST::clamp(float& value, float min, float max) {
    value = std::max(min, std::min(max, value));
}

float Spectrum3DTerrainVST::lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float Spectrum3DTerrainVST::smoothStep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

void Spectrum3DTerrainVST::cleanupOpenGL() {
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

#endif // SPECTRUM3D_TERRAIN_VST_H

// =============================================================================
// GLFW Event Handlers (to be implemented in main.cpp or host application)
// =============================================================================

/*
// Example GLFW event handlers to be used in the host application:

void mouseButtonCallback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseButtonPressed[0] = (action == GLFW_PRESS);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mouseButtonPressed[2] = (action == GLFW_PRESS);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        mouseButtonPressed[1] = (action == GLFW_PRESS);
    }
}

void cursorPosCallback(double x, double y) {
    mouseX = x;
    mouseY = y;
    
    // Implement camera rotation based on mouse movement
    if (mouseButtonPressed[0]) {
        float dx = x - lastMouseX;
        float dy = y - lastMouseY;
        cameraAngleX += dx * 0.01f;
        cameraAngleY = std::max(-1.5f, std::min(1.5f, cameraAngleY + dy * 0.01f));
    }
}

void scrollCallback(double xOffset, double yOffset) {
    cameraDistance = std::max(1.0f, std::min(10.0f, cameraDistance + yOffset * 0.1f));
}

void keyCallback(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        showUI = !showUI;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        // Reset camera
        cameraAngleX = 0.0f;
        cameraAngleY = 0.3f;
        cameraDistance = 3.0f;
    }
}
*/