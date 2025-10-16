#pragma once
#include <app/common.hpp>
#include <widget/TransparentWidget.hpp>
#include <widget/FramebufferWidget.hpp>
#include <widget/SvgWidget.hpp>
#include <rack_settings.hpp>


namespace rack {
namespace app {


struct RACK_DLL_API PanelBorder : widget::TransparentWidget {
    void draw(const DrawArgs& args) override;
};


struct RACK_DLL_API SvgPanel : widget::Widget {
    widget::FramebufferWidget* fb;
    widget::SvgWidget* sw;
    PanelBorder* panelBorder;
    std::shared_ptr<window::Svg> svg;

    SvgPanel();
    void step() override;
    void setBackground(std::shared_ptr<window::Svg> svg);
};


DEPRECATED typedef SvgPanel SVGPanel;


struct RACK_DLL_API ThemedSvgPanel : SvgPanel {
    std::shared_ptr<window::Svg> lightSvg;
    std::shared_ptr<window::Svg> darkSvg;

    void setBackground(std::shared_ptr<window::Svg> lightSvg, std::shared_ptr<window::Svg> darkSvg) {
        this->lightSvg = lightSvg;
        this->darkSvg = darkSvg;
        SvgPanel::setBackground(settings::preferDarkPanels ? darkSvg : lightSvg);
    }

    void step() override {
        SvgPanel::setBackground(settings::preferDarkPanels ? darkSvg : lightSvg);
        SvgPanel::step();
    }
};


} // namespace app
} // namespace rack
