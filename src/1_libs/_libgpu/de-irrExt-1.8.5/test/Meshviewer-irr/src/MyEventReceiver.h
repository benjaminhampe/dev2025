#include "MyGlobals.h"

/*
To get all the events sent by the GUI Elements, we need to create an event
receiver. This one is really simple. If an event occurs, it checks the id of
the caller and the event type, and starts an action based on these values. For
example, if a menu item with id GUI_ID_OPEN_MODEL was selected, it opens a file-open-dialog.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	bool OnEvent(const SEvent& event) override;

	/*
		Handle key-up events
	*/
	bool OnKeyUp(irr::EKEY_CODE keyCode);

	/*
		Handle "menu item clicked" events.
	*/
	void OnMenuItemSelected( IGUIContextMenu* menu );
	/*
		Handle the event that one of the texture-filters was selected in the corresponding combobox.
	*/
	void OnTextureFilterSelected( IGUIComboBox* combo );
};

