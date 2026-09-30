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
    m_orbRadiusSlider.onValueChange = [this]
        {
            auto& synth = m_processor.getSynth();
            synth.m_orbRadius = m_orbRadiusSlider.getValue();
        };

    m_orbRadiusSlider.setRange(0.0, 1.0);
    m_orbRadiusSlider.setValue(.1);

    // --- OrbSpeed ---
    m_orbSpeedSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    m_orbSpeedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    m_orbSpeedSlider.setColour(Slider::rotarySliderFillColourId, juce::Colour(255,0,0));
    addAndMakeVisible(m_orbSpeedSlider);
    m_orbSpeedAttach.reset (new AudioProcessorValueTreeState::SliderAttachment(apvts, PID::orbitSpeed, m_orbSpeedSlider));
    m_orbSpeedSlider.onValueChange = [this]
        {
            auto& synth = m_processor.getSynth();
            synth.setOrbiterSpeed(m_orbSpeedSlider.getValue());
        };

    m_orbSpeedSlider.setRange(0.001, 1000.0);
    m_orbSpeedSlider.setValue(1.0);

    setSize(900,600);
    startTimerHz(30);
}

void VectorPluginEditor::resized()
{
    auto area = getLocalBounds();
    auto right = area.removeFromRight(180);
    m_display.setBounds(area.reduced(10));
    m_orbRadiusSlider.setBounds(right.removeFromTop(120).reduced(10));
    m_orbSpeedSlider.setBounds(right.removeFromTop(120).reduced(10));
}

void VectorPluginEditor::timerCallback()
{

}

void VectorPluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
}

