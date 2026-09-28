#include "MorphDisplay.h"

MorphDisplay::MorphDisplay()
{
    attackPoint  = { 60.0f, 220.0f };
    decayPoint   = { 180.0f, 80.0f };
    sustainPoint = { 350.0f, 120.0f };
    releasePoint = { 520.0f, 220.0f };

    startTimerHz(60);
}

void MorphDisplay::resized()
{
    displayRect =
        getLocalBounds()
            .toFloat()
            .reduced(10.0f);
}

void MorphDisplay::paint(juce::Graphics& g)
{
    drawBackground(g);

    drawEnvelope(g);

    drawHandles(g);

    drawVoices(g);
}

void MorphDisplay::drawBackground(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colours::black);

    g.setColour(
        juce::Colours::darkgrey);

    g.drawRect(displayRect);

    g.drawLine(
        displayRect.getCentreX(),
        displayRect.getY(),
        displayRect.getCentreX(),
        displayRect.getBottom());

    g.drawLine(
        displayRect.getX(),
        displayRect.getCentreY(),
        displayRect.getRight(),
        displayRect.getCentreY());
}

void MorphDisplay::drawEnvelope(
    juce::Graphics& g)
{
    g.setColour(
        juce::Colours::orange);

    g.drawLine(
        attackPoint.x,
        attackPoint.y,
        decayPoint.x,
        decayPoint.y,
        2.0f);

    g.drawLine(
        decayPoint.x,
        decayPoint.y,
        sustainPoint.x,
        sustainPoint.y,
        2.0f);

    g.drawLine(
        sustainPoint.x,
        sustainPoint.y,
        releasePoint.x,
        releasePoint.y,
        2.0f);
}

void MorphDisplay::drawHandles(
    juce::Graphics& g)
{
    g.setColour(
        juce::Colours::yellow);

    auto drawNode =
        juce::Point<float> p
        {
            g.fillEllipse(
                p.x - 6.0f,
                p.y - 6.0f,
                12.0f,
                12.0f);
        };

    drawNode(attackPoint);
    drawNode(decayPoint);
    drawNode(sustainPoint);
    drawNode(releasePoint);
}

juce::Point<float>
MorphDisplay::getCursorPosition(
    const NoteVisualState& voice) const
{
    switch (voice.stage)
    {
        case VisualADSRStage::Attack:
        {
            return attackPoint +
                (decayPoint - attackPoint)
                * voice.stageProgress;
        }

        case VisualADSRStage::Decay:
        {
            return decayPoint +
                (sustainPoint - decayPoint)
                * voice.stageProgress;
        }

        case VisualADSRStage::Sustain:
        {
            return sustainPoint;
        }

        case VisualADSRStage::Release:
        {
            return sustainPoint +
                (releasePoint - sustainPoint)
                * voice.stageProgress;
        }

        case VisualADSRStage::Finished:
        default:
        {
            return releasePoint;
        }
    }
}

void MorphDisplay::drawVoices(
    juce::Graphics& g)
{
    for (const auto& voice : voiceStates)
    {
        if (!voice.active)
            continue;

        auto cursor =
            clampToDisplay(
                getCursorPosition(voice));

        g.setColour(
            juce::Colours::cyan);

        g.fillEllipse(
            cursor.x - 7.0f,
            cursor.y - 7.0f,
            14.0f,
            14.0f);

        const float angle =
            voice.orbiterAngle;

        auto orbiter =
            juce::Point<float>(
                cursor.x
                    + std::cos(angle)
                        * orbitRadius,

                cursor.y
                    + std::sin(angle)
                        * orbitRadius);

        orbiter =
            clampToDisplay(
                orbiter);

        g.setColour(
            juce::Colours::white);

        g.fillEllipse(
            orbiter.x - 4.0f,
            orbiter.y - 4.0f,
            8.0f,
            8.0f);
    }
}

MorphDisplay::DragPoint
MorphDisplay::hitTestHandle(
    juce::Point<float> position)
{
    constexpr float radius = 12.0f;

    if (position.getDistanceFrom(
            attackPoint) < radius)
        return DragPoint::Attack;

    if (position.getDistanceFrom(
            decayPoint) < radius)
        return DragPoint::Decay;

    if (position.getDistanceFrom(
            sustainPoint) < radius)
        return DragPoint::Sustain;

    if (position.getDistanceFrom(
            releasePoint) < radius)
        return DragPoint::Release;

    return DragPoint::None;
}

void MorphDisplay::mouseDown(
    const juce::MouseEvent& e)
{
    activeHandle =
        hitTestHandle(
            e.position);
}

void MorphDisplay::mouseDrag(
    const juce::MouseEvent& e)
{
    auto p =
        clampToDisplay(
            e.position);

    switch (activeHandle)
    {
        case DragPoint::Attack:
            attackPoint = p;
            break;

        case DragPoint::Decay:
            decayPoint = p;
            break;

        case DragPoint::Sustain:
            sustainPoint = p;
            break;

        case DragPoint::Release:
            releasePoint = p;
            break;

        default:
            break;
    }

    repaint();
}

void MorphDisplay::mouseUp(
    const juce::MouseEvent&)
{
    activeHandle =
        DragPoint::None;
}

void MorphDisplay::timerCallback()
{
    repaint();
}

void MorphDisplay::setOrbitRadius(
    float radius)
{
    orbitRadius = radius;
}

void MorphDisplay::setOrbitSpeed(
    float speed)
{
    orbitSpeed = speed;
}

void MorphDisplay::setVisualStates(
    const std::array<
        NoteVisualState,
        32>& states)
{
    voiceStates = states;
}

juce::Point<float>
MorphDisplay::clampToDisplay(
    juce::Point<float> point) const
{
    point.x =
        juce::jlimit(
            displayRect.getX(),
            displayRect.getRight(),
            point.x);

    point.y =
        juce::jlimit(
            displayRect.getY(),
            displayRect.getBottom(),
            point.y);

    return point;
}