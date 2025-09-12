
/*
  ==============================================================================

    FoldableComponent.cpp
    Created: 4 Feb 2016 12:51:20pm
    Author:  Juan Cabello

  ==============================================================================
*/

#include "FoldablePanel.h"

struct FoldablePanel::SectionComponent  : public Component
{
    // TODO: add color and height of component
    SectionComponent (const String& sectionTitle,
                      Component* const newPanel,
                      const Colour sectionColour,
                      const int sectionHeight,
                      ParamStepped<eSectionState>* sectionState)
    : Component (sectionTitle)
    , positionIndex(0)
    , titleHeight (28)
    , isOpen (sectionState->getStep() == eSectionState::eExpanded)
    , _sectionHeight(sectionHeight + titleHeight)
    , _sectionState(sectionState)
    , _sectionColour(sectionColour)
    {
        jassert(sectionTitle.isNotEmpty());
        addPanel (newPanel);
    }

    ~SectionComponent() override
    {
        panels.clear();
        positionIndex = 0;
    }

    void paint(Graphics& g) override
    {
        if (titleHeight > 0)
        {
            getLookAndFeel().drawPropertyPanelSectionHeader(g, getName(), isOpen, getWidth(), titleHeight);

            const float buttonSize = titleHeight * 0.65f;
            const float buttonIndent = (titleHeight - buttonSize) * 0.5f;

            // draw section header text
            const int textX = int(buttonIndent * 4.0f + buttonSize);
            g.setColour(getSectionColour().withSaturation(.5f).brighter(0.3f));
            g.setFont(Font(titleHeight * 0.7f, Font::plain));
            g.drawText(getName(), textX, -1, getWidth() - textX - 4, titleHeight, Justification::centredLeft, true);
        }
    }

    void addPanel(Component* const newPanel)
    {
        panels.insert(positionIndex++, newPanel);
        addAndMakeVisible (newPanel, 0);
        resized();
    }

    void resized() override
    {
        int x = 0;
        for (int i = 0; i < panels.size(); ++i ) {
            Component* const panel = panels.getUnchecked(i);
            panel->setBounds(x, titleHeight, panel->getWidth(), panel->getHeight());
            x += panel->getWidth();
        }
    }

    void setOpen (const bool open)
    {
        if (isOpen != open)
        {
            isOpen = open;
            _sectionState->setStep(open ? eSectionState::eExpanded : eSectionState::eCollapsed);
            for (int i = 0; i < panels.size(); ++i ) {
                Component* const panel = panels.getUnchecked(i);
                panel->setVisible(open);
            }

            if (FoldablePanel* const pp = findParentComponentOfClass<FoldablePanel>()) {
                pp->resized();

                pp->getBackToPoint(getY(), (open) ? getSectionHeight() - 22 : 0);
            }
        }
    }

    void mouseUp (const MouseEvent& e) override
    {
        if (e.getMouseDownX() < titleHeight
            && e.x < titleHeight
            && e.getNumberOfClicks() != 2)
            mouseDoubleClick (e);
    }

    void mouseDoubleClick (const MouseEvent& e) override
    {
        if (e.y < titleHeight) {
            setOpen (! isOpen);
        }

    }

    Colour getSectionColour()
    {
        return _sectionColour;
    }

    int getSectionHeight()
    {
        return (isOpen ? _sectionHeight : titleHeight);
    }

    void updateOpennessState()
    {
        if (_sectionState->isUIDirty()) {
            setOpen(_sectionState->getStep() == eSectionState::eExpanded);
        }
    }

    int positionIndex;
    OwnedArray<Component> panels;
    const int titleHeight;
    bool isOpen;
    const int _sectionHeight;
    ParamStepped<eSectionState>* _sectionState;
    const Colour _sectionColour;

    JUCE_DECLARE_NON_COPYABLE (SectionComponent)
};

