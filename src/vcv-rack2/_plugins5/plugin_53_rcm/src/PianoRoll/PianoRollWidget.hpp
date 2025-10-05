#include <tuple>
#include <limits>

#include "rack.hpp"
#include "../BaseWidget.hpp"
#include "RollAreaWidget.hpp"

using namespace rack;

struct PianoRollModule;
struct PianoRollWidget;
struct ModuleDragType;

enum CopyPasteState {
	COPYREADY,
	PATTERNLOADED,
	MEASURELOADED
};

struct PianoRollWidget : BaseWidget {
	PianoRollModule* module;
	CopyPasteState state;
	RollAreaWidget* rollAreaWidget;

	PianoRollWidget(PianoRollModule *module);

	Rect getRollArea();

	// Event Handlers
	void step() override;
	void appendContextMenu(Menu* menu) override;

};
