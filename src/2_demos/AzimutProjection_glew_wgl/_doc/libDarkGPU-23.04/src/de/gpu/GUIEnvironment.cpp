#include "GUIEnvironment.hpp"
#include "VideoDriver.hpp"

namespace de {
namespace gpu {


GUIButton::GUIButton( GUI* gui, GUIElement* parent, int id )
      : GUIElement(gui,parent,id)
{
}

void
GUIButton::render()
{

}

GUIText::GUIText( GUI* gui, GUIElement* parent, int id )
      : GUIElement(gui,parent,id)
{
}

void
GUIText::render()
{

}


} // end namespace gpu.
} // end namespace de.
