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
Display::Display()
//====================================================================
{
    attackPoint  = { 0.2f, 0.2f };
    decayPoint   = { 0.8f, 0.3f };
    sustainPoint = { 0.7f, 0.8f };
    releasePoint = { 0.2f, 0.9f };

    startTimerHz(60);
}

Display::~Display()
{
}

void Display::resized()
{
    displayRect = getLocalBounds().toFloat().reduced(10.0f);
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
    g.drawRect(displayRect);

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
        attackPoint.x * w,
        attackPoint.y * h,
        decayPoint.x * w,
        decayPoint.y * h,
        2.0f);

    g.drawLine(
        decayPoint.x * w,
        decayPoint.y * h,
        sustainPoint.x * w,
        sustainPoint.y * h,
        2.0f);

    g.drawLine(
        sustainPoint.x * w,
        sustainPoint.y * h,
        releasePoint.x * w,
        releasePoint.y * h,
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

    drawNode(attackPoint);
    drawNode(decayPoint);
    drawNode(sustainPoint);
    drawNode(releasePoint);
}

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

void Display::drawVoices(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    auto s1 = dbStr("Display(",w,",",h,")");
    g.setColour(juce::Colours::cyan);
    g.drawText(String(s1), 10, 10, w, 30, Justification::topLeft, false);

    bool bPrintedVoiceText = false;
    for (size_t i = 0; i < voiceStates.size(); ++i)
    {
        const auto& voice = *voiceStates[i];

        if (!voice.bPlaying)
            continue;

        // ==== DrawCursor ====
        auto cursor = clampf(getCursorPosition(voice));

        g.setColour(juce::Colours::cyan);

        float cx = cursor.x;
        float cy = cursor.y;

        g.fillEllipse(
            (cx * w) - 7.0f,
            (cy * h) - 7.0f,
            14.0f,
            14.0f);

        // ==== DrawOrbiter ====

        float ox = std::cos(voice.orbiterAngle) * orbitRadius;
        float oy = std::sin(voice.orbiterAngle) * orbitRadius;
        auto orbiter = juce::Point<float>(cx + ox, cy + ox);

        orbiter = clampf(orbiter);

        g.setColour(juce::Colours::white);
        g.fillEllipse((ox * w) - 10.0f, (oy * h) - 10.0f, 20.0f, 20.0f);

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

    if (position.getDistanceFrom(attackPoint * scr) < radius)
        return DragPoint::Attack;

    if (position.getDistanceFrom(decayPoint * scr) < radius)
        return DragPoint::Decay;

    if (position.getDistanceFrom(sustainPoint * scr) < radius)
        return DragPoint::Sustain;

    if (position.getDistanceFrom(releasePoint * scr) < radius)
        return DragPoint::Release;

    return DragPoint::None;
}

void Display::mouseDown(const juce::MouseEvent& e)
{
    activeHandle = hitTestHandle(e.position);
}

void Display::mouseDrag(const juce::MouseEvent& e)
{
    Point<float> ndc( e.position.x / float(getWidth()),
                      e.position.y / float(getHeight()) );

    auto p = clampf(ndc);

    switch (activeHandle)
    {
        case DragPoint::Attack: attackPoint = p; break;
        case DragPoint::Decay: decayPoint = p; break;
        case DragPoint::Sustain: sustainPoint = p; break;
        case DragPoint::Release: releasePoint = p; break;
        default: break;
    }

    repaint();
}

void Display::mouseUp(const juce::MouseEvent&)
{
    activeHandle = DragPoint::None;
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
*/

void Display::setVisualStates(std::vector<NoteVisualState*> states)
{
    voiceStates = states;
}

//====================================================================
VectorPluginEditor::VectorPluginEditor(VectorPluginProcessor& p)
//====================================================================
    : AudioProcessorEditor(&p)
    , processor(p)
{
    addAndMakeVisible(display);
    addAndMakeVisible(orbitRadiusSlider);
    addAndMakeVisible(orbitSpeedSlider);

    display.setVisualStates(p.getVisualStates());

    orbitRadiusSlider.setRange(0.0, 1.0);
    orbitSpeedSlider.setRange(0.001, 50.0);

    orbitRadiusSlider.onValueChange =
        [this]
        {
            display.setOrbitRadius(
                (float) orbitRadiusSlider.getValue());
        };

    orbitSpeedSlider.onValueChange =
        [this]
        {
            display.setOrbitSpeed(
                (float) orbitSpeedSlider.getValue());
        };

    orbitRadiusSlider.setValue(.2);
    orbitSpeedSlider.setValue(0.05);

    setSize(900,600);
    startTimerHz(30);
}

void VectorPluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
}

void VectorPluginEditor::resized()
{
    auto area = getLocalBounds();

    auto right = area.removeFromRight(180);

    display.setBounds(area.reduced(10));

    orbitRadiusSlider.setBounds(right.removeFromTop(120).reduced(10));

    orbitSpeedSlider.setBounds(right.removeFromTop(120).reduced(10));
}

void VectorPluginEditor::timerCallback()
{
}
