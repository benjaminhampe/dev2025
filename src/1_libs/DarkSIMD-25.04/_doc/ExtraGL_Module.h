#include "plugin.hpp"
#include "ExtraGLWindow.hpp"

struct GLWindowModule : Module {
    enum ParamIds {
        SHOW_BUTTON_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // V2:
    enum ParamIds {
        TOGGLE_WINDOW_PARAM,
        NUM_PARAMS
    };


    ExtraGLWindow* m_glWindow = nullptr;

    GLWindowModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configButton(SHOW_BUTTON_PARAM, "Show OpenGL Window");
    }

    ~GLWindowModule() {
        if (m_glWindow) {
            m_glWindow->stop();
            delete m_glWindow;
            m_glWindow = nullptr;
        }
    }

    void process(const ProcessArgs& args) override {
        if (params[SHOW_BUTTON_PARAM].getValue() > 0.5f) {
            if (!m_glWindow) {
                m_glWindow = new ExtraGLWindow(800, 600, "VCV Extra OpenGL");
                m_glWindow->start();
            } else {
                m_glWindow->show();
                m_glWindow->bringToFront();
            }
            params[SHOW_BUTTON_PARAM].setValue(0.0f); // reset button
        }
    }
    
    // V2:
    void process(const ProcessArgs& args) override 
    {
        bool togglePressed = params[TOGGLE_WINDOW_PARAM].getValue() > 0.5f;
        if (togglePressed) 
        {
            if (!m_glWindow) {
                m_glWindow = new ExtraGLWindow(800, 600, "VCV Extra OpenGL");
                m_glWindow->start();
            } else {
                // Toggle visibility
                if (m_glWindow->isVisible()) {
                    m_glWindow->hide();
                } else {
                    m_glWindow->show();
                    m_glWindow->bringToFront();
                }
            }

            // Reset button state
            params[TOGGLE_WINDOW_PARAM].setValue(0.0f);
        }
    }

};


struct GLWindowWidget : ModuleWidget 
{
    GLWindowWidget(GLWindowModule* module) 
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/GLWindowPanel.svg")));

        addChild(createWidget<Widget>(Vec(0, 0)));

        addParam(createParamCentered<CKD6>(mm2px(Vec(15, 30)), module, GLWindowModule::SHOW_BUTTON_PARAM));
    }
};

// V3:
struct GLWindowWidget : ModuleWidget 
{
    GLWindowWidget(GLWindowModule* module) 
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/GLWindowPanel.svg")));

        addParam(createParamCentered<CKD6>(mm2px(Vec(15, 30)), module, GLWindowModule::TOGGLE_WINDOW_PARAM));
    }
};


Model* modelGLWindowModule = createModel<GLWindowModule, GLWindowWidget>("GLWindowModule");