//==============================================================================
struct FoldablePanel::PanelHolderComponent  : public Component, private Timer
{
    PanelHolderComponent()
    {
        startTimerHz(60);
    }

    void paint (Graphics& g) override
    {
        const int titleHeight = 22;
        int y = 0;

        // TODO: at each panel some space is cutoff at bottom of original panel
        // see NOTE in PlugUI.cpp
        for (int i = 0; i < sections.size(); ++i)
        {
            SectionComponent* const section = sections.getUnchecked(i);

            Rectangle<int> content (0, y + 22, getWidth(), section->getSectionHeight() - titleHeight);
            y = section->getBottom();

            g.setColour(section->getSectionColour());
            g.fillRect (content);
        }
    }

    void updateLayout (int width)
    {
        int y = 0;

        for (int i = 0; i < sections.size(); ++i)
        {
            SectionComponent* const section = sections.getUnchecked(i);
            section->setBounds (0, y, width, section->getSectionHeight());
            y = section->getBottom();
        }

        setSize(width, y);

        repaint();
    }

    void insertSection (int indexToInsertAt, SectionComponent* newSection)
    {
        sections.insert (indexToInsertAt, newSection);
        addAndMakeVisible (newSection, 0);
    }

    SectionComponent* getSection (const int targetIndex) const noexcept
    {
        return sections.getUnchecked (targetIndex);
    }

    void timerCallback() override
    {
        for (int i = 0; i < sections.size(); ++i) {
            SectionComponent* const section = sections.getUnchecked(i);
            section->updateOpennessState();
        }
    }

    OwnedArray<SectionComponent> sections;

    JUCE_DECLARE_NON_COPYABLE (PanelHolderComponent)
};

FoldablePanel::FoldablePanel (const String& name)  : Component (name)
{
    addAndMakeVisible (viewport);
    viewport.setViewedComponent (panelHolderComponent = new PanelHolderComponent());
    viewport.setScrollBarsShown(true, false, false, false);
    ScrollBar* scrollbarY = &viewport.getVerticalScrollBar();
    scrollbarY->setAutoHide(false);
    viewport.setScrollBarThickness(10);
    viewport.setFocusContainer(true);
}

FoldablePanel::~FoldablePanel()
{
    clear();
}

void FoldablePanel::paint (Graphics& /*g*/)
{
}

void FoldablePanel::resized()
{
    lastPosY = viewport.getViewPositionY();
    viewport.setBounds (getLocalBounds());

    panelHolderComponent->setBounds (viewport.getLocalBounds());
    panelHolderComponent->updateLayout (viewport.getWidth());
}

bool FoldablePanel::isEmpty() const
{
    return (panelHolderComponent->sections.size() == 0);
}

void FoldablePanel::clear()
{
    if (! isEmpty())
    {
        panelHolderComponent->sections.clear();
        updateLayout();
    }
}

void FoldablePanel::addSection (const String& sectionTitle,
                                Component* const newPanel,
                                const Colour sectionColour,
                                const int sectionHeight,
                                ParamStepped<eSectionState>* sectionState,
                                const int indexToInsertAt)
{
    jassert (sectionTitle.isNotEmpty());

    if (isEmpty()) {
        repaint();
    }

    panelHolderComponent->insertSection (indexToInsertAt, new SectionComponent (sectionTitle, newPanel, sectionColour, sectionHeight, sectionState));
    resized();
    updateLayout();
}

void FoldablePanel::getBackToPoint(const int y, int height)
{
    if (lastPosY + y + height > viewport.getViewHeight()) {
        viewport.setViewPosition(viewport.getX(), lastPosY + height);
    }
}

void FoldablePanel::addPanel(const int sectionIndex, Component* const newPanel)
{
    if (SectionComponent* s = panelHolderComponent->getSection (sectionIndex)) {
        s->addPanel(newPanel);
    }
}

void FoldablePanel::updateLayout() const
{
    panelHolderComponent->updateLayout (getWidth());
}
