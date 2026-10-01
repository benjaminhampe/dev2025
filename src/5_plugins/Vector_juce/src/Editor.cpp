#include "Editor.h"

namespace {

juce::Point<float>
clampf(juce::Point<float> point)
{
    point.x = de::clampf(point.x, 0.0f, 1.0);
    point.y = de::clampf(point.y, 0.0f, 1.0);

    // point.x =
    //     juce::jlimit(
    //         displayRect.getX(),
    //         displayRect.getRight(),
    //         point.x);

    // point.y =
    //     juce::jlimit(
    //         displayRect.getY(),
    //         displayRect.getBottom(),
    //         point.y);

    return point;
}

} // end namespace.

//====================================================================
Display::Display(VectorSynthesiser& synth)
//====================================================================
    : m_synth(synth)
{
    startTimerHz(60);
}

Display::~Display()
{
}

void Display::resized()
{
    m_displayRect = getLocalBounds().toFloat().reduced(10.0f);
}

void Display::paint(juce::Graphics& g)
{
    drawBackground(g);

    drawEnvelope(g);

    drawHandles(g);

    drawVoices(g);
}

void Display::drawBackground(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::darkgrey);
    g.drawRect(m_displayRect);

    // g.drawLine(
    //     displayRect.getCentreX(),
    //     displayRect.getY(),
    //     displayRect.getCentreX(),
    //     displayRect.getBottom());

    // g.drawLine(
    //     displayRect.getX(),
    //     displayRect.getCentreY(),
    //     displayRect.getRight(),
    //     displayRect.getCentreY());
}

void Display::drawEnvelope(juce::Graphics& g)
{
    g.setColour(juce::Colours::orange);

    int w = getWidth();
    int h = getHeight();

    g.drawLine(
        m_synth.m_attackPoint.x * w,
        m_synth.m_attackPoint.y * h,
        m_synth.m_decayPoint.x * w,
        m_synth.m_decayPoint.y * h,
        2.0f);

    g.drawLine(
        m_synth.m_decayPoint.x * w,
        m_synth.m_decayPoint.y * h,
        m_synth.m_sustainPoint.x * w,
        m_synth.m_sustainPoint.y * h,
        2.0f);

    g.drawLine(
        m_synth.m_sustainPoint.x * w,
        m_synth.m_sustainPoint.y * h,
        m_synth.m_releasePoint.x * w,
        m_synth.m_releasePoint.y * h,
        2.0f);
}

void Display::drawHandles(juce::Graphics& g)
{
    g.setColour(juce::Colours::yellow);

    auto drawNode = [&](juce::Point<float> p)
        {
            const int w = getWidth();
            const int h = getHeight();

            g.fillEllipse(
                (p.x * w) - 6.0f,
                (p.y * h) - 6.0f,
                12.0f,
                12.0f);
        };

    drawNode(m_synth.m_attackPoint);
    drawNode(m_synth.m_decayPoint);
    drawNode(m_synth.m_sustainPoint);
    drawNode(m_synth.m_releasePoint);
}

/*
juce::Point<float>
Display::getCursorPosition(const NoteVisualState& voice) const
{
    switch (voice.stage)
    {
        case de::vec::Env::Attack:
        {
            return attackPoint + (decayPoint - attackPoint)
                * voice.stageProgress;
        }

        case de::vec::Env::Decay:
        {
            return decayPoint + (sustainPoint - decayPoint)
                * voice.stageProgress;
        }

        case de::vec::Env::Sustain:
        {
            return sustainPoint;
        }

        case de::vec::Env::Release:
        {
            return sustainPoint + (releasePoint - sustainPoint)
                * voice.stageProgress;
        }

        case de::vec::Env::Idle:
        default:
        {
            return releasePoint;
        }
    }
}
*/

