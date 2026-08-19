#include "plugin.hpp"
#include <gl/GL.h>
#include <vector>

struct SmoothedWaveform : rack::Module {
    enum InputIds {
        AUDIO_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum ParamIds {
        NUM_PARAMS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    std::vector<float> waveform;
    const int bufferSize = 512;

    SmoothedWaveform() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        waveform.resize(bufferSize, 0.f);
    }

    void process(const ProcessArgs& args) override {
        float in = inputs[AUDIO_INPUT].getVoltage();
        waveform.push_back(in);
        if (waveform.size() > bufferSize)
            waveform.erase(waveform.begin());
    }
};

struct GLWaveform : rack::Widget {
    SmoothedWaveform* module;
    GLuint shaderProgram = 0;

    GLWaveform(SmoothedWaveform* m) {
        module = m;
        box.pos = Vec(0, 0);
        box.size = Vec(200, 120);
        initShader();
    }

    void draw(const DrawArgs& args) override {
        if (!module || module->waveform.empty())
            return;

        nvgEndFrame(args.vg);
        glViewport(box.pos.x, args.windowSize.y - box.pos.y - box.size.y, box.size.x, box.size.y);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(shaderProgram);

        std::vector<float> vertices;
        for (size_t i = 0; i < module->waveform.size(); ++i) {
            float x = (float)i / module->waveform.size() * 2.f - 1.f;
            float y = module->waveform[i] / 5.f;
            vertices.push_back(x); vertices.push_back(y + 0.02f); vertices.push_back(0.0f); // v = 0.0
            vertices.push_back(x); vertices.push_back(y - 0.02f); vertices.push_back(1.0f); // v = 1.0
        }

        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); // v coord
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        glUseProgram(0);
        nvgBeginFrame(args.vg, args.windowSize[0], args.windowSize[1], 1.f);
    }

    void initShader() {
        const char* vertexSrc = R"GLSL(
            #version 330 core
            layout(location = 0) in vec2 position;
            layout(location = 1) in float vCoord;
            out float v;
            void main() {
                v = vCoord;
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )GLSL";

        const char* fragmentSrc = R"GLSL(
            #version 330 core
            in float v;
            out vec4 fragColor;
            void main() {
                float alpha = smoothstep(0.0, 0.1, v) * smoothstep(1.0, 0.9, v);
                fragColor = vec4(0.2, 0.8, 1.0, alpha);
            }
        )GLSL";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertexSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragmentSrc, nullptr);
        glCompileShader(fs);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fs);
        glLinkProgram(shaderProgram);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }
};

struct SmoothedWaveformWidget : rack::ModuleWidget {
    SmoothedWaveformWidget(SmoothedWaveform* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/SmoothedWaveform.svg")));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 110)), module, SmoothedWaveform::AUDIO_INPUT));
        addChild(new GLWaveform(module));
    }
};

Model* modelSmoothedWaveform = createModel<SmoothedWaveform, SmoothedWaveformWidget>("SmoothedWaveform");
