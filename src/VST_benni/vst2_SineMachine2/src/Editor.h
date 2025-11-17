#pragma once
#include "Globals.h"
#include "Preview.h"

class Synth;

class Editor
{
public:
    Editor(Synth* effect);
    ~Editor();
    void open();
    void close();
    void run();
    void requestClose();

private:
    void draw();
    void updateLayout();
    void doPartialDrawing();
    static void resizeEvent(GLFWwindow* window, int w, int h);
    static void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
    static void mouseMoveEvent(GLFWwindow* window, double x, double y);
    static void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

    Synth* m_synth;
    GLFWwindow* m_window;
    NVGcontext* m_vg;
    bool m_doPartialDawing;
    double m_mouseX;
    double m_mouseY;
    int m_fontNotoEmojiMedium;
    int m_fontShareTechMonoRegular;

    de::Recti m_rHeader;
    de::Recti m_rFooter;
    de::Recti m_rPreview;
    de::Recti m_rButtons;
    de::Recti m_rPartial;
    de::Recti m_rVolume;

    Preview m_preview;
};