void Display::drawVoices(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    auto s1 = dbStr("Display(",w,",",h,")");
    g.setColour(juce::Colours::cyan);
    g.drawText(String(s1), 10, 10, w, 30, Justification::topLeft, false);

    bool bPrintedVoiceText = false;

    const auto& voices = m_synth.getVoices();
    for (size_t i = 0; i < voices.size(); ++i)
    {
        const auto& voice = *voices[i];

        if (!voice.isVoiceActive())
            continue;

        // ==== DrawCursor ====
        auto cursor = voice.getCursorPosition();
        float cx = cursor.x * w;
        float cy = cursor.y * h;
        g.setColour(juce::Colours::cyan);
        g.fillEllipse(cx - 7.0f, cy - 7.0f, 14.0f, 14.0f);

        // ==== DrawOrbiter ====
        auto orbiter = voice.getOrbiterPosition();
        float ox = orbiter.x * w;
        float oy = orbiter.y * h;
        g.setColour(juce::Colours::white);
        g.fillEllipse(ox - 10.0f, oy - 10.0f, 20.0f, 20.0f);

        // ==== DrawVoiceText ====
        if (!bPrintedVoiceText)
        {
            auto s2 = dbStr("Cursor(",cx,",",cy,")");
            auto s3 = dbStr("Orbiter(",ox,",",oy,")");
            g.drawText(String(s2), 10, 100, w, 30, Justification::topLeft, false);
            g.drawText(String(s3), 10, 130, w, 30, Justification::topLeft, false);
            bPrintedVoiceText = true;
        }
    }
}

Display::DragPoint
Display::hitTestHandle(juce::Point<float> position)
{
    constexpr float radius = 12.0f;

    Point<float> scr( getWidth(), getHeight() );

    if (position.getDistanceFrom(m_synth.m_attackPoint * scr) < radius)
        return DragPoint::Attack;

    if (position.getDistanceFrom(m_synth.m_decayPoint * scr) < radius)
        return DragPoint::Decay;

    if (position.getDistanceFrom(m_synth.m_sustainPoint * scr) < radius)
        return DragPoint::Sustain;

    if (position.getDistanceFrom(m_synth.m_releasePoint * scr) < radius)
        return DragPoint::Release;

    return DragPoint::None;
}

void Display::mouseDown(const juce::MouseEvent& e)
{
    m_activeHandle = hitTestHandle(e.position);
}

void Display::mouseDrag(const juce::MouseEvent& e)
{
    Point<float> ndc( e.position.x / float(getWidth()),
                      e.position.y / float(getHeight()) );

    auto p = clampf(ndc);

    switch (m_activeHandle)
    {
        case DragPoint::Attack: m_synth.m_attackPoint = p; break;
        case DragPoint::Decay: m_synth.m_decayPoint = p; break;
        case DragPoint::Sustain: m_synth.m_sustainPoint = p; break;
        case DragPoint::Release: m_synth.m_releasePoint = p; break;
        default: break;
    }

    repaint();
}

void Display::mouseUp(const juce::MouseEvent&)
{
    m_activeHandle = DragPoint::None;
}

void Display::timerCallback()
{
    repaint();
}

/*
void Display::setVisualStates(const std::array<NoteVisualState,32>& states)
{
    voiceStates = states;
}

void Display::setVisualStates(std::vector<NoteVisualState*> states)
{
    voiceStates = states;
}
*/

//====================================================================
VectorPluginEditor::VectorPluginEditor(VectorPluginProcessor& p)
//====================================================================
    : AudioProcessorEditor(&p)
    , m_processor(p)
    , m_display(p.getSynth())
{
    auto& apvts = p.getAPVTS();

    addAndMakeVisible(m_display);

    // --- OrbRadius ---

    m_orbRadiusSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    m_orbRadiusSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    m_orbRadiusSlider.setColour(Slider::rotarySliderFillColourId, juce::Colour(255,0,0));
    addAndMakeVisible(m_orbRadiusSlider);
    m_orbRadiusAttach.reset (new AudioProcessorValueTreeState::SliderAttachment(apvts, PID::orbitRadius, m_orbRadiusSlider));
    // m_orbRadiusSlider.setRange(.0, 1.);
    // m_orbRadiusSlider.setValue(.1);

    // --- OrbSpeed ---

    m_orbSpeedSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    m_orbSpeedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    m_orbSpeedSlider.setColour(Slider::rotarySliderFillColourId, juce::Colour(255,0,0));
    addAndMakeVisible(m_orbSpeedSlider);
    m_orbSpeedAttach.reset (new AudioProcessorValueTreeState::SliderAttachment(apvts, PID::orbitSpeed, m_orbSpeedSlider));
    // m_orbSpeedSlider.setRange(.01, 60.);
    // m_orbSpeedSlider.setValue(1.);

    // --- OrbSpeed127 ---

    m_orbSpeedSlider127.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    m_orbSpeedSlider127.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    m_orbSpeedSlider127.setColour(Slider::rotarySliderFillColourId, juce::Colour(255,0,0));
    addAndMakeVisible(m_orbSpeedSlider127);
    m_orbSpeedAttach127.reset (new AudioProcessorValueTreeState::SliderAttachment(apvts, PID::orbitSpeed127, m_orbSpeedSlider127));
    // m_orbSpeedSlider127.setRange(.0, 127.);
    // m_orbSpeedSlider127.setValue(.0);

    // --- OrbSpeed1k ---

    m_orbSpeedSlider1k.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    m_orbSpeedSlider1k.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    m_orbSpeedSlider1k.setColour(Slider::rotarySliderFillColourId, juce::Colour(255,0,0));
    addAndMakeVisible(m_orbSpeedSlider1k);
    m_orbSpeedAttach1k.reset (new AudioProcessorValueTreeState::SliderAttachment(apvts, PID::orbitSpeed1k, m_orbSpeedSlider1k));
    // m_orbSpeedSlider1k.setRange(.0, 1016.);
    // m_orbSpeedSlider1k.setValue(.0);

    // --- OrbPhaseStart ---

    m_orbPhaseSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    m_orbPhaseSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    m_orbPhaseSlider.setColour(Slider::rotarySliderFillColourId, juce::Colour(255,0,0));
    addAndMakeVisible(m_orbPhaseSlider);
    m_orbPhaseAttach.reset (new AudioProcessorValueTreeState::SliderAttachment(apvts, PID::orbitPhase, m_orbPhaseSlider));
    // m_orbPhaseSlider.setRange(.0, 2.0 * M_PI);
    // m_orbPhaseSlider.setValue(.0);

    // --- OrbPhaseDirMode ---

    m_orbDirModeCombo.setColour (ComboBox::backgroundColourId, Colours::black);
    m_orbDirModeCombo.setColour (ComboBox::textColourId, Colours::white);
    //m_orbDirModeCombo.addItemList (StringArray { "LP12", "LP24", "BP12", "HP12", "Notch12" }, 1);
    addAndMakeVisible(m_orbDirModeCombo);
    m_orbDirModeAttach.reset(new AudioProcessorValueTreeState::ComboBoxAttachment (apvts, PID::orbitDirMode, m_orbDirModeCombo));


    setSize(900,600);
    startTimerHz(30);
}

void VectorPluginEditor::resized()
{
    // int x = getLocalBounds().getX();
    // int y = getLocalBounds().getY();
    int w = getLocalBounds().getWidth();
    int h = getLocalBounds().getHeight();
    // DE_DEBUG("bounds(",x,",",y,",",w,",",h,")")

    int w10 = w / 10;
    int m = w - 3*w10;

    m_display.setBounds(w10,0,m,h);

    int mt = 10;        // Margin top
    int h4 = h / 4;
    int ho = h4 - mt;

    int x = 8 * w10;
    int y = mt;
    m_orbRadiusSlider.setBounds(x,y,w10,ho); y += h4;
    m_orbSpeedSlider.setBounds(x,y,w10,ho); y += h4;
    m_orbSpeedSlider127.setBounds(x,y,w10,ho); y += h4;
    m_orbSpeedSlider1k.setBounds(x,y,w10,ho); y += h4;

    x += w10;
    y = mt;
    m_orbPhaseSlider.setBounds(x,y,w10,ho); y += h4;
    m_orbDirModeCombo.setBounds(x,y,w10,ho); y += h4;
}

void VectorPluginEditor::timerCallback()
{

}

void VectorPluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
}

